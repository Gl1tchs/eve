#include "renderer/post_processor.h"

#include "renderer/frame_buffer.h"
#include "renderer/renderer_api.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/vertex_array.h"

PostProcessor::PostProcessor() {
	vertex_array = create_ref<VertexArray>();

	// TODO maybe use single shader?
	screen_shader = create_ref<Shader>(
			"shaders/post-processing/screen.vert",
			"shaders/post-processing/screen.frag");

	gray_scale_shader = create_ref<Shader>(
			"shaders/post-processing/screen.vert",
			"shaders/post-processing/gray_scale.frag");

	chromatic_aberration_shader = create_ref<Shader>(
			"shaders/post-processing/screen.vert",
			"shaders/post-processing/chromatic-aberration.frag");

	blur_shader = create_ref<Shader>(
			"shaders/post-processing/screen.vert",
			"shaders/post-processing/blur.frag");

	sharpen_shader = create_ref<Shader>(
			"shaders/post-processing/screen.vert",
			"shaders/post-processing/sharpen.frag");

	vignette_shader = create_ref<Shader>(
			"shaders/post-processing/screen.vert",
			"shaders/post-processing/vignette.frag");

	FrameBufferCreateInfo fb_info;
	fb_info.attachments = {
		FrameBufferTextureFormat::RGBA8,
	};
	fb_info.width = 1280;
	fb_info.height = 768;
	frame_buffer = create_ref<FrameBuffer>(fb_info);
}

void PostProcessor::process(uint16_t effects, const Ref<FrameBuffer>& screen_buffer) {
	const auto screen_size = screen_buffer->get_size();
	frame_buffer->resize(screen_size.x, screen_size.y);

	frame_buffer->bind();

	RendererAPI::set_depth_testing(false);

	RendererAPI::set_clear_color(COLOR_GRAY);
	RendererAPI::clear(BUFFER_BITS_COLOR);

	last_texture_id = screen_buffer->get_color_attachment_renderer_id(0);
	effect_provided = false;

	// TODO maybe calculate everything seperatly then add the values
	if (effects & POST_PROCESSING_EFFECT_GRAY_SCALE) {
		_process_gray_scale(last_texture_id);

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	if (effects & POST_PROCESSING_EFFECT_CHROMATIC_ABERRATION) {
		_process_chromatic_aberration(last_texture_id);

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	if (effects & POST_PROCESSING_EFFECT_BLUR) {
		_process_blur(last_texture_id);

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	if (effects & POST_PROCESSING_EFFECT_SHARPEN) {
		_process_sharpen(last_texture_id);

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	if (effects & POST_PROCESSING_EFFECT_VIGNETTE) {
		_process_vignette(last_texture_id);

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	// if no effects provided draw default screen shader
	if (!effect_provided) {
		_process_empty(last_texture_id);
	}

	frame_buffer->unbind();
}

uint32_t PostProcessor::get_frame_buffer_renderer_id() const {
	return frame_buffer->get_color_attachment_renderer_id(0);
}

PostProcessorSettings& PostProcessor::get_settings() {
	return settings;
}

void PostProcessor::set_settings(const PostProcessorSettings& _settings) {
	settings = _settings;
}

void PostProcessor::_process_gray_scale(uint32_t texture_id) {
	gray_scale_shader->bind();

	RendererAPI::bind_texture(texture_id);
	RendererAPI::draw_arrays(vertex_array, 6);
}

void PostProcessor::_process_chromatic_aberration(uint32_t texture_id) {
	chromatic_aberration_shader->bind();

	glm::vec3 offset;
	static_assert(sizeof(glm::vec3) == sizeof(PostProcessorSettings::ChromaticAberrationSettings));
	memcpy(&offset, &settings.chromatic_aberration, sizeof(glm::vec3));

	chromatic_aberration_shader->set_uniform("u_offset", offset);

	RendererAPI::bind_texture(texture_id);
	RendererAPI::draw_arrays(vertex_array, 6);
}

void PostProcessor::_process_blur(uint32_t texture_id) {
	blur_shader->bind();

	blur_shader->set_uniform("u_size", (int)settings.blur.size);
	blur_shader->set_uniform("u_seperation", settings.blur.seperation);

	RendererAPI::bind_texture(texture_id);
	RendererAPI::draw_arrays(vertex_array, 6);
}

void PostProcessor::_process_sharpen(uint32_t texture_id) {
	sharpen_shader->bind();

	sharpen_shader->set_uniform("u_amount", settings.sharpen.amount);

	RendererAPI::bind_texture(texture_id);
	RendererAPI::draw_arrays(vertex_array, 6);
}

void PostProcessor::_process_vignette(uint32_t texture_id) {
	vignette_shader->bind();

	vignette_shader->set_uniform("u_inner", settings.vignette.inner);
	vignette_shader->set_uniform("u_outer", settings.vignette.outer);
	vignette_shader->set_uniform("u_strength", settings.vignette.strength);
	vignette_shader->set_uniform("u_curvature", settings.vignette.curvature);

	RendererAPI::bind_texture(texture_id);
	RendererAPI::draw_arrays(vertex_array, 6);
}

void PostProcessor::_process_empty(uint32_t screen_texture) {
	screen_shader->bind();

	RendererAPI::bind_texture(screen_texture);
	RendererAPI::draw_arrays(vertex_array, 6);
}
