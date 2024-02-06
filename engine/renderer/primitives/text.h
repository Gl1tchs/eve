#ifndef TEXT_H
#define TEXT_H

#include "core/color.h"

class Font;

struct TextVertex {
	glm::vec3 position;
	glm::vec2 tex_coord;
	Color fg_color = glm::vec4(1.0f);
	Color bg_color = glm::vec4(0.0f);
	int is_screen_space = false;
	uint32_t entity_id;
};

glm::vec2 get_text_size(const std::string& text, Ref<Font> font, float kerning = 0.0f, float line_spacing = 0.0f);

#endif
