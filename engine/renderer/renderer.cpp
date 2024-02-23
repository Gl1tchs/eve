#include "renderer/renderer.h"

#include "core/buffer.h"
#include "renderer/font.h"
#include "renderer/primitives/line.h"
#include "renderer/primitives/quad.h"
#include "renderer/primitives/text.h"
#include "renderer/render_command.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/uniform_buffer.h"
#include "renderer/vertex_array.h"
#include "renderer/vertex_buffer.h"

namespace renderer {

struct RenderData {
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

	CameraData camera_data{};
	Ref<UniformBuffer> camera_buffer;
};

static RenderData* s_data = nullptr;

void init() {
	EVE_PROFILE_FUNCTION();

	// initialize render data
	s_data = new RenderData();

	RenderCommand::init();

	// quad data
	s_data->quad_vertex_array = create_ref<VertexArray>();

	s_data->quad_vertices.allocate(QUAD_MAX_VERTEX_COUNT);

	s_data->quad_vertex_buffer =
			create_ref<VertexBuffer>(s_data->quad_vertices.get_size());
	s_data->quad_vertex_buffer->set_layout({
			{ ShaderDataType::FLOAT3, "a_position" },
			{ ShaderDataType::FLOAT2, "a_tex_coord" },
			{ ShaderDataType::FLOAT4, "a_color" },
			{ ShaderDataType::FLOAT, "a_tex_index" },
			{ ShaderDataType::FLOAT2, "a_tex_tiling" },
			{ ShaderDataType::INT, "a_entity_id" },
	});
	s_data->quad_vertex_array->add_vertex_buffer(s_data->quad_vertex_buffer);

	uint32_t* indices = new uint32_t[QUAD_MAX_INDEX_COUNT];

	uint32_t offset = 0;
	for (uint32_t i = 0; i < QUAD_MAX_INDEX_COUNT; i += QUAD_INDEX_COUNT) {
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;

		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0;

		offset += QUAD_VERTEX_COUNT;
	}

	Ref<IndexBuffer> quad_index_buffer =
			create_ref<IndexBuffer>(indices, QUAD_MAX_INDEX_COUNT);
	s_data->quad_vertex_array->set_index_buffer(quad_index_buffer);

	delete[] indices;

	s_data->quad_shader = create_ref<Shader>(
			"shaders/sprite.vert.spv", "shaders/sprite.frag.spv");

	// fill the textures with empty values (which is default white texture)
	{
		s_data->quad_shader->bind();
		int samplers[32];
		std::iota(std::begin(samplers), std::end(samplers), 0);
		s_data->quad_shader->set_uniform("u_textures", 32, samplers);
	}

	// text data
	s_data->text_vertex_array = create_ref<VertexArray>();

	s_data->text_vertices.allocate(QUAD_MAX_VERTEX_COUNT);

	s_data->text_vertex_buffer =
			create_ref<VertexBuffer>(s_data->text_vertices.get_size());
	s_data->text_vertex_buffer->set_layout({
			{ ShaderDataType::FLOAT3, "a_position" },
			{ ShaderDataType::FLOAT2, "a_tex_coord" },
			{ ShaderDataType::FLOAT4, "a_fg_color" },
			{ ShaderDataType::FLOAT4, "a_bg_color" },
			{ ShaderDataType::INT, "a_is_screen_space" },
			{ ShaderDataType::INT, "a_entity_id" },
	});
	s_data->text_vertex_array->add_vertex_buffer(s_data->text_vertex_buffer);
	s_data->text_vertex_array->set_index_buffer(quad_index_buffer);

	s_data->text_shader = create_ref<Shader>(
			"shaders/text.vert.spv", "shaders/text.frag.spv");

	// line data
	s_data->line_vertex_array = create_ref<VertexArray>();

	s_data->line_vertices.allocate(LINE_MAX_VERTEX_COUNT);

	s_data->line_vertex_buffer =
			create_ref<VertexBuffer>(s_data->line_vertices.get_size());
	s_data->line_vertex_buffer->set_layout({
			{ ShaderDataType::FLOAT3, "a_position" },
			{ ShaderDataType::FLOAT4, "a_color" },
	});
	s_data->line_vertex_array->add_vertex_buffer(s_data->line_vertex_buffer);

	s_data->line_shader = create_ref<Shader>(
			"shaders/line.vert.spv", "shaders/line.frag.spv");

	// Create default 1x1 white texture
	TextureMetadata metadata;
	metadata.generate_mipmaps = false;

	uint32_t color = 0xffffffff;
	s_data->white_texture =
			create_ref<Texture2D>(metadata, &color, glm::ivec2{ 1, 1 });

	// fill texture slots with default white texture
	std::fill(std::begin(s_data->texture_slots),
			std::end(s_data->texture_slots), s_data->white_texture);

	// create camera uniform buffer
	s_data->camera_buffer = create_ref<UniformBuffer>(sizeof(CameraData), 0);
}

void shutdown() {
	// destroy render data
	delete s_data;
}

void begin_pass(const CameraData& camera_data) {
	s_data->camera_data = camera_data;
	s_data->camera_buffer->set_data(&s_data->camera_data, sizeof(CameraData));

	begin_batch();
}

void end_pass() { flush(); }

void draw_quad(
		const Transform& transform, const Color& color, uint32_t entity_id) {
	draw_quad(transform, nullptr, color, { 1, 1 }, entity_id);
}

void draw_quad(const Transform& transform, Ref<Texture2D> texture,
		const glm::vec2& tex_tiling, uint32_t entity_id) {
	draw_quad(transform, texture, COLOR_WHITE, tex_tiling, entity_id);
}

void draw_quad(const Transform& transform, Ref<Texture2D> texture,
		const Color& color, const glm::vec2& tex_tiling, uint32_t entity_id) {
	if (quad_needs_batch(s_data->quad_index_count) ||
			(texture && s_data->texture_slot_index + 1 >= MAX_TEXTURE_COUNT)) {
		next_batch();
	}

	const float tex_index = texture ? find_texture_index(texture) : 0.0f;

	const glm::mat4 transform_matrix = transform.get_transform_matrix();

	for (uint32_t i = 0; i < QUAD_VERTEX_COUNT; i++) {
		QuadVertex vertex;
		vertex.position = transform_matrix * QUAD_VERTEX_POSITIONS[i];
		vertex.tex_coord = QUAD_TEX_COORDS[i];
		vertex.color = color;
		vertex.tex_index = tex_index;
		vertex.tex_tiling = tex_tiling;
		vertex.entity_id = entity_id;

		s_data->quad_vertices.add(vertex);
	}

	s_data->quad_index_count += QUAD_INDEX_COUNT;

	s_data->stats.quad_count++;
	s_data->stats.vertex_count += QUAD_VERTEX_COUNT;
	s_data->stats.index_count += QUAD_INDEX_COUNT;
}

void draw_text(const std::string& text, const Transform& transform,
		const Color& fg_color, const Color& bg_color, float kerning,
		float line_spacing, bool is_screen_space, uint32_t entity_id) {
	draw_text(text, Font::get_default(), transform, fg_color, bg_color, kerning,
			line_spacing, is_screen_space, entity_id);
}

void draw_text(const std::string& text, Ref<Font> font, Transform transform,
		const Color& fg_color, const Color& bg_color, float kerning,
		float line_spacing, bool is_screen_space, uint32_t entity_id) {
	if (!font) {
		font = Font::get_default();
	}

	Ref<Texture2D> font_atlas = font->get_atlas_texture();
	if (font_atlas != s_data->font_atlas_texture) {
		next_batch();
	}

	s_data->font_atlas_texture = font_atlas;

	const auto& font_geometry = font->get_msdf_data().font_geometry;
	const auto& metrics = font_geometry.getMetrics();

	double fs_scale = 1.0 / (metrics.ascenderY - metrics.descenderY);

	const float space_glyph_advance = font_geometry.getGlyph(' ')->getAdvance();

	// center the text around the transform if its not in screen space
	if (!is_screen_space) {
		float text_width =
				get_text_size(text, font, kerning).x * transform.get_scale().x;
		transform.local_position.x -= text_width / 2.0f;
	} else {
		transform.local_position.x *= s_data->camera_data.aspect_ratio;
	}

	const glm::mat4 transform_matrix = transform.get_transform_matrix();

	double x = 0.0;
	double y = 0.0;

	for (size_t i = 0; i < text.size(); i++) {
		char character = text[i];
		if (character == '\r') {
			continue;
		}

		if (character == '\n') {
			x = 0;
			y -= fs_scale * metrics.lineHeight + line_spacing;
			continue;
		}

		if (character == ' ') {
			float advance = space_glyph_advance;
			if (i < text.size() - 1) {
				char next_character = text[i + 1];
				double d_advance;
				font_geometry.getAdvance(d_advance, character, next_character);
				advance = (float)d_advance;
			}

			x += fs_scale * advance + kerning;
			continue;
		}

		if (character == '\t') {
			x += 4.0f * (fs_scale * space_glyph_advance + kerning);
			continue;
		}

		auto glyph = font_geometry.getGlyph(character);
		if (!glyph) {
			glyph = font_geometry.getGlyph('?');
		}
		if (!glyph) {
			continue;
		}

		double al, ab, ar, at;
		glyph->getQuadAtlasBounds(al, ab, ar, at);
		glm::vec2 tex_coord_min((float)al, (float)ab);
		glm::vec2 tex_coord_max((float)ar, (float)at);

		double pl, pb, pr, pt;
		glyph->getQuadPlaneBounds(pl, pb, pr, pt);
		glm::vec2 quad_min((float)pl, (float)pb);
		glm::vec2 quad_max((float)pr, (float)pt);

		quad_min *= fs_scale, quad_max *= fs_scale;
		quad_min += glm::vec2(x, y);
		quad_max += glm::vec2(x, y);

		const glm::vec2 texel_size = 1.0f / (glm::vec2)font_atlas->get_size();

		tex_coord_min *= texel_size;
		tex_coord_max *= texel_size;

		// render here
		TextVertex vertex;

		vertex.position = transform_matrix * glm::vec4(quad_min, 0.0f, 1.0f);
		vertex.tex_coord = tex_coord_min;
		vertex.fg_color = fg_color;
		vertex.bg_color = bg_color;
		vertex.is_screen_space = is_screen_space;
		vertex.entity_id = entity_id;
		s_data->text_vertices.add(vertex);

		vertex.position = transform_matrix *
				glm::vec4(quad_min.x, quad_max.y, 0.0f, 1.0f);
		vertex.tex_coord = { tex_coord_min.x, tex_coord_max.y };
		vertex.fg_color = fg_color;
		vertex.bg_color = bg_color;
		vertex.is_screen_space = is_screen_space;
		vertex.entity_id = entity_id;
		s_data->text_vertices.add(vertex);

		vertex.position = transform_matrix * glm::vec4(quad_max, 0.0f, 1.0f);
		vertex.tex_coord = tex_coord_max;
		vertex.fg_color = fg_color;
		vertex.bg_color = bg_color;
		vertex.is_screen_space = is_screen_space;
		vertex.entity_id = entity_id;
		s_data->text_vertices.add(vertex);

		vertex.position = transform_matrix *
				glm::vec4(quad_max.x, quad_min.y, 0.0f, 1.0f);
		vertex.tex_coord = { tex_coord_max.x, tex_coord_min.y };
		vertex.fg_color = fg_color;
		vertex.bg_color = bg_color;
		vertex.is_screen_space = is_screen_space;
		vertex.entity_id = entity_id;
		s_data->text_vertices.add(vertex);

		s_data->text_index_count += QUAD_INDEX_COUNT;

		if (i < text.size() - 1) {
			double advance = glyph->getAdvance();
			const char next_character = text[i + 1];
			font_geometry.getAdvance(advance, character, next_character);

			x += fs_scale * advance + kerning;
		}

		s_data->stats.quad_count++;
		s_data->stats.vertex_count += QUAD_VERTEX_COUNT;
		s_data->stats.index_count += QUAD_INDEX_COUNT;
	}
}

void draw_line(const glm::vec2& p0, const glm::vec2& p1, const Color& color) {
	draw_line({ p0.x, p0.y, 0 }, { p1.x, p1.y, 0 }, color);
}

void draw_line(const glm::vec3& p0, const glm::vec3& p1, const Color& color) {
	LineVertex vertex;

	vertex.position = p0;
	vertex.color = color;

	s_data->line_vertices.add(vertex);

	vertex.position = p1;
	vertex.color = color;

	s_data->line_vertices.add(vertex);

	s_data->stats.vertex_count += 2;
}

void draw_box(const glm::vec2& min, const glm::vec2& max, const Color& color) {
	draw_line(min, { min.x, max.y }, color);
	draw_line({ min.x, max.y }, max, color);
	draw_line(max, { max.x, min.y }, color);
	draw_line({ max.x, min.y }, min, color);
}

void draw_box(const Transform& transform, const Color& color) {
	// https://en.wikipedia.org/wiki/Rotation_matrix
	glm::mat4 transform_matrix = transform.get_transform_matrix();

	glm::vec2 lb(transform_matrix * QUAD_VERTEX_POSITIONS[0]);
	glm::vec2 lt(transform_matrix * QUAD_VERTEX_POSITIONS[1]);
	glm::vec2 rt(transform_matrix * QUAD_VERTEX_POSITIONS[2]);
	glm::vec2 rb(transform_matrix * QUAD_VERTEX_POSITIONS[3]);

	draw_line(lb, lt, color);
	draw_line(lt, rt, color);
	draw_line(rt, rb, color);
	draw_line(rb, lb, color);
}

const RendererStats& get_stats() { return s_data->stats; }

void reset_stats() { memset(&s_data->stats, 0, sizeof(RendererStats)); }

void begin_batch() {
	s_data->quad_vertices.reset_index();
	s_data->quad_index_count = 0;

	s_data->text_vertices.reset_index();
	s_data->text_index_count = 0;

	s_data->line_vertices.reset_index();

	s_data->texture_slot_index = 1;
}

void flush() {
	EVE_PROFILE_FUNCTION();

	if (s_data->line_vertices.get_count() > 0) {
		s_data->line_vertex_buffer->set_data(s_data->line_vertices.get_data(),
				s_data->line_vertices.get_size());

		s_data->line_shader->bind();
		RenderCommand::draw_lines(
				s_data->line_vertex_array, s_data->line_vertices.get_count());

		s_data->stats.draw_calls++;
	}

	if (s_data->text_index_count > 0) {
		s_data->text_vertex_buffer->set_data(s_data->text_vertices.get_data(),
				s_data->text_vertices.get_size());

		if (s_data->font_atlas_texture) {
			s_data->font_atlas_texture->bind();
		}

		s_data->text_shader->bind();
		RenderCommand::draw_indexed(
				s_data->text_vertex_array, s_data->text_index_count);

		s_data->stats.draw_calls++;
	}

	if (s_data->quad_index_count > 0) {
		s_data->quad_vertex_buffer->set_data(s_data->quad_vertices.get_data(),
				s_data->quad_vertices.get_size());

		for (uint32_t i = 0; i <= s_data->texture_slot_index; i++) {
			s_data->texture_slots[i]->bind(i);
		}

		s_data->quad_shader->bind();
		RenderCommand::draw_indexed(
				s_data->quad_vertex_array, s_data->quad_index_count);

		s_data->stats.draw_calls++;
	}
}

void next_batch() {
	flush();
	begin_batch();
}

float find_texture_index(const Ref<Texture2D>& texture) {
	float texture_index = 0.0f;
	for (uint32_t i = 1; i < s_data->texture_slot_index; i++) {
		if (s_data->texture_slots[i] == texture) {
			texture_index = (float)i;
		}
	}

	if (texture_index == 0.0f) {
		texture_index = (float)s_data->texture_slot_index;
		s_data->texture_slots[s_data->texture_slot_index++] = texture;
	}

	return texture_index;
}

} //namespace renderer
