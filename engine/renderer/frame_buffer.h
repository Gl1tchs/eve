#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "renderer/texture.h"

class FrameBuffer {
public:
	FrameBuffer(const glm::ivec2& size);
	~FrameBuffer();

	void bind() const;

	void unbind() const;

	void refresh();

	const glm::ivec2& get_size() const;
	void set_size(glm::ivec2 size);

	Ref<Texture2D> get_texture();

private:
	glm::ivec2 size;

	uint32_t fbo;
	uint32_t rbo;

	Ref<Texture2D> texture;
};

#endif