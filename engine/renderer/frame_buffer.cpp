#include "renderer/frame_buffer.h"
#include "core/assert.h"

#include <glad/glad.h>

static constexpr uint32_t max_framebuffer_size = 8192;

inline static int get_texture_target(bool multisampled) {
	return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

inline static void create_textures(bool multisampled, uint32_t* out_id,
		uint32_t count) {
	glCreateTextures(get_texture_target(multisampled), count, out_id);
}

inline static void bind_texture(bool multisampled, uint32_t id) {
	glBindTexture(get_texture_target(multisampled), id);
}

inline static void attach_color_texture(uint32_t id, int samples, int internal_format,
		uint32_t format, uint32_t width, uint32_t height,
		uint32_t index) {
	bool multisampled = samples > 1;
	if (multisampled) {
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format,
				width, height, GL_FALSE);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
				GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
			get_texture_target(multisampled), id, 0);
}

inline static void attach_depth_texture(uint32_t id, int samples, uint32_t format,
		uint32_t attachment_type, uint32_t width,
		uint32_t height) {
	bool multisampled = samples > 1;
	if (multisampled) {
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width,
				height, GL_FALSE);
	} else {
		glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type,
			get_texture_target(multisampled), id, 0);
}

inline static bool is_depth_format(FrameBufferTextureFormat format) {
	switch (format) {
		case FrameBufferTextureFormat::DEPTH24_STENCIL8:
		case FrameBufferTextureFormat::DEPTH32F_STENCIL8:
			return true;
		default:
			return false;
	}
}

FrameBufferTextureSpecification::FrameBufferTextureSpecification(
		FrameBufferTextureFormat format) :
		texture_format(format) {}

FrameBufferAttachmentSpecification::FrameBufferAttachmentSpecification(
		std::initializer_list<FrameBufferTextureSpecification> attachments) :
		attachments(attachments) {}

FrameBuffer::FrameBuffer(const FrameBufferCreateInfo& info) :
		fbo(0), width(info.width), height(info.height), samples(info.samples) {
	for (auto attachment : info.attachments.attachments) {
		if (!is_depth_format(attachment.texture_format)) {
			color_attachment_specs.emplace_back(attachment);
		} else {
			depth_attachment_spec = attachment;
		}
	}

	_invalidate();
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(color_attachments.size(), color_attachments.data());
	glDeleteTextures(1, &depth_attachment);
}

void FrameBuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);
}

void FrameBuffer::unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(int w, int h) {
	if (w == 0 || h == 0 || w > max_framebuffer_size ||
			h > max_framebuffer_size) {
		EVE_LOG_ENGINE_WARNING("Attempted to rezize framebuffer to {0}, {1}", w, w);
		return;
	}

	width = w;
	height = h;

	_invalidate();
}

void FrameBuffer::read_pixel(uint32_t index, uint32_t x, uint32_t y, FrameBufferTextureFormat format, void* pixel) {
	EVE_ASSERT_ENGINE(index < color_attachments.size());

	glReadBuffer(GL_COLOR_ATTACHMENT0 + index);

	switch (format) {
		case FrameBufferTextureFormat::RED_INT: {
			glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, pixel);
			break;
		}
		case FrameBufferTextureFormat::RGBA8: {
			glReadPixels(x, y, 1, 1, GL_RGBA8, GL_FLOAT, pixel);
			break;
		}
		default:
			break;
	}
}

void FrameBuffer::clear_attachment(uint32_t index, void* value) {
	EVE_ASSERT_ENGINE(index < color_attachments.size());
	auto& attachment_spec = color_attachment_specs[index];

	switch (attachment_spec.texture_format) {
		case FrameBufferTextureFormat::RED_INT: {
			glClearTexImage(color_attachments[index], 0, GL_RED_INTEGER, GL_INT, value);
			break;
		}
		case FrameBufferTextureFormat::RGBA8: {
			glClearTexImage(color_attachments[index], 0, GL_RGBA8, GL_FLOAT, value);
			break;
		}
		default:
			break;
	}
}

uint32_t FrameBuffer::get_color_attachment_renderer_id(uint32_t index) const {
	EVE_ASSERT_ENGINE(index < color_attachments.size());
	return color_attachments[index];
}

glm::ivec2 FrameBuffer::get_size() const {
	return { width, height };
}

void FrameBuffer::_invalidate() {
	if (fbo) {
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(color_attachments.size(), color_attachments.data());
		glDeleteTextures(1, &depth_attachment);

		color_attachments.clear();
		depth_attachment = 0;
	}

	glCreateFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	bool multisample = samples > 1;

	if (color_attachment_specs.size()) {
		color_attachments.resize(color_attachment_specs.size());

		create_textures(multisample, color_attachments.data(),
				color_attachments.size());

		for (size_t i = 0; i < color_attachments.size(); i++) {
			bind_texture(multisample, color_attachments[i]);
			switch (color_attachment_specs[i].texture_format) {
				case FrameBufferTextureFormat::RED_INT:
					attach_color_texture(color_attachments[i], samples, GL_R32I,
							GL_RED_INTEGER, width, height, i);
					break;
				case FrameBufferTextureFormat::RGBA8:
					attach_color_texture(color_attachments[i], samples, GL_RGBA8,
							GL_RGBA, width, height, i);
					break;
				default:
					break;
			}
		}
	}

	if (depth_attachment_spec.texture_format !=
			FrameBufferTextureFormat::NONE) {
		create_textures(multisample, &depth_attachment, 1);
		bind_texture(multisample, depth_attachment);
		switch (depth_attachment_spec.texture_format) {
			case FrameBufferTextureFormat::DEPTH24_STENCIL8:
				attach_depth_texture(depth_attachment, samples,
						GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT,
						width, height);
				break;
			case FrameBufferTextureFormat::DEPTH32F_STENCIL8:
				attach_depth_texture(depth_attachment, samples,
						GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT,
						width, height);
				break;
			default:
				break;
		}
	}

	if (color_attachments.size() > 1) {
		EVE_ASSERT_ENGINE(color_attachments.size() <= 4);
		const uint32_t buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(color_attachments.size(), buffers);
	} else if (color_attachments.empty()) {
		// Only depth-pass
		glDrawBuffer(GL_NONE);
	}

	EVE_ASSERT_ENGINE(
			glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
			"Framebuffer is incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
