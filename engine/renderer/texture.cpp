#include "renderer/texture.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int texture_format_to_gl(TextureFormat format) {
	switch (format) {
		case TextureFormat::RED:
			return GL_RED;
		case TextureFormat::RG:
			return GL_RG;
		case TextureFormat::RGB:
			return GL_RGB;
		case TextureFormat::BGR:
			return GL_BGR;
		case TextureFormat::RGBA:
			return GL_RGBA;
		case TextureFormat::BGRA:
			return GL_BGRA;
		default:
			return -1;
	}
}

inline static int texture_filtering_mode_to_gl(TextureFilteringMode mode) {
	switch (mode) {
		case TextureFilteringMode::LINEAR:
			return GL_LINEAR;
		case TextureFilteringMode::NEAREST:
			return GL_NEAREST;
		default:
			return -1;
	}
}

inline static int texture_wrapping_mode_to_gl(TextureWrappingMode mode) {
	switch (mode) {
		case TextureWrappingMode::REPEAT:
			return GL_REPEAT;
		case TextureWrappingMode::MIRRORED_REPEAT:
			return GL_MIRRORED_REPEAT;
		case TextureWrappingMode::CLAMP_TO_EDGE:
			return GL_CLAMP_TO_EDGE;
		case TextureWrappingMode::CLAMP_TO_BORDER:
			return GL_CLAMP_TO_BORDER;
		default:
			return -1;
	}
}

inline static TextureFormat get_texture_format_from_channels(const int channels) {
	switch (channels) {
		case 1:
			return TextureFormat::RED;
		case 2:
			return TextureFormat::RG;
		case 3:
			return TextureFormat::RGB;
		case 4:
			return TextureFormat::RGBA;
		default:
			break;
	}

	EVE_ASSERT(false, "Unsupported number of channels in the image");
	return TextureFormat::RED;
}

Texture2D::Texture2D(const fs::path& path, bool flip_on_load) :
		renderer_id(0) {
	EVE_PROFILE_FUNCTION();

	stbi_set_flip_vertically_on_load(flip_on_load);

	int channels;
	stbi_uc* data = stbi_load(path.string().c_str(), &size.x, &size.y, &channels, 0);

	if (!data) {
		stbi_image_free(data);
		EVE_LOG_ERROR("Unable to load texture from: {}", path.string());
		EVE_ASSERT(false);
	}

	TextureMetadata texture_metadata;
	texture_metadata.format = get_texture_format_from_channels(channels);
	texture_metadata.min_filter = TextureFilteringMode::LINEAR;
	texture_metadata.mag_filter = TextureFilteringMode::LINEAR;
	texture_metadata.wrap_s = TextureWrappingMode::CLAMP_TO_EDGE;
	texture_metadata.wrap_t = TextureWrappingMode::CLAMP_TO_EDGE;
	texture_metadata.generate_mipmaps = true;

	// generate texture (this will also set the metadata)
	_gen_texture(texture_metadata, data);

	stbi_image_free(data);
}

Texture2D::Texture2D(const fs::path& path, const TextureMetadata& _metadata, bool flip_on_load, bool override_texture_format) :
		renderer_id(0) {
	EVE_PROFILE_FUNCTION();

	stbi_set_flip_vertically_on_load(flip_on_load);

	int channels;
	stbi_uc* data = stbi_load(path.string().c_str(), &size.x, &size.y, &channels, 0);

	if (!data) {
		stbi_image_free(data);
		EVE_LOG_ERROR("Unable to load texture from: {}", path.string());
		EVE_ASSERT(false);

		return;
	}

	TextureMetadata texture_metadata;
	texture_metadata.format = override_texture_format
			? _metadata.format
			: get_texture_format_from_channels(channels);
	texture_metadata.min_filter = _metadata.min_filter;
	texture_metadata.mag_filter = _metadata.mag_filter;
	texture_metadata.wrap_s = _metadata.wrap_s;
	texture_metadata.wrap_t = _metadata.wrap_t;
	texture_metadata.generate_mipmaps = _metadata.generate_mipmaps;

	// generate texture (this will also set the metadata)
	_gen_texture(texture_metadata, data);

	stbi_image_free(data);
}

Texture2D::Texture2D(const TextureMetadata& metadata, const void* pixels, const glm::ivec2& size) :
		renderer_id(0), metadata(metadata), size(size) {
	EVE_PROFILE_FUNCTION();

	_gen_texture(metadata, pixels);
}

Texture2D::~Texture2D() {
	glDeleteTextures(1, &renderer_id);
}

const TextureMetadata& Texture2D::get_metadata() const {
	return metadata;
}

uint32_t Texture2D::get_renderer_id() const {
	return renderer_id;
}

void Texture2D::set_data(void* data, uint32_t _size) {
	const int format = texture_format_to_gl(metadata.format);

	uint32_t bpp = format == GL_RGBA ? 4 : 3;
	EVE_ASSERT(_size == size.x * size.y * bpp,
			"Data must be entire texture!");

	glTextureSubImage2D(renderer_id, 0, 0, 0, size.x,
			size.y, format, GL_UNSIGNED_BYTE, data);
}

const glm::ivec2& Texture2D::get_size() const {
	return size;
}

void Texture2D::set_metadata(const TextureMetadata& _metadata) {
	metadata = _metadata;

	glBindTexture(GL_TEXTURE_2D, renderer_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			texture_filtering_mode_to_gl(metadata.min_filter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			texture_filtering_mode_to_gl(metadata.mag_filter));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			texture_wrapping_mode_to_gl(metadata.wrap_s));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			texture_wrapping_mode_to_gl(metadata.wrap_t));

	if (metadata.generate_mipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}

void Texture2D::bind(uint16_t slot) const {
	glBindTextureUnit(slot, renderer_id);
}

bool Texture2D::operator==(const Texture2D& other) const {
	return renderer_id == other.get_renderer_id();
}

void Texture2D::_gen_texture(const TextureMetadata& _metadata,
		const void* pixels) {
	glGenTextures(1, &renderer_id);
	glBindTexture(GL_TEXTURE_2D, renderer_id);

	set_metadata(_metadata);

	glTexImage2D(GL_TEXTURE_2D, 0, texture_format_to_gl(metadata.format),
			size.x, size.y, 0,
			texture_format_to_gl(metadata.format), GL_UNSIGNED_BYTE,
			pixels);
}
