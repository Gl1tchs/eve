#ifndef TEXT_H
#define TEXT_H

#include "core/color.h"

struct TextVertex {
	glm::vec3 position;
	glm::vec2 tex_coord;
	Color fg_color = glm::vec4(1.0f);
	Color bg_color = glm::vec4(0.0f);
	uint32_t entity_id;
};

#endif
