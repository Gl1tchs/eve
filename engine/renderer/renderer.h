#ifndef RENDERER_H
#define RENDERER_H

#include "core/buffer.h"
#include "core/color.h"
#include "renderer/primitives/line.h"
#include "renderer/primitives/quad.h"
#include "renderer/primitives/text.h"
#include "renderer/shader.h"
#include "renderer/uniform_buffer.h"
#include "renderer/vertex_array.h"
#include "scene/transform.h"

class CameraData;
class SpriteRendererComponent;
class TextRendererComponent;
class Font;
class Texture2D;

constexpr uint64_t MAX_TEXTURE_COUNT = 32;

struct RendererStats {
	uint32_t quad_count = 0;
	uint32_t vertex_count = 0;
	uint32_t index_count = 0;
	uint32_t draw_calls = 0;
};

class Renderer final {
public:
	Renderer();
	~Renderer() = default;

	void begin_pass(const CameraData& camera_data);

	void end_pass();

	void draw_sprite(const SpriteRendererComponent& sprite, const TransformComponent& transform, uint32_t entity_id = -1);

	void draw_quad(const TransformComponent& transform, const Color& color, uint32_t entity_id = -1);

	void draw_quad(const TransformComponent& transform, Ref<Texture2D> texture,
			const glm::vec2& tex_tiling, uint32_t entity_id = -1);

	void draw_quad(const TransformComponent& transform,
			Ref<Texture2D> texture, const Color& color,
			const glm::vec2& tex_tiling, uint32_t entity_id = -1);

	void draw_text(const TextRendererComponent& text_comp,
			const TransformComponent& transform,
			uint32_t entity_id = -1);

	void draw_text(const std::string& text, const TransformComponent& transform,
			const Color& fg_color,
			const Color& bg_color,
			float kerning, float line_spacing, uint32_t entity_id = -1);

	// TODO If camera zooms out characters are breaking down.
	void draw_text(const std::string& text, Ref<Font> font,
			const TransformComponent& transform, const Color& fg_color,
			const Color& bg_color,
			float kerning, float line_spacing, uint32_t entity_id = -1);

	void draw_line(const glm::vec2& p0, const glm::vec2& p1, const Color& color = COLOR_WHITE);

	void draw_line(const glm::vec3& p0, const glm::vec3& p1, const Color& color = COLOR_WHITE);

	void draw_box(const glm::vec2& min, const glm::vec2& max, const Color& color = COLOR_WHITE);

	const RendererStats& get_stats() const;

	void reset_stats();

private:
	void _begin_batch();

	void _flush();

	void _next_batch();

	float _find_texture_index(const Ref<Texture2D>& texture);

private:
	RendererStats stats;

	// quad render data
	Ref<VertexArray> quad_vertex_array;
	Ref<VertexBuffer> quad_vertex_buffer;
	Ref<Shader> quad_shader;

	BufferArray<QuadVertex> quad_vertices;
	uint32_t quad_index_count = 0;

	// text render data
	Ref<VertexArray> text_vertex_array;
	Ref<VertexBuffer> text_vertex_buffer;
	Ref<Shader> text_shader;

	BufferArray<TextVertex> text_vertices;
	uint32_t text_index_count = 0;

	Ref<Texture2D> font_atlas_texture;

	// line render data
	Ref<VertexArray> line_vertex_array;
	Ref<VertexBuffer> line_vertex_buffer;
	Ref<Shader> line_shader;

	BufferArray<LineVertex> line_vertices;

	// textures
	Ref<Texture2D> white_texture;
	std::array<Ref<Texture2D>, MAX_TEXTURE_COUNT> texture_slots;
	uint32_t texture_slot_index = 0;

	Ref<UniformBuffer> camera_buffer;
};

#endif
