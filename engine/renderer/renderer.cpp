#include "renderer/renderer.h"

#include "renderer/primitives/line.h"
#include "renderer/primitives/quad.h"
#include "renderer/primitives/text.h"
#include "renderer/renderer_api.h"
#include "renderer/vertex_buffer.h"

Renderer::Renderer() {
	RendererAPI::init();

	// quad data
	quad_vertex_array = create_ref<VertexArray>();

	quad_vertices.allocate(QUAD_MAX_VERTEX_COUNT);

	quad_vertex_buffer = create_ref<VertexBuffer>(quad_vertices.get_size());
	quad_vertex_buffer->set_layout({
			{ ShaderDataType::FLOAT3, "a_position" },
			{ ShaderDataType::FLOAT2, "a_tex_coord" },
			{ ShaderDataType::FLOAT4, "a_color" },
			{ ShaderDataType::FLOAT, "a_tex_index" },
			{ ShaderDataType::FLOAT2, "a_tex_tiling" },
	});
	quad_vertex_array->add_vertex_buffer(quad_vertex_buffer);

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
	quad_vertex_array->set_index_buffer(quad_index_buffer);

	delete[] indices;

	quad_shader = create_ref<Shader>("shaders/sprite.vert", "shaders/sprite.frag");

	// fill the textures with empty values (which is default white texture)
	{
		quad_shader->bind();
		int samplers[32];
		std::iota(std::begin(samplers), std::end(samplers), 0);
		quad_shader->set_uniform("u_textures", 32, samplers);
	}

	// text data
	text_vertex_array = create_ref<VertexArray>();

	text_vertices.allocate(QUAD_MAX_VERTEX_COUNT);

	text_vertex_buffer = create_ref<VertexBuffer>(text_vertices.get_size());
	text_vertex_buffer->set_layout({
			{ ShaderDataType::FLOAT3, "a_position" },
			{ ShaderDataType::FLOAT2, "a_tex_coord" },
			{ ShaderDataType::FLOAT4, "a_fg_color" },
			{ ShaderDataType::FLOAT4, "a_bg_color" },
	});
	text_vertex_array->add_vertex_buffer(text_vertex_buffer);
	text_vertex_array->set_index_buffer(quad_index_buffer);

	text_shader = create_ref<Shader>("shaders/text.vert", "shaders/text.frag");

	// line data
	line_vertex_array = create_ref<VertexArray>();

	line_vertices.allocate(LINE_MAX_VERTEX_COUNT);

	line_vertex_buffer = create_ref<VertexBuffer>(line_vertices.get_size());
	line_vertex_buffer->set_layout({
			{ ShaderDataType::FLOAT3, "a_position" },
			{ ShaderDataType::FLOAT4, "a_color" },
	});
	line_vertex_array->add_vertex_buffer(line_vertex_buffer);

	line_shader = create_ref<Shader>("shaders/line.vert", "shaders/line.frag");

	// Create default 1x1 white texture
	TextureMetadata metadata;
	metadata.generate_mipmaps = false;

	uint32_t color = 0xffffffff;
	white_texture = create_ref<Texture2D>(metadata, &color);

	// fill texture slots with default white texture
	std::fill(std::begin(texture_slots), std::end(texture_slots),
			white_texture);

	// create camera uniform buffer
	camera_buffer = create_ref<UniformBuffer>(sizeof(CameraData), 0);
}

void Renderer::begin_pass(const CameraData& camera_data) {
	camera_buffer->set_data(&camera_data, sizeof(CameraData));

	_begin_batch();
}

void Renderer::end_pass() {
	_flush();
}

void Renderer::draw_quad(const TransformComponent& transform,
		Ref<Texture2D> texture, const Color& color,
		const glm::vec2& tex_tiling) {
	if (quad_needs_batch(quad_index_count) ||
			(texture && texture_slot_index + 1 >= MAX_TEXTURE_COUNT)) {
		_next_batch();
	}

	const float tex_index = texture ? _find_texture_index(texture) : 0.0f;

	const glm::mat4 transform_matrix = transform.get_transform_matrix();

	for (uint32_t i = 0; i < QUAD_VERTEX_COUNT; i++) {
		QuadVertex vertex;
		vertex.position = transform_matrix * QUAD_VERTEX_POSITIONS[i];
		vertex.tex_coord = QUAD_TEX_COORDS[i];
		vertex.color = color;
		vertex.tex_index = tex_index;
		vertex.tex_tiling = tex_tiling;

		quad_vertices.add(vertex);
	}

	quad_index_count += QUAD_INDEX_COUNT;
}

void Renderer::draw_string(const std::string& text, Ref<Font> font, const TransformComponent& transform,
		const Color& fg_color, const Color& bg_color,
		float kerning, float line_spacing) {
	Ref<Texture2D> font_atlas = font->get_atlas_texture();
	if (font_atlas != font_atlas_texture) {
		_next_batch();
	}

	font_atlas_texture = font_atlas;

	const glm::mat4 transform_matrix = transform.get_transform_matrix();

	const auto& font_geometry = font->get_msdf_data().font_geometry;
	const auto& metrics = font_geometry.getMetrics();

	double x = 0.0;
	double y = 0.0;

	double fs_scale = 1.0 / (metrics.ascenderY - metrics.descenderY);

	const float space_glyph_advance = font_geometry.getGlyph(' ')->getAdvance();

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

		const glm::vec2 texel_size = 1.0f / (glm::vec2)font_atlas->get_metadata().size;

		tex_coord_min *= texel_size;
		tex_coord_max *= texel_size;

		// render here
		TextVertex vertex;

		vertex.position = transform_matrix * glm::vec4(quad_min, 0.0f, 1.0f);
		vertex.tex_coord = tex_coord_min;
		vertex.fg_color = fg_color;
		vertex.bg_color = bg_color;
		text_vertices.add(vertex);

		vertex.position = transform_matrix * glm::vec4(quad_min.x, quad_max.y, 0.0f, 1.0f);
		vertex.tex_coord = { tex_coord_min.x, tex_coord_max.y };
		vertex.fg_color = fg_color;
		vertex.bg_color = bg_color;
		text_vertices.add(vertex);

		vertex.position = transform_matrix * glm::vec4(quad_max, 0.0f, 1.0f);
		vertex.tex_coord = tex_coord_max;
		vertex.fg_color = fg_color;
		vertex.bg_color = bg_color;
		text_vertices.add(vertex);

		vertex.position = transform_matrix * glm::vec4(quad_max.x, quad_min.y, 0.0f, 1.0f);
		vertex.tex_coord = { tex_coord_max.x, tex_coord_min.y };
		vertex.fg_color = fg_color;
		vertex.bg_color = bg_color;
		text_vertices.add(vertex);

		text_index_count += QUAD_INDEX_COUNT;

		if (i < text.size() - 1) {
			double advance = glyph->getAdvance();
			const char next_character = text[i + 1];
			font_geometry.getAdvance(advance, character, next_character);

			x += fs_scale * advance + kerning;
		}
	}
}

void Renderer::draw_line(const glm::vec3& p0, const glm::vec3& p1, const Color& color) {
	LineVertex vertex;

	vertex.position = p0;
	vertex.color = color;

	line_vertices.add(vertex);

	vertex.position = p1;
	vertex.color = color;

	line_vertices.add(vertex);
}

void Renderer::_begin_batch() {
	quad_vertices.reset_index();
	quad_index_count = 0;

	text_vertices.reset_index();
	text_index_count = 0;

	line_vertices.reset_index();

	texture_slot_index = 1;
}

void Renderer::_flush() {
	if (line_vertices.get_count() > 0) {
		line_vertex_buffer->set_data(line_vertices.get_data(), line_vertices.get_size());

		line_shader->bind();
		RendererAPI::draw_lines(line_vertex_array, line_vertices.get_count());
	}

	if (text_index_count > 0) {
		text_vertex_buffer->set_data(text_vertices.get_data(), text_vertices.get_size());

		if (font_atlas_texture) {
			font_atlas_texture->bind();
		}

		text_shader->bind();
		RendererAPI::draw_indexed(text_vertex_array, text_index_count);
	}

	if (quad_index_count > 0) {
		quad_vertex_buffer->set_data(quad_vertices.get_data(), quad_vertices.get_size());

		for (uint32_t i = 0; i <= texture_slot_index; i++) {
			texture_slots[i]->bind(i);
		}

		quad_shader->bind();
		RendererAPI::draw_indexed(quad_vertex_array, quad_index_count);
	}
}

void Renderer::_next_batch() {
	_flush();
	_begin_batch();
}

float Renderer::_find_texture_index(const Ref<Texture2D>& texture) {
	float texture_index = 0.0f;
	for (uint32_t i = 1; i < texture_slot_index; i++) {
		if (texture_slots[i] == texture) {
			texture_index = (float)i;
		}
	}

	if (texture_index == 0.0f) {
		texture_index = (float)texture_slot_index;
		texture_slots[texture_slot_index++] = texture;
	}

	return texture_index;
}
