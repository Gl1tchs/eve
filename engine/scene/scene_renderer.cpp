#include "scene/scene_renderer.h"

#include "asset/asset_registry.h"
#include "core/color.h"
#include "renderer/texture.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/scene_manager.h"
#include "scene/transform.h"

SceneRenderer::SceneRenderer(Ref<Renderer> renderer) :
		renderer(renderer), viewport_size(0, 0) {
}

void SceneRenderer::render_runtime(float ds) {
	const auto scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	Entity camera;

	auto view = scene->view<CameraComponent>();
	for (auto entity_handle : view) {
		const Entity camera_candidate{ entity_handle, scene.get() };
		if (camera_candidate && camera_candidate.has_component<CameraComponent>()) {
			camera = camera_candidate;
		}
	}

	if (camera) {
		const auto& cc = camera.get_component<CameraComponent>();
		const auto& tc = camera.get_transform();

		CameraData data = { cc.camera.get_view_matrix(tc),
			cc.camera.get_projection_matrix(), tc.get_position() };

		_render_scene(data);
	}
}

void SceneRenderer::render_editor(float ds, Ref<EditorCamera>& editor_camera) {
	const auto scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	CameraData data = { editor_camera->get_view_matrix(),
		editor_camera->get_projection_matrix(),
		editor_camera->get_transform().get_position() };

	_render_scene(data);
}

void SceneRenderer::on_viewport_resize(glm::uvec2 size) {
	const auto scene = SceneManager::get_active();
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
	const auto scene = SceneManager::get_active();

	renderer->begin_pass(data);
	{
		scene->view<TransformComponent, SpriteRendererComponent>().each(
				[this](entt::entity entity_id, const TransformComponent& transform,
						const SpriteRendererComponent& sprite) {
					renderer->draw_sprite(sprite, transform, (uint32_t)entity_id);
				});

		scene->view<TransformComponent, TextRendererComponent>().each(
				[this](entt::entity entity_id, const TransformComponent& transform,
						const TextRendererComponent& text_component) {
					renderer->draw_text(text_component, transform, (uint32_t)entity_id);
				});
	}
	renderer->end_pass();
}
