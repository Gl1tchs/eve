#include "renderer/uniform_buffer.h"

#include <glad/glad.h>

UniformBuffer::UniformBuffer(uint32_t size, uint32_t binding) {
	glCreateBuffers(1, &_ubo);
	glNamedBufferData(_ubo, size, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, _ubo);
}

UniformBuffer::~UniformBuffer() {
	glDeleteBuffers(1, &_ubo);
}

void UniformBuffer::set_data(const void *data, uint32_t size, uint32_t offset) {
	glNamedBufferSubData(_ubo, offset, size, data);
}
