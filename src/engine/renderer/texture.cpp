#include "renderer/texture.h"

#include "core/assert.h"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int DeserializeTextureFormat(TextureFormat format) {
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

int DeserializeTextureFilteringMode(TextureFilteringMode mode) {
	switch (mode) {
		case TextureFilteringMode::LINEAR:
			return GL_LINEAR;
		case TextureFilteringMode::NEAREST:
			return GL_NEAREST;
		default:
			return -1;
	}
}

int DeserializeTextureWrappingMode(TextureWrappingMode mode) {
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

Texture2D::Texture2D(const char *path, bool flip_on_load) {
	stbi_set_flip_vertically_on_load(flip_on_load);

	int width, height, channels;
	stbi_uc *data = stbi_load(path, &width, &height, &channels, 0);

	if (!data) {
		stbi_image_free(data);
		printf("Unable to load texture from: %s", path);
		EVE_ASSERT(false);
	}

	TextureMetadata metadata;
	metadata.size = glm::ivec2{ width, height };
	metadata.min_filter = TextureFilteringMode::LINEAR;
	metadata.mag_filter = TextureFilteringMode::LINEAR;
	metadata.wrap_s = TextureWrappingMode::REPEAT;
	metadata.wrap_t = TextureWrappingMode::REPEAT;
	metadata.generate_mipmaps = true;

	switch (channels) {
		case 1:
			metadata.format = TextureFormat::RED;
			break;
		case 2:
			metadata.format = TextureFormat::RG;
			break;
		case 3:
			metadata.format = TextureFormat::RGB;
			break;
		case 4:
			metadata.format = TextureFormat::RGBA;
			break;
		default:
			EVE_ASSERT(false, "Unsupported number of channels in the image");
			break;
	}

	// save metadata
	_metadata = metadata;

	gen_texture(_metadata, data);

	stbi_image_free(data);
}

Texture2D::Texture2D(const TextureMetadata &metadata, const void *pixels) :
		_metadata(metadata) {
	gen_texture(_metadata, pixels);
}

Texture2D::~Texture2D() {
	glDeleteTextures(1, &_renderer_id);
}

const TextureMetadata &Texture2D::metadata() const {
	return _metadata;
}

uint32_t Texture2D::renderer_id() const {
	return _renderer_id;
}

void Texture2D::set_data(void *data, uint32_t size) {
	int format = DeserializeTextureFormat(_metadata.format);

	uint32_t bpp = format == GL_RGBA ? 4 : 3;
	EVE_ASSERT(size == _metadata.size.x * _metadata.size.y * bpp,
			"Data must be entire texture!");

	glTextureSubImage2D(_renderer_id, 0, 0, 0, _metadata.size.x,
			_metadata.size.y, format, GL_UNSIGNED_BYTE, data);
}

void Texture2D::bind(uint16_t slot) const {
	glBindTextureUnit(slot, _renderer_id);
}

bool Texture2D::operator==(const Texture2D &other) const {
	return _renderer_id == other.renderer_id();
}

void Texture2D::gen_texture(const TextureMetadata &metadata,
		const void *pixels) {
	glGenTextures(1, &_renderer_id);
	glBindTexture(GL_TEXTURE_2D, _renderer_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			DeserializeTextureFilteringMode(metadata.min_filter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			DeserializeTextureFilteringMode(metadata.mag_filter));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			DeserializeTextureWrappingMode(metadata.wrap_s));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			DeserializeTextureWrappingMode(metadata.wrap_t));

	glTexImage2D(GL_TEXTURE_2D, 0, DeserializeTextureFormat(_metadata.format),
			_metadata.size.x, _metadata.size.y, 0,
			DeserializeTextureFormat(_metadata.format), GL_UNSIGNED_BYTE,
			pixels);

	if (_metadata.generate_mipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}
