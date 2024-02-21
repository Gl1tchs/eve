#ifndef RENDERER_H
#define RENDERER_H

#include "core/color.h"
#include "renderer/camera.h"
#include "renderer/font.h"
#include "renderer/texture.h"
#include "scene/transform.h"

constexpr uint64_t MAX_TEXTURE_COUNT = 32;

struct RendererStats {
	uint32_t quad_count = 0;
	uint32_t vertex_count = 0;
	uint32_t index_count = 0;
	uint32_t draw_calls = 0;
};

namespace renderer {

void init();

void shutdown();

void begin_pass(const CameraData& camera_data);

void end_pass();

void draw_quad(const Transform& transform, const Color& color, uint32_t entity_id = -1);

void draw_quad(const Transform& transform, Ref<Texture2D> texture,
		const glm::vec2& tex_tiling, uint32_t entity_id = -1);

void draw_quad(const Transform& transform,
		Ref<Texture2D> texture, const Color& color,
		const glm::vec2& tex_tiling, uint32_t entity_id = -1);

void draw_text(const std::string& text, const Transform& transform,
		const Color& fg_color, const Color& bg_color,
		float kerning, float line_spacing,
		bool is_screen_space = false, uint32_t entity_id = -1);

// TODO If camera zooms out characters are breaking down.
void draw_text(const std::string& text, Ref<Font> font,
		Transform transform, const Color& fg_color,
		const Color& bg_color, float kerning,
		float line_spacing, bool is_screen_space = false,
		uint32_t entity_id = -1);

void draw_line(const glm::vec2& p0, const glm::vec2& p1, const Color& color = COLOR_WHITE);

void draw_line(const glm::vec3& p0, const glm::vec3& p1, const Color& color = COLOR_WHITE);

void draw_box(const glm::vec2& min, const glm::vec2& max, const Color& color = COLOR_WHITE);

void draw_box(const Transform& transform, const Color& color = COLOR_WHITE);

const RendererStats& get_stats();

void reset_stats();

void begin_batch();

void flush();

void next_batch();

float find_texture_index(const Ref<Texture2D>& texture);

}; //namespace renderer

#endif
