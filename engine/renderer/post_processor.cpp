#include "renderer/post_processor.h"

#include "post_processor.h"
#include "renderer/frame_buffer.h"
#include "renderer/render_command.h"
#include "renderer/shader_library.h"
#include "renderer/uniform_buffer.h"
#include "renderer/vertex_array.h"

struct PostProcessUniformBlock {
	float p1 = 0.0f;
	float p2 = 0.0f;
	float p3 = 0.0f;
	float p4 = 0.0f;
};

PostProcessor::PostProcessor() {
	EVE_PROFILE_FUNCTION();

	vertex_array = create_ref<VertexArray>();

	uniform_buffer =
			create_ref<UniformBuffer>(sizeof(PostProcessUniformBlock), 1);

	// TODO maybe use single shader?
	gray_scale_shader = ShaderLibrary::get_shader("screen.vert",
			"post-processing/gray-scale.frag");

	chromatic_aberration_shader =
			ShaderLibrary::get_shader("screen.vert",
					"post-processing/chromatic-aberration.frag");

	blur_shader = ShaderLibrary::get_shader(
			"screen.vert", "post-processing/blur.frag");

	sharpen_shader = ShaderLibrary::get_shader("screen.vert",
			"post-processing/sharpen.frag");

	vignette_shader = ShaderLibrary::get_shader("screen.vert",
			"post-processing/vignette.frag");

	FrameBufferCreateInfo fb_info;
	fb_info.width = 1280;
	fb_info.height = 768;
	fb_info.attachments = {
		FrameBufferTextureFormat::RGBA8,
	};

	frame_buffer = create_ref<FrameBuffer>(fb_info);
}

bool PostProcessor::process(const Ref<FrameBuffer>& screen_buffer,
		const PostProcessVolume& _volume) {
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

Ref<FrameBuffer> PostProcessor::get_frame_buffer() { return frame_buffer; }

uint32_t PostProcessor::get_frame_buffer_renderer_id() const {
	return frame_buffer->get_color_attachment_renderer_id(0);
}

void PostProcessor::_process_gray_scale() {
	gray_scale_shader->bind();

	_draw_screen_quad();
}

void PostProcessor::_process_chromatic_aberration() {
	PostProcessUniformBlock block = {
		.p1 = volume.chromatic_aberration.offset.x,
		.p2 = volume.chromatic_aberration.offset.y,
		.p3 = volume.chromatic_aberration.offset.z,
	};

	uniform_buffer->set_data(&block, sizeof(PostProcessUniformBlock));

	chromatic_aberration_shader->bind();
	_draw_screen_quad();
}

void PostProcessor::_process_blur() {
	PostProcessUniformBlock block = {
		.p1 = (float)volume.blur.size,
		.p2 = volume.blur.seperation,
	};

	uniform_buffer->set_data(&block, sizeof(PostProcessUniformBlock));

	blur_shader->bind();
	_draw_screen_quad();
}

void PostProcessor::_process_sharpen() {
	PostProcessUniformBlock block = {
		.p1 = volume.sharpen.amount,
	};

	uniform_buffer->set_data(&block, sizeof(PostProcessUniformBlock));

	sharpen_shader->bind();
	_draw_screen_quad();
}

void PostProcessor::_process_vignette() {
	PostProcessUniformBlock block = {
		.p1 = volume.vignette.inner,
		.p2 = volume.vignette.outer,
		.p3 = volume.vignette.strength,
		.p4 = volume.vignette.curvature,
	};

	uniform_buffer->set_data(&block, sizeof(PostProcessUniformBlock));

	vignette_shader->bind();
	_draw_screen_quad();
}

void PostProcessor::_draw_screen_quad() {
	RenderCommand::bind_texture(last_texture_id);
	RenderCommand::draw_arrays(vertex_array, 6);
}
