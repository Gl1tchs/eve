#include "renderer/uniform_buffer.h"

#include <glad/glad.h>

UniformBuffer::UniformBuffer(uint32_t size, uint32_t binding) :
		ubo(0) {
	glCreateBuffers(1, &ubo);
	glNamedBufferData(ubo, size, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);
}

UniformBuffer::~UniformBuffer() {
	glDeleteBuffers(1, &ubo);
}

void UniformBuffer::set_data(const void* data, uint32_t size, uint32_t offset) {
	glNamedBufferSubData(ubo, offset, size, data);
}
