#include "renderer/vertex_buffer.h"

#include <glad/glad.h>

uint32_t get_shader_data_type_size(ShaderDataType type) {
	switch (type) {
		case ShaderDataType::FLOAT:
			return 4;
		case ShaderDataType::FLOAT2:
			return 4 * 2;
		case ShaderDataType::FLOAT3:
			return 4 * 3;
		case ShaderDataType::FLOAT4:
			return 4 * 4;
		case ShaderDataType::MAT3:
			return 4 * 3 * 3;
		case ShaderDataType::MAT4:
			return 4 * 4 * 4;
		case ShaderDataType::INT:
			return 4;
		case ShaderDataType::INT2:
			return 4 * 2;
		case ShaderDataType::INT3:
			return 4 * 3;
		case ShaderDataType::INT4:
			return 4 * 4;
		case ShaderDataType::BOOL:
			return 1;
		default:
			EVE_ASSERT_ENGINE(false, "Unknown ShaderDataType!")
			return 0;
	}
}

BufferElement::BufferElement(ShaderDataType type, const std::string& name,
		bool normalized, uint32_t divisor) :
		type(type),
		name(name),
		size(get_shader_data_type_size(type)),
		offset(0),
		normalized(normalized),
		divisor(divisor) {}

uint32_t BufferElement::get_component_count() const {
	switch (type) {
		case ShaderDataType::FLOAT:
			return 1;
		case ShaderDataType::FLOAT2:
			return 2;
		case ShaderDataType::FLOAT3:
			return 3;
		case ShaderDataType::FLOAT4:
			return 4;
		case ShaderDataType::MAT3:
			return 3; // 3 * float3
		case ShaderDataType::MAT4:
			return 4; // 4 * float4
		case ShaderDataType::INT:
			return 1;
		case ShaderDataType::INT2:
			return 2;
		case ShaderDataType::INT3:
			return 3;
		case ShaderDataType::INT4:
			return 4;
		case ShaderDataType::BOOL:
			return 1;
		default:
			EVE_ASSERT_ENGINE(false, "Unknown ShaderDataType!")
			return 0;
	}
}

BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements) :
		_elements(elements) {
	_calculate_offset_and_stride();
}

void BufferLayout::_calculate_offset_and_stride() {
	size_t offset = 0;
	_stride = 0;
	for (auto& element : _elements) {
		element.offset = offset;
		offset += element.size;
		_stride += element.size;
	}
}

VertexBuffer::VertexBuffer(uint32_t size) :
		vbo(0) {
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// TODO documentate this constructor and tell the client this defines as DYNAMIC_DRAW
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(const void* vertices, uint32_t size) :
		vbo(0) {
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &vbo);
}

void VertexBuffer::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void VertexBuffer::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::set_data(const void* data, uint32_t size) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

const BufferLayout& VertexBuffer::get_layout() {
	return layout;
}

void VertexBuffer::set_layout(const BufferLayout& r_layout) {
	layout = r_layout;
}
