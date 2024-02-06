#include "renderer/primitives/text.h"

#include "renderer/font.h"

glm::vec2 get_text_size(const std::string& text, Ref<Font> font, float kerning, float line_spacing) {
	if (!font) {
		font = Font::get_default();
	}

	const auto& font_geometry = font->get_msdf_data().font_geometry;
	const auto& metrics = font_geometry.getMetrics();

	const double fs_scale = 1.0 / (metrics.ascenderY - metrics.descenderY);

	const float space_glyph_advance = font_geometry.getGlyph(' ')->getAdvance();

	// center the text around the transform if its not in screen space
	glm::vec2 text_size(0.0f);

	float max_width = 0.0f;
	for (size_t i = 0; i < text.size(); i++) {
		const char character = text[i];
		if (character == '\r') {
			continue;
		}

		if (character == '\n') {
			max_width = std::max(text_size.x, max_width);

			text_size.x = 0.0f;
			text_size.y += fs_scale * metrics.lineHeight + line_spacing;
			continue;
		}

		if (character == ' ') {
			text_size.x += fs_scale * space_glyph_advance + kerning;
			continue;
		}

		if (character == '\t') {
			text_size.x += 4.0f * (fs_scale * space_glyph_advance + kerning);
			continue;
		}

		auto glyph = font_geometry.getGlyph(character);
		if (!glyph) {
			glyph = font_geometry.getGlyph('?');
		}
		if (!glyph) {
			continue;
		}

		if (i < text.size()) {
			double advance = glyph->getAdvance();
			const char next_character = text[i + 1];
			font_geometry.getAdvance(advance, character, next_character);

			text_size.x += fs_scale * advance + kerning;
		}
	}

	text_size.x = std::max(max_width, text_size.x);

	return text_size;
}
