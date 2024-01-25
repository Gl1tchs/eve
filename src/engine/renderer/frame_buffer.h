#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "renderer/texture.h"

class FrameBuffer {
public:
	FrameBuffer(const glm::ivec2 &size);
	~FrameBuffer();

	void bind() const;

	void unbind() const;

	void refresh();

	[[nodiscard]] const glm::ivec2 &size() const;
	void set_size(glm::ivec2 size);

	Ref<Texture2D> texture();

private:
	glm::ivec2 _size;

	uint32_t _fbo;
	uint32_t _rbo;

	Ref<Texture2D> _texture;
};

#endif