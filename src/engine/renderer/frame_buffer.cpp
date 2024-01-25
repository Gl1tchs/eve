#include "renderer/frame_buffer.h"

#include <glad/glad.h>

FrameBuffer::FrameBuffer(const glm::ivec2 &size) :
		_size(size) {
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	glGenRenderbuffers(1, &_rbo);

	refresh();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer is not complete!");
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &_fbo);
	glDeleteRenderbuffers(1, &_rbo);
}

void FrameBuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void FrameBuffer::unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::refresh() {
	TextureMetadata metadata;
	metadata.size = _size;
	metadata.format = TextureFormat::RGB;
	metadata.min_filter = TextureFilteringMode::LINEAR;
	metadata.mag_filter = TextureFilteringMode::LINEAR;
	metadata.wrap_s = TextureWrappingMode::CLAMP_TO_EDGE;
	metadata.wrap_t = TextureWrappingMode::CLAMP_TO_EDGE;
	metadata.generate_mipmaps = false;

	_texture = create_ref<Texture2D>(metadata);

	glBindTexture(GL_TEXTURE_2D, _texture->renderer_id());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			_texture->renderer_id(), 0);

	glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _size.x,
			_size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER, _rbo);
}

const glm::ivec2 &FrameBuffer::size() const {
	return _size;
}

void FrameBuffer::set_size(glm::ivec2 size) {
	_size = size;
}

Ref<Texture2D> FrameBuffer::texture() {
	return _texture;
}
