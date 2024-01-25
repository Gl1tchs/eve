#ifndef BUFFER_H
#define BUFFER_H

#include "core/assert.h"

struct Buffer {
	uint8_t *data = nullptr;
	uint64_t size = 0;

	Buffer() = default;
	Buffer(uint64_t size);
	Buffer(const void *data, uint64_t size);
	Buffer(const Buffer &) = default;

	static Buffer copy(Buffer other);

	void allocate(uint64_t size);

	void release();

	template <typename T>
	T *as() {
		return (T *)data;
	}

	operator bool() const { return (bool)data; }
};

struct ScopedBuffer {
	ScopedBuffer(Buffer buffer);
	ScopedBuffer(uint64_t size);
	~ScopedBuffer();

	[[nodiscard]] uint8_t *data();
	[[nodiscard]] const uint64_t &size() const;

	template <typename T>
	T *as() {
		return buffer.as<T>();
	}

	operator bool() const { return buffer; }

private:
	Buffer buffer;
};

template <typename T>
struct BufferArray {
	BufferArray() = default;
	BufferArray(const uint64_t max_elements) :
			_buffer(max_elements * sizeof(T)), _max_elements(max_elements) {}

	~BufferArray() {
		if (_buffer) {
			_buffer.release();
		}
	}

	[[nodiscard]] uint8_t *data() { return _buffer.data; }
	[[nodiscard]] const uint64_t &size() const { return _buffer.size; }
	[[nodiscard]] const uint32_t &count() const { return _count; }

	void allocate(const uint64_t max_elements) {
		_max_elements = max_elements;
		_buffer.allocate(_max_elements * sizeof(T));
	}

	void release() {
		_count = 0;
		_max_elements = 0;
		_buffer.release();
	}

	void add(const T &value) {
		// prevent memory leaks
		EVE_ASSERT(_buffer && _count < _max_elements);
		_buffer.as<T>()[_count++] = value;
	}

	[[nodiscard]] T &at(const uint32_t idx) {
		EVE_ASSERT(_buffer && idx < _max_elements);
		return _buffer.as<T>()[idx];
	}

	void clear() {
		EVE_ASSERT(_buffer);
		int64_t max_elements_copy = _max_elements;
		release();
		allocate(max_elements_copy);
	}

	void reset_index() { _count = 0; }

	T &operator[](const uint32_t idx) { return at(idx); }

	operator bool() const { return _buffer; }

private:
	Buffer _buffer;
	uint32_t _count = 0;
	uint32_t _max_elements = 0;
};

#endif