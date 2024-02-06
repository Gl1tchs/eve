#include "scene/scene_renderer.h"

#include "asset/asset_registry.h"
#include "core/color.h"
#include "renderer/frame_buffer.h"
#include "renderer/post_processor.h"
#include "renderer/primitives/text.h"
#include "renderer/render_command.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/scene_manager.h"
#include "scene/transform.h"

SceneRenderer::SceneRenderer() :
		viewport_size(0, 0) {
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

		CameraData data = {
			cc.camera.get_view_matrix(tc),
			cc.camera.get_projection_matrix(),
			cc.camera.zoom_level
		};

		_render_scene(data);

		_post_process();
	} else {
		// TODO render no camera text center of the screen

		// clear the screen
		frame_buffer->bind();
		{
			RenderCommand::set_depth_testing(true);

			RenderCommand::set_clear_color(COLOR_GRAY);
			RenderCommand::clear(BUFFER_BITS_COLOR | BUFFER_BITS_DEPTH);
		}
		frame_buffer->unbind();

		post_processed = false;
	}
}

void SceneRenderer::render_editor(float ds, Ref<EditorCamera>& editor_camera) {
	const auto scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	CameraData data = {
		editor_camera->get_view_matrix(),
		editor_camera->get_projection_matrix(),
		editor_camera->zoom_level
	};

	// TODO this should not be affected by post processing
	submit(RenderFuncTickFormat::ON_RENDER, [this, &scene](const Ref<FrameBuffer>& fb) {
		for (auto entity : scene->get_selected_entities()) {
			_render_entity_bounds(entity);
		}
	});

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

void SceneRenderer::submit(const RenderFuncTickFormat format, const RenderFunc& function) {
	switch (format) {
		case RenderFuncTickFormat::BEFORE_RENDER:
			before_render_functions.push_back(function);
			break;
		case RenderFuncTickFormat::ON_RENDER:
			on_render_functions.push_back(function);
			break;
		case RenderFuncTickFormat::AFTER_RENDER:
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
	if (!scene) {
		return;
	}

	EVE_PROFILE_FUNCTION();

	Renderer::reset_stats();

	frame_buffer->bind();
	{
		RenderCommand::set_depth_testing(true);

		RenderCommand::set_clear_color(COLOR_GRAY);
		RenderCommand::clear(BUFFER_BITS_COLOR | BUFFER_BITS_DEPTH);

		int attachment_data = -1;
		frame_buffer->clear_attachment(1, &attachment_data);

		for (const auto function : before_render_functions) {
			function(frame_buffer);
		}
		before_render_functions.clear();

		Renderer::begin_pass(camera_data);
		{
			scene->view<Transform, SpriteRenderer>().each(
					[this](entt::entity entity_id, const Transform& transform,
							const SpriteRenderer& sprite) {
						Renderer::draw_sprite(sprite, transform, (uint32_t)entity_id);
					});

			scene->view<Transform, TextRenderer>().each(
					[this](entt::entity entity_id, const Transform& transform,
							const TextRenderer& text_component) {
						Renderer::draw_text(text_component, transform, (uint32_t)entity_id);
					});

			for (const auto function : on_render_functions) {
				function(frame_buffer);
			}
			on_render_functions.clear();
		}
		Renderer::end_pass();

		for (const auto function : after_render_functions) {
			function(frame_buffer);
		}
		after_render_functions.clear();
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

void SceneRenderer::_render_entity_bounds(Entity entity) {
	const Transform& transform = entity.get_transform();

	if (entity.has_component<SpriteRenderer>()) {
		Renderer::draw_box(transform, COLOR_GREEN);
	}
	if (entity.has_component<TextRenderer>()) {
		const TextRenderer& text_renderer = entity.get_component<TextRenderer>();
		if (text_renderer.is_screen_space) {
			return;
		}

		Transform text_transform = transform;
		glm::vec2 scale = text_transform.get_scale();

		Ref<Font> font = AssetRegistry::get<Font>(text_renderer.font);

		glm::vec2 text_size = get_text_size(text_renderer.text, font, text_renderer.kerning) * scale;

		text_transform.local_position.y += (scale.y / 2.0f) - (text_size.y / 2.0f);

		text_transform.local_scale.x = text_size.x;
		text_transform.local_scale.y += text_size.y;

		Renderer::draw_box(text_transform, COLOR_GREEN);
	}

	for (auto child : entity.get_children()) {
		_render_entity_bounds(child);
	}
}
