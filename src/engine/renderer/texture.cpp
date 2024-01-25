#include "renderer/texture.h"

#include "core/assert.h"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int deserialize_texture_format(TextureFormat format) {
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

int deserialize_texture_filtering_mode(TextureFilteringMode mode) {
	switch (mode) {
		case TextureFilteringMode::LINEAR:
			return GL_LINEAR;
		case TextureFilteringMode::NEAREST:
			return GL_NEAREST;
		default:
			return -1;
	}
}

int deserialize_texture_wrapping_mode(TextureWrappingMode mode) {
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

Texture2D::Texture2D(const char* path, bool flip_on_load) {
	stbi_set_flip_vertically_on_load(flip_on_load);

	int width, height, channels;
	stbi_uc* data = stbi_load(path, &width, &height, &channels, 0);

	if (!data) {
		stbi_image_free(data);
		printf("Unable to load texture from: %s", path);
		EVE_ASSERT(false);
	}

	TextureMetadata texture_metadata;
	texture_metadata.size = glm::ivec2{ width, height };
	texture_metadata.min_filter = TextureFilteringMode::LINEAR;
	texture_metadata.mag_filter = TextureFilteringMode::LINEAR;
	texture_metadata.wrap_s = TextureWrappingMode::REPEAT;
	texture_metadata.wrap_t = TextureWrappingMode::REPEAT;
	texture_metadata.generate_mipmaps = true;

	switch (channels) {
		case 1:
			texture_metadata.format = TextureFormat::RED;
			break;
		case 2:
			texture_metadata.format = TextureFormat::RG;
			break;
		case 3:
			texture_metadata.format = TextureFormat::RGB;
			break;
		case 4:
			texture_metadata.format = TextureFormat::RGBA;
			break;
		default:
			EVE_ASSERT(false, "Unsupported number of channels in the image");
			break;
	}

	// save metadata
	metadata = texture_metadata;

	_gen_texture(texture_metadata, data);

	stbi_image_free(data);
}

Texture2D::Texture2D(const TextureMetadata& metadata, const void* pixels) :
		metadata(metadata) {
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

void Texture2D::set_data(void* data, uint32_t size) {
	int format = deserialize_texture_format(metadata.format);

	uint32_t bpp = format == GL_RGBA ? 4 : 3;
	EVE_ASSERT(size == metadata.size.x * metadata.size.y * bpp,
			"Data must be entire texture!");

	glTextureSubImage2D(renderer_id, 0, 0, 0, metadata.size.x,
			metadata.size.y, format, GL_UNSIGNED_BYTE, data);
}

void Texture2D::bind(uint16_t slot) const {
	glBindTextureUnit(slot, renderer_id);
}

bool Texture2D::operator==(const Texture2D& other) const {
	return renderer_id == other.get_renderer_id();
}

void Texture2D::_gen_texture(const TextureMetadata& metadata,
		const void* pixels) {
	glGenTextures(1, &renderer_id);
	glBindTexture(GL_TEXTURE_2D, renderer_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			deserialize_texture_filtering_mode(metadata.min_filter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			deserialize_texture_filtering_mode(metadata.mag_filter));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			deserialize_texture_wrapping_mode(metadata.wrap_s));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			deserialize_texture_wrapping_mode(metadata.wrap_t));

	glTexImage2D(GL_TEXTURE_2D, 0, deserialize_texture_format(metadata.format),
			metadata.size.x, metadata.size.y, 0,
			deserialize_texture_format(metadata.format), GL_UNSIGNED_BYTE,
			pixels);

	if (metadata.generate_mipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}
