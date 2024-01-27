#ifndef LINE_H
#define LINE_H

#include "core/color.h"

struct LineVertex {
	glm::vec3 position;
	Color color;
};

constexpr size_t LINE_MAX_BATCHES = 10000;

constexpr size_t LINE_VERTEX_COUNT = 2;
constexpr size_t LINE_MAX_VERTEX_COUNT = LINE_MAX_BATCHES * LINE_VERTEX_COUNT;

#endif
