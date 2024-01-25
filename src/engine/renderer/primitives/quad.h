#ifndef QUAD_H
#define QUAD_H

#include "core/color.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/vertex_array.h"
#include "renderer/vertex_buffer.h"

struct QuadVertex {
	glm::vec2 position;
	glm::vec2 tex_coords;
	float z_index = 0.0f;
	Color color = glm::vec4(1.0f);
	float tex_index = 0.0f;
	glm::vec2 tex_tiling = { 1, 1 };
};

constexpr uint64_t QUAD_VERTEX_COUNT = 4;
constexpr uint64_t QUAD_INDEX_COUNT = 6;

constexpr uint64_t QUAD_MAX_BATCHES = 5000;

constexpr uint64_t QUAD_MAX_VERTEX_COUNT = QUAD_VERTEX_COUNT * QUAD_MAX_BATCHES;
constexpr uint64_t QUAD_MAX_INDEX_COUNT = QUAD_INDEX_COUNT * QUAD_MAX_BATCHES;
constexpr uint64_t QUAD_MAX_TEXTURE_COUNT = 32;

constexpr glm::vec4 QUAD_VERTEX_POSITIONS[QUAD_VERTEX_COUNT] = {
	{ -0.5f, -0.5f, 0.0f, 1.0f },
	{ -0.5f, 0.5f, 0.0f, 1.0f },
	{ 0.5f, 0.5f, 0.0f, 1.0f },
	{ 0.5f, -0.5f, 0.0f, 1.0f }
};

constexpr glm::vec2 QUAD_TEX_COORDS[QUAD_VERTEX_COUNT] = { { 0.0f, 0.0f },
	{ 0.0f, 1.0f },
	{ 1.0f, 1.0f },
	{ 1.0f, 0.0f } };

constexpr uint32_t QUAD_INDICES[QUAD_INDEX_COUNT] = { 0, 1, 2, 2, 3, 0 };

class QuadPrimitive {
public:
	QuadPrimitive();
	~QuadPrimitive();

	void render();

	void reset();

	bool needs_batch() const;

	void add_instance(const glm::mat4& transform, int z_index = 0,
			Ref<Texture2D> texture = nullptr,
			const Color& color = { 1, 1, 1, 1 },
			const glm::vec2& tex_tiling = { 1, 1 });

	[[nodiscard]] float find_texture_index(const Ref<Texture2D>& texture);

private:
	Ref<VertexArray> _vertex_array;
	Ref<VertexBuffer> _vertex_buffer;
	Ref<Shader> _shader;

	// render data
	BufferArray<QuadVertex> _vertices;
	uint32_t _index_count = 0;

	// textures
	Ref<Texture2D> _white_texture;
	std::array<Ref<Texture2D>, QUAD_MAX_TEXTURE_COUNT> _texture_slots;
	uint32_t _texture_slot_index = 0;
};

#endif