#include "renderer/vertex_array.h"

#include <glad/glad.h>

static GLenum shader_data_type_to_opengl(ShaderDataType type) {
	switch (type) {
		case ShaderDataType::FLOAT:
			return GL_FLOAT;
		case ShaderDataType::FLOAT2:
			return GL_FLOAT;
		case ShaderDataType::FLOAT3:
			return GL_FLOAT;
		case ShaderDataType::FLOAT4:
			return GL_FLOAT;
		case ShaderDataType::MAT3:
			return GL_FLOAT;
		case ShaderDataType::MAT4:
			return GL_FLOAT;
		case ShaderDataType::INT:
			return GL_INT;
		case ShaderDataType::INT2:
			return GL_INT;
		case ShaderDataType::INT3:
			return GL_INT;
		case ShaderDataType::INT4:
			return GL_INT;
		case ShaderDataType::BOOL:
			return GL_BOOL;
		default:
			EVE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
	}
}

VertexArray::VertexArray() {
	glCreateVertexArrays(1, &_vao);
}

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &_vao);
}

void VertexArray::bind() const {
	glBindVertexArray(_vao);
}

void VertexArray::unbind() const {
	glBindVertexArray(0);
}

const std::vector<Ref<VertexBuffer>> &
VertexArray::vertex_buffers() const {
	return _vertex_buffers;
}

void VertexArray::add_vertex_buffer(
		const Ref<VertexBuffer> &vertex_buffer) {
	EVE_ASSERT(vertex_buffer->layout().elements().size(),
			"Vertex Buffer has no layout!");

	glBindVertexArray(_vao);

	vertex_buffer->bind();

	const auto layout = vertex_buffer->layout();
	for (const auto &element : layout) {
		switch (element.type) {
			case ShaderDataType::FLOAT:
			case ShaderDataType::FLOAT2:
			case ShaderDataType::FLOAT3:
			case ShaderDataType::FLOAT4: {
				glEnableVertexAttribArray(_vertex_buffer_index);
				glVertexAttribPointer(
						_vertex_buffer_index, element.component_count(),
						shader_data_type_to_opengl(element.type),
						element.normalized ? GL_TRUE : GL_FALSE, layout.stride(),
						(const void *)element.offset);
				if (element.divisor) {
					glVertexAttribDivisor(_vertex_buffer_index,
							element.divisor);
				}
				_vertex_buffer_index++;
				break;
			}
			case ShaderDataType::INT:
			case ShaderDataType::INT2:
			case ShaderDataType::INT3:
			case ShaderDataType::INT4:
			case ShaderDataType::BOOL: {
				glEnableVertexAttribArray(_vertex_buffer_index);
				glVertexAttribIPointer(
						_vertex_buffer_index, element.component_count(),
						shader_data_type_to_opengl(element.type), layout.stride(),
						(const void *)element.offset);
				if (element.divisor) {
					glVertexAttribDivisor(_vertex_buffer_index,
							element.divisor);
				}
				_vertex_buffer_index++;
				break;
			}
			case ShaderDataType::MAT3:
			case ShaderDataType::MAT4: {
				int count = element.component_count();
				for (int i = 0; i < count; i++) {
					glEnableVertexAttribArray(_vertex_buffer_index);
					glVertexAttribPointer(
							_vertex_buffer_index, count,
							shader_data_type_to_opengl(element.type),
							element.normalized ? GL_TRUE : GL_FALSE,
							layout.stride(),
							(const void *)(element.offset +
									sizeof(float) * count * i));
					if (element.divisor) {
						glVertexAttribDivisor(_vertex_buffer_index,
								element.divisor);
					}
					_vertex_buffer_index++;
				}
				break;
			}
			default:
				EVE_ASSERT(false, "Unknown ShaderDataType!");
		}
	}

	_vertex_buffers.push_back(vertex_buffer);
}

const Ref<IndexBuffer> &VertexArray::index_buffer() const {
	return _index_buffer;
}

void VertexArray::set_index_buffer(
		const Ref<IndexBuffer> &index_buffer) {
	glBindVertexArray(_vao);
	index_buffer->bind();

	_index_buffer = index_buffer;
}
