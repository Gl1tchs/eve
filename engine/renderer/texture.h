#ifndef TEXTURE_H
#define TEXTURE_H

#include "asset/asset.h"

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

bool is_texture_filtering_mode_valid(const char* mode);

enum class TextureWrappingMode {
	REPEAT,
	MIRRORED_REPEAT,
	CLAMP_TO_EDGE,
	CLAMP_TO_BORDER,
};

bool is_texture_wrapping_mode_valid(const char* mode);

struct TextureMetadata final {
	// will be setted by texture importer
	TextureFormat format = TextureFormat::RGBA;
	TextureFilteringMode min_filter = TextureFilteringMode::LINEAR;
	TextureFilteringMode mag_filter = TextureFilteringMode::LINEAR;
	TextureWrappingMode wrap_s = TextureWrappingMode::CLAMP_TO_EDGE;
	TextureWrappingMode wrap_t = TextureWrappingMode::CLAMP_TO_EDGE;
	bool generate_mipmaps = true;
};

class Texture2D final : public Asset {
public:
	EVE_IMPL_ASSET(AssetType::TEXTURE)

	Texture2D(const fs::path& path, bool flip_on_load = true);
	Texture2D(const fs::path& path, const TextureMetadata& metadata,
			bool flip_on_load = true, bool override_texture_format = false);
	Texture2D(const TextureMetadata& metadata, const void* pixels,
			const glm::ivec2& size);

	virtual ~Texture2D();

	const glm::ivec2& get_size() const;

	const TextureMetadata& get_metadata() const;
	void set_metadata(const TextureMetadata& _metadata);

	uint32_t get_renderer_id() const;

	void set_data(void* data, uint32_t size);

	void bind(uint16_t slot = 0) const;

	bool operator==(const Texture2D& other) const;

private:
	void _gen_texture(
			const TextureMetadata& _metadata, const void* pixels = nullptr);

private:
	uint32_t renderer_id;

	TextureMetadata metadata;
	glm::ivec2 size = { 0, 0 };
};

#endif
