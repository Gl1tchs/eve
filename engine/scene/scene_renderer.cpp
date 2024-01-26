#include "scene/scene_renderer.h"

#include "renderer/renderer_api.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/scene_manager.h"
#include "scene/transform.h"
#include "scene_renderer.h"

SceneRenderer::SceneRenderer(Ref<Renderer> renderer) :
		renderer(renderer), viewport_size(0, 0) {
}

void SceneRenderer::render_runtime(float ds) {
	auto scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	Entity camera;

	auto view = scene->view<CameraComponent>();
	for (auto entity_handle : view) {
		Entity camera_candidate{ entity_handle, scene.get() };
		if (camera_candidate && camera_candidate.has_component<CameraComponent>()) {
			camera = camera_candidate;
		}
	}

	if (camera) {
		auto& cc = camera.get_component<CameraComponent>();
		auto& tc = camera.get_transform();

		CameraData data = { cc.camera.get_view_matrix(tc),
			cc.camera.get_projection_matrix(), tc.get_position() };

		_render_scene(data);
	}
}

void SceneRenderer::render_editor(float ds, Ref<EditorCamera>& editor_camera) {
	auto scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	CameraData data = { editor_camera->get_view_matrix(),
		editor_camera->get_projection_matrix(),
		editor_camera->get_transform().get_position() };

	_render_scene(data);
}

void SceneRenderer::on_viewport_resize(glm::uvec2 size) {
	auto scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	viewport_size = size;

	scene->view<CameraComponent>().each(
			[size](entt::entity, CameraComponent& cc) {
				if (cc.is_fixed_aspect_ratio) {
					return;
				}

				cc.camera.aspect_ratio = ((float)size.x / (float)size.y);
			});
}

void SceneRenderer::_render_scene(const CameraData& data) {
	auto scene = SceneManager::get_active();

	RendererAPI::set_viewport(0, 0, viewport_size.x, viewport_size.y);
	RendererAPI::set_clear_color(COLOR_GRAY);
	RendererAPI::clear(BUFFER_BITS_COLOR | BUFFER_BITS_DEPTH);

	renderer->begin_pass(data);

	scene->view<TransformComponent, SpriteRendererComponent>().each(
			[&](entt::entity entity_id, const TransformComponent& transform,
					const SpriteRendererComponent& sprite) {
				Entity entity{ entity_id, scene.get() };

				renderer->draw_quad(
						transform,
						sprite.texture,
						sprite.color,
						sprite.tex_tiling);
			});

	scene->view<TransformComponent, TextRendererComponent>().each(
			[&](entt::entity entity_id, const TransformComponent& transform,
					const TextRendererComponent& text_component) {
				Entity entity{ entity_id, scene.get() };

				renderer->draw_string(
						text_component.text,
						text_component.font ? text_component.font : Font::get_default(),
						transform,
						text_component.fg_color,
						text_component.bg_color,
						text_component.kerning,
						text_component.line_spacing);
			});

	renderer->end_pass();
}
