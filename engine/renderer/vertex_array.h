#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "renderer/index_buffer.h"
#include "renderer/vertex_buffer.h"

class VertexArray final {
public:
	VertexArray();
	~VertexArray();

	void bind() const;
	void unbind() const;

	const std::vector<Ref<VertexBuffer>>&
	get_vertex_buffers() const;
	void add_vertex_buffer(const Ref<VertexBuffer>& vertex_buffer);

	const Ref<IndexBuffer>& get_index_buffer() const;
	void set_index_buffer(const Ref<IndexBuffer>& r_index_buffer);

private:
	uint32_t vao;
	uint32_t vertex_buffer_index = 0;
	std::vector<Ref<VertexBuffer>> vertex_buffers;
	Ref<IndexBuffer> index_buffer;
};

#endif