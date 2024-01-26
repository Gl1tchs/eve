#include "renderer/index_buffer.h"

#include <glad/glad.h>

IndexBuffer::IndexBuffer(uint32_t size) :
		ibo(0), count(0) {
	glCreateBuffers(1, &ibo);
	glBindBuffer(GL_ARRAY_BUFFER, ibo);
	// TODO documentate this constructor and tell the client this defines as DYNAMIC_DRAW
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count) :
		ibo(0), count(count) {
	glCreateBuffers(1, &ibo);

	// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
	// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
	glBindBuffer(GL_ARRAY_BUFFER, ibo);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices,
			GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &ibo);
}

void IndexBuffer::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

void IndexBuffer::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::set_data(const void* data, uint32_t size) const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
}

uint32_t IndexBuffer::get_count() const {
	return count;
}