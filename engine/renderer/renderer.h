#ifndef RENDERER_H
#define RENDERER_H

#include "core/color.h"
#include "renderer/font.h"
#include "renderer/primitives/quad.h"
#include "renderer/primitives/text.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/uniform_buffer.h"
#include "renderer/vertex_array.h"
#include "scene/transform.h"

constexpr uint64_t MAX_TEXTURE_COUNT = 32;

struct CameraData final {
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec2 position;
};

class Renderer final {
public:
	Renderer();
	~Renderer();

	void begin_pass(const CameraData& camera_data);

	void end_pass();

	void draw_quad(const TransformComponent& transform,
			Ref<Texture2D> texture = nullptr, const Color& color = COLOR_WHITE,
			const glm::vec2& tex_tiling = { 1, 1 });

	// FIXME
	//  If camera zooms out characters are breaking down.
	void draw_string(const std::string& text, Ref<Font> font,
			const TransformComponent& transform, const Color& fg_color = COLOR_WHITE,
			const Color& bg_color = COLOR_TRANSPARENT,
			float kerning = 0.0f, float line_spacing = 0.0f);

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

	// textures
	Ref<Texture2D> white_texture;
	std::array<Ref<Texture2D>, MAX_TEXTURE_COUNT> texture_slots;
	uint32_t texture_slot_index = 0;

	Ref<UniformBuffer> camera_buffer;
};

#endif
