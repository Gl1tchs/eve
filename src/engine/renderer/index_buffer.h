#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

class IndexBuffer final {
public:
	IndexBuffer(uint32_t size);
	IndexBuffer(const uint32_t* indices, uint32_t count);
	~IndexBuffer();

	void bind() const;
	void unbind() const;

	void set_data(const void* data, uint32_t size) const;

	uint32_t get_count() const;

private:
	uint32_t ibo;
	uint32_t count;
};

#endif