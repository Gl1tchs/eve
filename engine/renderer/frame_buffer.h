#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "renderer/texture.h"

enum class FrameBufferTextureFormat {
	NONE = 0,
	// Color
	RED_INT,
	RGBA8,
	// Depth
	DEPTH24_STENCIL8,
	DEPTH32F_STENCIL8,
	// Defaults
	DEPTH = DEPTH24_STENCIL8
};

struct FrameBufferTextureSpecification {
	FrameBufferTextureSpecification() = default;
	FrameBufferTextureSpecification(FrameBufferTextureFormat format);

	FrameBufferTextureFormat texture_format = FrameBufferTextureFormat::NONE;
	// TODO filtering/wrap
};

struct FrameBufferAttachmentSpecification {
	FrameBufferAttachmentSpecification() = default;
	FrameBufferAttachmentSpecification(
			std::initializer_list<FrameBufferTextureSpecification> attachments);

	std::vector<FrameBufferTextureSpecification> attachments;
};

struct FrameBufferCreateInfo {
	uint32_t width = 0, height = 0;
	FrameBufferAttachmentSpecification attachments;
	uint32_t samples = 1;
};

class FrameBuffer {
public:
	FrameBuffer(const FrameBufferCreateInfo& info);
	~FrameBuffer();

	void bind() const;

	void unbind() const;

	void resize(int w, int h);

	void read_pixel(uint32_t index, uint32_t x, uint32_t y, FrameBufferTextureFormat format, void* pixel);

	// DISCLAIMER
	//	This only works on OpenGL4 but not with GLES
	void clear_attachment(uint32_t index, void* value);

	uint32_t get_color_attachment_renderer_id(uint32_t index) const;

	glm::ivec2 get_size() const;

private:
	void _invalidate();

private:
	uint32_t fbo;

	uint32_t width;
	uint32_t height;
	uint32_t samples;

	std::vector<FrameBufferTextureSpecification> color_attachment_specs;
	FrameBufferTextureSpecification depth_attachment_spec;

	std::vector<uint32_t> color_attachments;
	uint32_t depth_attachment = 0;
};

#endif
