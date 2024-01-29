#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

class UniformBuffer final {
public:
	UniformBuffer(uint64_t size, uint32_t binding);
	~UniformBuffer();

	void set_data(const void* data, uint64_t size, uint32_t offset = 0);

private:
	uint32_t ubo;
};

#endif
