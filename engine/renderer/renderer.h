#ifndef RENDERER_H
#define RENDERER_H

#include "core/buffer.h"
#include "core/color.h"
#include "renderer/camera.h"
#include "renderer/font.h"
#include "renderer/primitives/line.h"
#include "renderer/primitives/quad.h"
#include "renderer/primitives/text.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/uniform_buffer.h"
#include "renderer/vertex_array.h"
#include "scene/transform.h"

constexpr uint64_t MAX_TEXTURE_COUNT = 32;

class Renderer final {
public:
	Renderer();
	~Renderer() = default;

	void begin_pass(const CameraData& camera_data);

	// DISCLAIMER
	//	This won't set any camera data
	//	do not try to access u_camera buffer
	//	from shader it will return old passes value.
	void begin_pass();

	void end_pass();

	void draw_quad(const TransformComponent& transform, const Color& color, uint32_t entity_id = -1);

	void draw_quad(const TransformComponent& transform, Ref<Texture2D> texture,
			const glm::vec2& tex_tiling, uint32_t entity_id = -1);

	void draw_quad(const TransformComponent& transform,
			Ref<Texture2D> texture, const Color& color,
			const glm::vec2& tex_tiling, uint32_t entity_id = -1);

	void draw_string(const std::string& text, const TransformComponent& transform,
			const Color& fg_color,
			const Color& bg_color,
			float kerning, float line_spacing, uint32_t entity_id = -1);

	// FIXME
	//  If camera zooms out characters are breaking down.
	void draw_string(const std::string& text, Ref<Font> font,
			const TransformComponent& transform, const Color& fg_color,
			const Color& bg_color,
			float kerning, float line_spacing, uint32_t entity_id = -1);

	void draw_line(const glm::vec3& p0, const glm::vec3& p1, const Color& color = COLOR_WHITE);

private:
	void _begin_batch();

	void _flush();

	void _next_batch();

	float _find_texture_index(const Ref<Texture2D>& texture);

private:
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
