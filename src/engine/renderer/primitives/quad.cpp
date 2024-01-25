#include "renderer/primitives/quad.h"

#include "renderer/index_buffer.h"
#include "renderer/renderer_api.h"

QuadPrimitive::QuadPrimitive() :
		_vertices(QUAD_MAX_VERTEX_COUNT) {
	_vertex_array = create_ref<VertexArray>();

	// create dynamic vertex buffer
	_vertex_buffer = create_ref<VertexBuffer>(_vertices.size());
	_vertex_buffer->set_layout({
			{ ShaderDataType::FLOAT2, "a_position" },
			{ ShaderDataType::FLOAT2, "a_tex_coords" },
			{ ShaderDataType::FLOAT, "a_z_index" },
			{ ShaderDataType::FLOAT4, "a_color" },
			{ ShaderDataType::FLOAT, "a_tex_index" },
			{ ShaderDataType::FLOAT2, "a_tex_tiling" },
	});
	_vertex_array->add_vertex_buffer(_vertex_buffer);

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
	_vertex_array->set_index_buffer(index_buffer);

	delete[] indices;

	// create shader program
	_shader =
			create_ref<Shader>("shaders/sprite.vert", "shaders/sprite.frag");

	// fill the textures with empty values (which is default white texture)
	{
		_shader->bind();
		int samplers[32];
		std::iota(std::begin(samplers), std::end(samplers), 0);
		_shader->set_uniform("u_textures", 32, samplers);
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
	_white_texture = create_ref<Texture2D>(metadata, &color);

	// Fill texture slots with default white texture
	std::fill(std::begin(_texture_slots), std::end(_texture_slots),
			_white_texture);
}

QuadPrimitive::~QuadPrimitive() {}

void QuadPrimitive::render() {
	if (_index_count <= 0) {
		return;
	}

	// set vertex data
	_vertex_buffer->set_data(_vertices.data(), _vertices.size());

	// bind textures
	for (uint32_t i = 0; i <= _texture_slot_index; i++) {
		_texture_slots[i]->bind(i);
	}

	_shader->bind();
	RendererAPI::draw_indexed(_vertex_array, _index_count);
}

void QuadPrimitive::reset() {
	_vertices.reset_index();
	_index_count = 0;
	_texture_slot_index = 1;
}

bool QuadPrimitive::needs_batch() const {
	return _index_count + QUAD_INDEX_COUNT >= QUAD_MAX_INDEX_COUNT ||
			_texture_slot_index + 1 >= QUAD_MAX_TEXTURE_COUNT;
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

		_vertices.add(vertex);
	}

	_index_count += QUAD_INDEX_COUNT;
}

[[nodiscard]] float QuadPrimitive::find_texture_index(
		const Ref<Texture2D>& texture) {
	float texture_index = 0.0f;
	for (uint32_t i = 1; i < _texture_slot_index; i++) {
		if (_texture_slots[i] == texture) {
			texture_index = (float)i;
		}
	}

	if (texture_index == 0.0f) {
		texture_index = (float)_texture_slot_index;
		_texture_slots[_texture_slot_index++] = texture;
	}

	return texture_index;
}
