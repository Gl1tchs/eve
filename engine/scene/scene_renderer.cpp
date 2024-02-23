#include "scene/scene_renderer.h"

#include "asset/asset_registry.h"
#include "core/color.h"
#include "renderer/frame_buffer.h"
#include "renderer/post_processor.h"
#include "renderer/render_command.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/scene_manager.h"
#include "scene/transform.h"

SceneRenderer::SceneRenderer() : viewport_size(0, 0) {
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

	Entity camera = [scene]() -> Entity {
		const auto view = scene->view<CameraComponent>();
		const auto camera_it = std::find_if(
				view.begin(), view.end(), [&](const auto& entity_handle) {
					const Entity camera_candidate{ entity_handle, scene.get() };
					return camera_candidate &&
							camera_candidate.has_component<CameraComponent>();
				});

		if (camera_it != view.end()) {
			return { *camera_it, scene.get() };
		}

		return INVALID_ENTITY;
	}();

	if (camera) {
		const auto& cc = camera.get_component<CameraComponent>();
		const auto& tc = camera.get_transform();

		CameraData data = {
			.view = cc.camera.get_view_matrix(tc),
			.proj = cc.camera.get_projection_matrix(),
			.zoom_level = cc.camera.zoom_level,
			.aspect_ratio = cc.camera.aspect_ratio,
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
		.view = editor_camera->get_view_matrix(),
		.proj = editor_camera->get_projection_matrix(),
		.zoom_level = editor_camera->zoom_level,
		.aspect_ratio = editor_camera->aspect_ratio,
	};

	_render_scene(data);

	// TODO if scene_settings.show_post_processing
	_post_process();
}

void SceneRenderer::on_viewport_resize(glm::uvec2 size) {
	if (size.x == 0.0f || size.y == 0.0f) {
		return;
	}

	viewport_size = size;

	//! TODO make this cached
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

void SceneRenderer::submit(
		const RenderFuncTickFormat format, const RenderFunc& function) {
	render_functions[format].push_back(function);
}

uint32_t SceneRenderer::get_final_texture_id() const {
	return post_processed ? post_processor->get_frame_buffer_renderer_id()
						  : frame_buffer->get_color_attachment_renderer_id(0);
}

void SceneRenderer::_render_scene(const CameraData& camera_data) {
	const auto scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	EVE_PROFILE_FUNCTION();

	renderer::reset_stats();

	frame_buffer->bind();
	{
		RenderCommand::set_depth_testing(true);

		RenderCommand::set_clear_color(COLOR_GRAY);
		RenderCommand::clear(BUFFER_BITS_COLOR | BUFFER_BITS_DEPTH);

		int attachment_data = -1;
		frame_buffer->clear_attachment(1, &attachment_data);

		for (const auto function :
				render_functions[RenderFuncTickFormat::BEFORE_RENDER]) {
			function(frame_buffer);
		}
		render_functions[RenderFuncTickFormat::BEFORE_RENDER].clear();

		renderer::begin_pass(camera_data);
		{
			scene->view<Transform, SpriteRenderer>().each(
					[this, scene](entt::entity entity_id,
							const Transform& transform,
							const SpriteRenderer& sprite) {
						Ref<Texture2D> texture =
								scene->get_asset_registry()
										.get_asset<Texture2D>(sprite.texture);

						renderer::draw_quad(transform, texture, sprite.color,
								sprite.tex_tiling, (uint32_t)entity_id);
					});

			scene->view<Transform, TextRenderer>().each(
					[this, scene](entt::entity entity_id,
							const Transform& transform,
							const TextRenderer& text_component) {
						Ref<Font> font =
								scene->get_asset_registry().get_asset<Font>(
										text_component.font);
						if (!font) {
							font = Font::get_default();
						}

						renderer::draw_text(text_component.text,
								font ? font : Font::get_default(), transform,
								text_component.fg_color,
								text_component.bg_color, text_component.kerning,
								text_component.line_spacing,
								text_component.is_screen_space,
								(uint32_t)entity_id);
					});

			for (const auto function :
					render_functions[RenderFuncTickFormat::ON_RENDER]) {
				function(frame_buffer);
			}
			render_functions[RenderFuncTickFormat::ON_RENDER].clear();
		}
		renderer::end_pass();

		for (const auto function :
				render_functions[RenderFuncTickFormat::AFTER_RENDER]) {
			function(frame_buffer);
		}
		render_functions[RenderFuncTickFormat::AFTER_RENDER].clear();
	}

	frame_buffer->unbind();
}

void SceneRenderer::_post_process() {
	post_processed = false;

	const auto scene = SceneManager::get_active();

	const auto is_global_volume = [scene](const auto& entity_id) {
		return scene->get_component<PostProcessVolume>(entity_id).is_global;
	};

	const auto view = scene->view<PostProcessVolume>();

	// TODO implement local effects
	for (const auto& entity_id : view | std::views::filter(is_global_volume)) {
		const PostProcessVolume& volume =
				scene->get_component<PostProcessVolume>(entity_id);
		post_processed = post_processor->process(frame_buffer, volume);
	}
}
