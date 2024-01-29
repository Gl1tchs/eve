#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

class IndexBuffer final {
public:
	IndexBuffer(uint64_t size);
	IndexBuffer(const uint32_t* indices, uint64_t count);
	~IndexBuffer();

	void bind() const;
	void unbind() const;

	void set_data(const void* data, uint64_t size) const;

	uint64_t get_count() const;

private:
	uint32_t ibo;
	uint64_t count;
};

#endif
