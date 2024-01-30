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
		EVE_LOG_ENGINE_ERROR("Failed to load texture file from: {}", path.string());
		return nullptr;
	}

	EVE_ASSERT_ENGINE(json["type"].get<std::string>() == "TEXTURE");

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
		EVE_LOG_ENGINE_ERROR("Failed to load font file from: {}", path.string());
		return nullptr;
	}

	EVE_ASSERT_ENGINE(json["type"].get<std::string>() == "FONT");

	const std::string asset_path = json["path"].get<std::string>();
	const fs::path asset_path_abs = Project::get_asset_path(asset_path);

	// TODO maybe add metadata to this too?

	Ref<Font> font = create_ref<Font>(asset_path_abs);

	return font;
}

Ref<Scene> AssetLoader::load_scene(const fs::path& path) {
	Ref<Scene> scene = create_ref<Scene>();
	if (!Scene::deserialize(scene, path)) {
		return nullptr;
	}

	return scene;
}
