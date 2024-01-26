#ifndef TEXTURE_H
#define TEXTURE_H

enum class TextureFormat {
	RED,
	RG,
	RGB,
	BGR,
	RGBA,
	BGRA,
};

enum class TextureFilteringMode {
	NEAREST,
	LINEAR,
};

enum class TextureWrappingMode {
	REPEAT,
	MIRRORED_REPEAT,
	CLAMP_TO_EDGE,
	CLAMP_TO_BORDER,
};

struct TextureMetadata final {
	// will be setted by texture importer
	glm::ivec2 size = { 1, 1 };
	TextureFormat format = TextureFormat::RGBA;
	TextureFilteringMode min_filter = TextureFilteringMode::LINEAR;
	TextureFilteringMode mag_filter = TextureFilteringMode::LINEAR;
	TextureWrappingMode wrap_s = TextureWrappingMode::REPEAT;
	TextureWrappingMode wrap_t = TextureWrappingMode::REPEAT;
	bool generate_mipmaps = true;
};

class Texture2D final {
public:
	Texture2D(const char* path, bool flip_on_load = false);
	Texture2D(const TextureMetadata& metadata, const void* pixels = nullptr);
	~Texture2D();

	// TODO update metadata

	const TextureMetadata& get_metadata() const;

	uint32_t get_renderer_id() const;

	void set_data(void* data, uint32_t size);

	void bind(uint16_t slot = 0) const;

	bool operator==(const Texture2D& other) const;

private:
	void _gen_texture(const TextureMetadata& metadata, const void* pixels = nullptr);

private:
	TextureMetadata metadata;
	uint32_t renderer_id;
};

#endif