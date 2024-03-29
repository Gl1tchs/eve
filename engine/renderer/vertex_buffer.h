#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

enum class ShaderDataType {
	NONE = 0,
	FLOAT,
	FLOAT2,
	FLOAT3,
	FLOAT4,
	MAT3,
	MAT4,
	INT,
	INT2,
	INT3,
	INT4,
	BOOL,
};

uint32_t get_shader_data_type_size(ShaderDataType type);

struct BufferElement final {
	ShaderDataType type;
	std::string name;
	uint32_t size;
	uint64_t offset;
	bool normalized;
	uint32_t divisor;

	BufferElement(ShaderDataType type, const std::string& name,
			bool normalized = false, uint32_t divisor = 0);

	uint32_t get_component_count() const;
};

class BufferLayout final {
public:
	BufferLayout() = default;

	BufferLayout(std::initializer_list<BufferElement> elements);

	uint32_t get_stride() const { return _stride; }

	const std::vector<BufferElement>& get_elements() const { return _elements; }

	std::vector<BufferElement>::iterator begin() { return _elements.begin(); }
	std::vector<BufferElement>::iterator end() { return _elements.end(); }
	std::vector<BufferElement>::const_iterator begin() const {
		return _elements.begin();
	}
	std::vector<BufferElement>::const_iterator end() const {
		return _elements.end();
	}

private:
	void _calculate_offset_and_stride();

	std::vector<BufferElement> _elements;
	uint32_t _stride = 0;
};

class VertexBuffer final {
public:
	VertexBuffer(uint64_t size);
	VertexBuffer(const void* vertices, uint64_t size);
	~VertexBuffer();

	void bind();
	void unbind();

	void set_data(const void* data, uint64_t size);

	const BufferLayout& get_layout();
	void set_layout(const BufferLayout& _layout);

private:
	uint32_t vbo;
	BufferLayout layout;
};

#endif
