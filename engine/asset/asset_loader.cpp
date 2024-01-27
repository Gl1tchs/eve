#include "asset/asset_loader.h"

#include "core/json_utils.h"
#include "project/project.h"

NLOHMANN_JSON_SERIALIZE_ENUM(TextureFilteringMode,
		{
				{ TextureFilteringMode::LINEAR, "linear" },
				{ TextureFilteringMode::NEAREST, "nearest" },
		});

NLOHMANN_JSON_SERIALIZE_ENUM(TextureWrappingMode,
		{
				{ TextureWrappingMode::REPEAT, "repeaat" },
				{ TextureWrappingMode::MIRRORED_REPEAT, "mirrored_repeat" },
				{ TextureWrappingMode::CLAMP_TO_EDGE, "clamp_to_edge" },
				{ TextureWrappingMode::CLAMP_TO_BORDER, "clamp_to_border" },
		});

Ref<Texture2D> AssetLoader::load_texture(const fs::path& path) {
	Json json{};
	if (!json_utils::read_file(path, json)) {
		printf("Failed to load texture file from: %s\n", path.c_str());
		return nullptr;
	}

	EVE_ASSERT(json["type"].get<std::string>() == "texture");

	const std::string asset_path = json["path"].get<std::string>();
	const fs::path asset_path_abs = Project::get_asset_path(asset_path);

	const auto json_metadata = json["metadata"];

	TextureMetadata metadata{};
	metadata.min_filter = json_metadata["min_filter"].get<TextureFilteringMode>();
	metadata.mag_filter = json_metadata["mag_filter"].get<TextureFilteringMode>();
	metadata.wrap_s = json_metadata["wrap_s"].get<TextureWrappingMode>();
	metadata.wrap_t = json_metadata["wrap_t"].get<TextureWrappingMode>();
	metadata.generate_mipmaps = json_metadata["generate_mipmaps"].get<bool>();

	Ref<Texture2D> texture = create_ref<Texture2D>(asset_path_abs, metadata);

	return texture;
}

Ref<Font> AssetLoader::load_font(const fs::path& path) {
	Json json{};
	if (!json_utils::read_file(path, json)) {
		printf("Failed to load font file from: %s\n", path.c_str());
		return nullptr;
	}

	EVE_ASSERT(json["type"].get<std::string>() == "font");

	const std::string asset_path = json["path"].get<std::string>();
	const fs::path asset_path_abs = Project::get_asset_path(asset_path);

	// TODO maybe add metadata to this too?

	Ref<Font> fnont = create_ref<Font>(asset_path_abs);

	return fnont;
}
