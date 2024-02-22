#include "renderer/post_processor.h"

#include "post_processor.h"
#include "renderer/frame_buffer.h"
#include "renderer/render_command.h"
#include "renderer/shader.h"
#include "renderer/vertex_array.h"

PostProcessor::PostProcessor() {
	EVE_PROFILE_FUNCTION();

	vertex_array = create_ref<VertexArray>();

	// TODO maybe use single shader?
	gray_scale_shader = create_ref<Shader>(
			"shaders/post-processing/screen.vert.spv",
			"shaders/post-processing/gray-scale.frag.spv");

	chromatic_aberration_shader = create_ref<Shader>(
			"shaders/post-processing/screen.vert.spv",
			"shaders/post-processing/chromatic-aberration.frag.spv");

	blur_shader = create_ref<Shader>(
			"shaders/post-processing/screen.vert.spv",
			"shaders/post-processing/blur.frag.spv");

	sharpen_shader = create_ref<Shader>(
			"shaders/post-processing/screen.vert.spv",
			"shaders/post-processing/sharpen.frag.spv");

	vignette_shader = create_ref<Shader>(
			"shaders/post-processing/screen.vert.spv",
			"shaders/post-processing/vignette.frag.spv");

	FrameBufferCreateInfo fb_info;
	fb_info.width = 1280;
	fb_info.height = 768;
	fb_info.attachments = {
		FrameBufferTextureFormat::RGBA8,
	};

	frame_buffer = create_ref<FrameBuffer>(fb_info);
}

bool PostProcessor::process(const Ref<FrameBuffer>& screen_buffer, const PostProcessVolume& _volume) {
	EVE_PROFILE_FUNCTION();

	volume = _volume;

	if (!volume.is_any_effect_provided()) {
		return false;
	}

	const auto screen_size = screen_buffer->get_size();
	if (screen_size != frame_buffer->get_size()) {
		frame_buffer->resize(screen_size.x, screen_size.y);
	}

	frame_buffer->bind();

	RenderCommand::set_depth_testing(false);

	RenderCommand::set_clear_color(COLOR_GRAY);
	RenderCommand::clear(BUFFER_BITS_COLOR);

	last_texture_id = screen_buffer->get_color_attachment_renderer_id(0);
	effect_provided = false;

	// TODO maybe calculate everything seperatly then add the values
	if (volume.gray_scale.enabled) {
		_process_gray_scale();

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	if (volume.chromatic_aberration.enabled) {
		_process_chromatic_aberration();

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	if (volume.blur.enabled) {
		_process_blur();

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	if (volume.sharpen.enabled) {
		_process_sharpen();

		last_texture_id = get_frame_buffer_renderer_id();
		effect_provided = true;
	}

	if (volume.vignette.enabled) {
		_process_vignette();

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

void PostProcessor::_process_gray_scale() {
	gray_scale_shader->bind();

	_draw_screen_quad();
}

void PostProcessor::_process_chromatic_aberration() {
	chromatic_aberration_shader->bind();

	chromatic_aberration_shader->set_uniform("u_offset", volume.chromatic_aberration.offset);

	_draw_screen_quad();
}

void PostProcessor::_process_blur() {
	blur_shader->bind();

	blur_shader->set_uniform("u_size", (int)volume.blur.size);
	blur_shader->set_uniform("u_seperation", volume.blur.seperation);

	_draw_screen_quad();
}

void PostProcessor::_process_sharpen() {
	sharpen_shader->bind();

	sharpen_shader->set_uniform("u_amount", volume.sharpen.amount);

	_draw_screen_quad();
}

void PostProcessor::_process_vignette() {
	vignette_shader->bind();

	vignette_shader->set_uniform("u_inner", volume.vignette.inner);
	vignette_shader->set_uniform("u_outer", volume.vignette.outer);
	vignette_shader->set_uniform("u_strength", volume.vignette.strength);
	vignette_shader->set_uniform("u_curvature", volume.vignette.curvature);

	_draw_screen_quad();
}

void PostProcessor::_draw_screen_quad() {
	RenderCommand::bind_texture(last_texture_id);
	RenderCommand::draw_arrays(vertex_array, 6);
}
