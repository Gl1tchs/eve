#include "scene/scene_renderer.h"

#include "asset/asset_registry.h"
#include "core/color.h"
#include "renderer/frame_buffer.h"
#include "renderer/post_processor.h"
#include "renderer/renderer_api.h"
#include "renderer/texture.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/scene_manager.h"
#include "scene/transform.h"

SceneRenderer::SceneRenderer(Ref<Renderer> renderer) :
		renderer(renderer), viewport_size(0, 0) {
	FrameBufferCreateInfo fb_info;
	fb_info.attachments = {
		FrameBufferTextureFormat::RGBA8,
		FrameBufferTextureFormat::RED_INT,
		FrameBufferTextureFormat::DEPTH24_STENCIL8,
	};
	fb_info.width = 1280;
	fb_info.height = 768;
	frame_buffer = create_ref<FrameBuffer>(fb_info);

	post_processor = create_ref<PostProcessor>();
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

		_post_process();
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

	// TODO if scene_settings.show_post_processing
	_post_process();
}

void SceneRenderer::on_viewport_resize(glm::uvec2 size) {
	if (viewport_size == size) {
		return;
	}

	viewport_size = size;

	frame_buffer->resize(viewport_size.x, viewport_size.y);

	const auto scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	scene->view<CameraComponent>().each(
			[size](entt::entity, CameraComponent& cc) {
				if (cc.is_fixed_aspect_ratio) {
					return;
				}

				cc.camera.aspect_ratio = ((float)size.x / (float)size.y);
			});
}

void SceneRenderer::push_beheaviour(const RenderBeheaviourTickFormat format, const RenderFunc& function) {
	switch (format) {
		case RenderBeheaviourTickFormat::BEFORE_RENDER:
			before_render_functions.push_back(function);
			break;
		case RenderBeheaviourTickFormat::ON_RENDER:
			on_render_functions.push_back(function);
			break;
		case RenderBeheaviourTickFormat::AFTER_RENDER:
			after_render_functions.push_back(function);
			break;
		default:
			break;
	}
}

uint32_t SceneRenderer::get_final_texture_id() const {
	return post_processed
			? post_processor->get_frame_buffer_renderer_id()
			: frame_buffer->get_color_attachment_renderer_id(0);
}

void SceneRenderer::_render_scene(const CameraData& camera_data) {
	const auto scene = SceneManager::get_active();

	renderer->reset_stats();

	frame_buffer->bind();
	{
		RendererAPI::set_depth_testing(true);

		RendererAPI::set_clear_color(COLOR_GRAY);
		RendererAPI::clear(BUFFER_BITS_COLOR | BUFFER_BITS_DEPTH);

		int attachment_data = -1;
		frame_buffer->clear_attachment(1, &attachment_data);

		for (const auto function : before_render_functions) {
			function(frame_buffer);
		}

		renderer->begin_pass(camera_data);
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

			for (const auto function : on_render_functions) {
				function(frame_buffer);
			}
		}
		renderer->end_pass();

		for (const auto function : after_render_functions) {
			function(frame_buffer);
		}
	}

	frame_buffer->unbind();
}

void SceneRenderer::_post_process() {
	post_processed = false;

	const auto scene = SceneManager::get_active();

	auto view = scene->view<PostProcessVolume>();
	for (const entt::entity entity_id : view) {
		const PostProcessVolume& volume = scene->get_component<PostProcessVolume>(entity_id);
		// TODO implement local effects
		if (volume.is_global) {
			post_processed = post_processor->process(frame_buffer, volume);
		}
	}
}
