#ifndef RENDERER_H
#define RENDERER_H

#include "core/color.h"
#include "renderer/camera.h"
#include "scene/components.h"
#include "scene/transform.h"

constexpr uint64_t MAX_TEXTURE_COUNT = 32;

struct RendererStats {
	uint32_t quad_count = 0;
	uint32_t vertex_count = 0;
	uint32_t index_count = 0;
	uint32_t draw_calls = 0;
};

class Renderer final {
public:
	static void init();

	static void shutdown();

	static void begin_pass(const CameraData& camera_data);

	static void end_pass();

	static void draw_sprite(const SpriteRenderer& sprite, const Transform& transform, uint32_t entity_id = -1);

	static void draw_quad(const Transform& transform, const Color& color, uint32_t entity_id = -1);

	static void draw_quad(const Transform& transform, Ref<Texture2D> texture,
			const glm::vec2& tex_tiling, uint32_t entity_id = -1);

	static void draw_quad(const Transform& transform,
			Ref<Texture2D> texture, const Color& color,
			const glm::vec2& tex_tiling, uint32_t entity_id = -1);

	static void draw_text(const TextRenderer& text_comp,
			const Transform& transform,
			uint32_t entity_id = -1);

	static void draw_text(const std::string& text, const Transform& transform,
			const Color& fg_color,
			const Color& bg_color,
			float kerning, float line_spacing, uint32_t entity_id = -1);

	// TODO If camera zooms out characters are breaking down.
	static void draw_text(const std::string& text, Ref<Font> font,
			const Transform& transform, const Color& fg_color,
			const Color& bg_color,
			float kerning, float line_spacing, uint32_t entity_id = -1);

	static void draw_line(const glm::vec2& p0, const glm::vec2& p1, const Color& color = COLOR_WHITE);

	static void draw_line(const glm::vec3& p0, const glm::vec3& p1, const Color& color = COLOR_WHITE);

	static void draw_box(const glm::vec2& min, const glm::vec2& max, const Color& color = COLOR_WHITE);

	static const RendererStats& get_stats();

	static void reset_stats();

private:
	static void _begin_batch();

	static void _flush();

	static void _next_batch();

	static float _find_texture_index(const Ref<Texture2D>& texture);
};

#endif
