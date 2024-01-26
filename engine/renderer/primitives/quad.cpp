#include "renderer/primitives/quad.h"

#include "renderer/index_buffer.h"
#include "renderer/renderer_api.h"

QuadPrimitive::QuadPrimitive() :
		vertices(QUAD_MAX_VERTEX_COUNT) {
	vertex_array = create_ref<VertexArray>();

	// create dynamic vertex buffer
	vertex_buffer = create_ref<VertexBuffer>(vertices.get_size());
	vertex_buffer->set_layout({
			{ ShaderDataType::FLOAT2, "a_position" },
			{ ShaderDataType::FLOAT2, "a_tex_coords" },
			{ ShaderDataType::FLOAT, "a_z_index" },
			{ ShaderDataType::FLOAT4, "a_color" },
			{ ShaderDataType::FLOAT, "a_tex_index" },
			{ ShaderDataType::FLOAT2, "a_tex_tiling" },
	});
	vertex_array->add_vertex_buffer(vertex_buffer);

	// initialize index buffer
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

	Ref<IndexBuffer> index_buffer =
			create_ref<IndexBuffer>(indices, QUAD_MAX_INDEX_COUNT);
	vertex_array->set_index_buffer(index_buffer);

	delete[] indices;

	// create shader program
	shader = create_ref<Shader>("shaders/sprite.vert", "shaders/sprite.frag");

	// fill the textures with empty values (which is default white texture)
	{
		shader->bind();
		int samplers[32];
		std::iota(std::begin(samplers), std::end(samplers), 0);
		shader->set_uniform("u_textures", 32, samplers);
	}

	// Create default 1x1 white texture
	TextureMetadata metadata;
	metadata.size = { 1, 1 };
	metadata.format = TextureFormat::RGBA;
	metadata.min_filter = TextureFilteringMode::LINEAR;
	metadata.mag_filter = TextureFilteringMode::LINEAR;
	metadata.wrap_s = TextureWrappingMode::CLAMP_TO_EDGE;
	metadata.wrap_t = TextureWrappingMode::CLAMP_TO_EDGE;
	metadata.generate_mipmaps = false;

	uint32_t color = 0xffffffff;
	white_texture = create_ref<Texture2D>(metadata, &color);

	// Fill texture slots with default white texture
	std::fill(std::begin(texture_slots), std::end(texture_slots),
			white_texture);
}

QuadPrimitive::~QuadPrimitive() {}

void QuadPrimitive::render() {
	if (index_count <= 0) {
		return;
	}

	// set vertex data
	vertex_buffer->set_data(vertices.get_data(), vertices.get_size());

	// bind textures
	for (uint32_t i = 0; i <= texture_slot_index; i++) {
		texture_slots[i]->bind(i);
	}

	shader->bind();
	RendererAPI::draw_indexed(vertex_array, index_count);
}

void QuadPrimitive::reset() {
	vertices.reset_index();
	index_count = 0;
	texture_slot_index = 1;
}

bool QuadPrimitive::needs_batch() const {
	return index_count + QUAD_INDEX_COUNT >= QUAD_MAX_INDEX_COUNT ||
			texture_slot_index + 1 >= QUAD_MAX_TEXTURE_COUNT;
}

void QuadPrimitive::add_instance(const glm::mat4& transform, int z_index,
		Ref<Texture2D> texture,
		const Color& color,
		const glm::vec2& tex_tiling) {
	const float tex_index = texture ? find_texture_index(texture) : 0.0f;

	for (uint32_t i = 0; i < QUAD_VERTEX_COUNT; i++) {
		QuadVertex vertex;
		vertex.position = transform * QUAD_VERTEX_POSITIONS[i];
		vertex.tex_coords = QUAD_TEX_COORDS[i];
		vertex.z_index = z_index;
		vertex.color = color;
		vertex.tex_index = tex_index;
		vertex.tex_tiling = tex_tiling;

		vertices.add(vertex);
	}

	index_count += QUAD_INDEX_COUNT;
}

float QuadPrimitive::find_texture_index(
		const Ref<Texture2D>& texture) {
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
