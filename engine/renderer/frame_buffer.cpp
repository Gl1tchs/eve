#include "renderer/frame_buffer.h"

#include <glad/glad.h>

FrameBuffer::FrameBuffer(const glm::ivec2& size) :
		size(size), fbo(0), rbo(0) {
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenRenderbuffers(1, &rbo);

	refresh();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer is not complete!");
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);
}

void FrameBuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void FrameBuffer::unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::refresh() {
	TextureMetadata metadata;
	metadata.size = size;
	metadata.format = TextureFormat::RGB;
	metadata.min_filter = TextureFilteringMode::LINEAR;
	metadata.mag_filter = TextureFilteringMode::LINEAR;
	metadata.wrap_s = TextureWrappingMode::CLAMP_TO_EDGE;
	metadata.wrap_t = TextureWrappingMode::CLAMP_TO_EDGE;
	metadata.generate_mipmaps = false;

	texture = create_ref<Texture2D>(metadata);

	glBindTexture(GL_TEXTURE_2D, texture->get_renderer_id());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			texture->get_renderer_id(), 0);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x,
			size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER, rbo);
}

const glm::ivec2& FrameBuffer::get_size() const {
	return size;
}

void FrameBuffer::set_size(glm::ivec2 size) {
	size = size;
}

Ref<Texture2D> FrameBuffer::get_texture() {
	return texture;
}
