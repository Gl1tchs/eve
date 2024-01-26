#ifndef QUAD_H
#define QUAD_H

#include "core/color.h"

struct QuadVertex {
	glm::vec3 position;
	glm::vec2 tex_coord;
	Color color = glm::vec4(1.0f);
	float tex_index = 0.0f;
	glm::vec2 tex_tiling = { 1, 1 };
};

constexpr uint64_t QUAD_VERTEX_COUNT = 4;
constexpr uint64_t QUAD_INDEX_COUNT = 6;

constexpr uint64_t QUAD_MAX_BATCHES = 20000;

constexpr uint64_t QUAD_MAX_VERTEX_COUNT = QUAD_VERTEX_COUNT * QUAD_MAX_BATCHES;
constexpr uint64_t QUAD_MAX_INDEX_COUNT = QUAD_INDEX_COUNT * QUAD_MAX_BATCHES;

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

inline bool quad_needs_batch(uint32_t index_count) {
	return index_count + QUAD_INDEX_COUNT >= QUAD_MAX_INDEX_COUNT;
}

#endif
