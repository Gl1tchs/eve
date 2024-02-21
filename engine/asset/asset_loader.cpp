#include "asset/asset_loader.h"

#include "project/project.h"
#include "renderer/font.h"
#include "renderer/texture.h"
#include "scene/entity.h"

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

namespace asset_loader {

Ref<Texture2D> load_texture(const fs::path& path) {
	EVE_PROFILE_FUNCTION();

	// Get .meta path
	fs::path metadata_path = path;
	metadata_path.replace_extension(path.extension().string() + ".meta");

	const std::string metadata_path_rel = Project::get_relative_asset_path(metadata_path.string());

	// Create .meta if it doesn't exist
	if (!fs::exists(metadata_path)) {
		Ref<Texture2D> texture = create_ref<Texture2D>(path);
		texture->handle = AssetHandle();
		texture->path = Project::get_relative_asset_path(path.string());

		// Create and populate JSON data
		Json json{};
		json["uid"] = texture->handle;
		json["path"] = texture->path;
		json["type"] = AssetType::TEXTURE;

		Json& json_metadata = json["metadata"];
		TextureMetadata metadata{};
		json_metadata["min_filter"] = metadata.min_filter;
		json_metadata["mag_filter"] = metadata.mag_filter;
		json_metadata["wrap_s"] = metadata.wrap_s;
		json_metadata["wrap_t"] = metadata.wrap_t;
		json_metadata["generate_mipmaps"] = metadata.generate_mipmaps;

		// Write JSON data to file
		json_utils::write_file(metadata_path, json);

		return texture;
	}

	// Read JSON data from file
	Json json{};
	if (!json_utils::read_file(metadata_path, json)) {
		EVE_LOG_ENGINE_ERROR("Failed to load texture file from: {}", path.string());
		return nullptr;
	}

	EVE_ASSERT_ENGINE(json["type"].get<std::string>() == "texture");

	const std::string asset_path = json["path"].get<std::string>();

	// Extract metadata from JSON
	const auto json_metadata = json["metadata"];
	TextureMetadata metadata{};
	metadata.min_filter = json_metadata["min_filter"].get<TextureFilteringMode>();
	metadata.mag_filter = json_metadata["mag_filter"].get<TextureFilteringMode>();
	metadata.wrap_s = json_metadata["wrap_s"].get<TextureWrappingMode>();
	metadata.wrap_t = json_metadata["wrap_t"].get<TextureWrappingMode>();
	metadata.generate_mipmaps = json_metadata["generate_mipmaps"].get<bool>();

	// Create and return texture

	Ref<Texture2D> texture = create_ref<Texture2D>(Project::get_asset_path(asset_path), metadata);
	texture->handle = json["uid"].get<AssetHandle>();
	texture->path = asset_path;

	return texture;
}

Ref<Font> load_font(const fs::path& path) {
	EVE_PROFILE_FUNCTION();

	// Get .meta path
	fs::path metadata_path = path;
	metadata_path.replace_extension(path.extension().string() + ".meta");

	const std::string metadata_path_rel = Project::get_relative_asset_path(metadata_path.string());

	// Create .meta if it doesn't exist
	if (!fs::exists(metadata_path)) {
		Ref<Font> font = create_ref<Font>(path);
		font->handle = AssetHandle();
		font->path = Project::get_relative_asset_path(path.string());

		// Create and populate JSON data
		Json json{};
		json["uid"] = font->handle;
		json["path"] = font->path;
		json["type"] = AssetType::FONT;

		// Write JSON data to file
		json_utils::write_file(metadata_path, json);

		return font;
	}

	Json json{};
	if (!json_utils::read_file(metadata_path.string(), json)) {
		EVE_LOG_ENGINE_ERROR("Failed to load font file from: {}", path.string());
		return nullptr;
	}

	EVE_ASSERT_ENGINE(json["type"].get<std::string>() == "font");

	const std::string asset_path = json["path"].get<std::string>();

	// TODO maybe add metadata to this too?
	Ref<Font> font = create_ref<Font>(Project::get_asset_path(asset_path));

	font->handle = json["uid"].get<AssetHandle>();
	font->path = asset_path;

	return font;
}

} //namespace asset_loader
