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

bool PostProcessor::process(const Ref<FrameBuffer>& screen_buffer, const PostProcessingVolume& _volume) {
	volume = _volume;

	if (!volume.is_any_effect_provided()) {
		return false;
	}

	const auto screen_size = screen_buffer->get_size();
	if (screen_size != frame_buffer->get_size()) {
		frame_buffer->resize(screen_size.x, screen_size.y);
	}

	frame_buffer->bind();

	RendererAPI::set_depth_testing(false);

	RendererAPI::set_clear_color(COLOR_GRAY);
	RendererAPI::clear(BUFFER_BITS_COLOR);

	last_texture_id = screen_buffer->get_color_attachment_renderer_id(0);
	effect_provided = false;

	// TODO maybe calculate everything seperatly then add the values
	if (volume.gray_scale.enabled) {
		_process_gray_scale(last_texture_id);

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	if (volume.chromatic_aberration.enabled) {
		_process_chromatic_aberration(last_texture_id);

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	if (volume.blur.enabled) {
		_process_blur(last_texture_id);

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	if (volume.sharpen.enabled) {
		_process_sharpen(last_texture_id);

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	if (volume.vignette.enabled) {
		_process_vignette(last_texture_id);

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	frame_buffer->unbind();

	return true;
}

Ref<FrameBuffer> PostProcessor::get_frame_buffer() {
	return frame_buffer;
}

uint32_t PostProcessor::get_frame_buffer_renderer_id() const {
	return frame_buffer->get_color_attachment_renderer_id(0);
}

void PostProcessor::_process_gray_scale(uint32_t texture_id) {
	gray_scale_shader->bind();

	RendererAPI::bind_texture(texture_id);
	RendererAPI::draw_arrays(vertex_array, 6);
}

void PostProcessor::_process_chromatic_aberration(uint32_t texture_id) {
	chromatic_aberration_shader->bind();

	glm::vec3 offset{
		volume.chromatic_aberration.red_offset,
		volume.chromatic_aberration.green_offset,
		volume.chromatic_aberration.blue_offset
	};

	chromatic_aberration_shader->set_uniform("u_offset", offset);

	RendererAPI::bind_texture(texture_id);
	RendererAPI::draw_arrays(vertex_array, 6);
}

void PostProcessor::_process_blur(uint32_t texture_id) {
	blur_shader->bind();

	blur_shader->set_uniform("u_size", (int)volume.blur.size);
	blur_shader->set_uniform("u_seperation", volume.blur.seperation);

	RendererAPI::bind_texture(texture_id);
	RendererAPI::draw_arrays(vertex_array, 6);
}

void PostProcessor::_process_sharpen(uint32_t texture_id) {
	sharpen_shader->bind();

	sharpen_shader->set_uniform("u_amount", volume.sharpen.amount);

	RendererAPI::bind_texture(texture_id);
	RendererAPI::draw_arrays(vertex_array, 6);
}

void PostProcessor::_process_vignette(uint32_t texture_id) {
	vignette_shader->bind();

	vignette_shader->set_uniform("u_inner", volume.vignette.inner);
	vignette_shader->set_uniform("u_outer", volume.vignette.outer);
	vignette_shader->set_uniform("u_strength", volume.vignette.strength);
	vignette_shader->set_uniform("u_curvature", volume.vignette.curvature);

	RendererAPI::bind_texture(texture_id);
	RendererAPI::draw_arrays(vertex_array, 6);
}

void PostProcessor::_process_empty(uint32_t screen_texture) {
	screen_shader->bind();

	RendererAPI::bind_texture(screen_texture);
	RendererAPI::draw_arrays(vertex_array, 6);
}
