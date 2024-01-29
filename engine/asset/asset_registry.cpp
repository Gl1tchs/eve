#include "asset/asset_registry.h"

#include "asset/asset_loader.h"
#include "core/json_utils.h"
#include "nlohmann/json.hpp"
#include "project/project.h"

NLOHMANN_JSON_SERIALIZE_ENUM(AssetType,
		{
				{ AssetType::NONE, "none" },
				{ AssetType::TEXTURE, "texture" },
				{ AssetType::FONT, "font" },
				{ AssetType::SCENE, "scene" },
		});

std::unordered_map<AssetHandle, AssetImportData> AssetRegistry::assets = {};
AssetRegistryMap AssetRegistry::loaded_assets = {};

Ref<Asset> AssetRegistry::get(const AssetHandle& handle) {
	if (!is_loaded(handle) && exists(handle)) {
		const AssetImportData& data = assets.at(handle);
		load(data.path, data.type, handle);
	}

	const auto it = loaded_assets.find(handle);
	if (it == loaded_assets.end()) {
		return nullptr;
	}

	return it->second;
}

AssetHandle AssetRegistry::subscribe(AssetImportData asset, AssetHandle handle) {
	assets[handle] = asset;
	return handle;
}

AssetHandle AssetRegistry::load(const std::string& path, AssetType type, AssetHandle handle) {
	const fs::path path_abs = Project::get_asset_path(path);

	Ref<Asset> asset = nullptr;
	switch (type) {
		case AssetType::TEXTURE:
			asset = AssetLoader::load_texture(path_abs);
			break;
		case AssetType::FONT:
			asset = AssetLoader::load_font(path_abs);
			break;
		case AssetType::SCENE:
			asset = AssetLoader::load_scene(path_abs);
			break;
		default:
			return 0;
	}

	if (!asset) {
		EVE_LOG_ENGINE_ERROR("Unable to load asset from: {}.", path);
		return 0;
	}

	asset->handle = handle;
	asset->path = path;

	loaded_assets[asset->handle] = asset;

	return handle;
}

void AssetRegistry::unload(const AssetHandle& handle) {
	const auto it = loaded_assets.find(handle);
	if (it != loaded_assets.end()) {
		loaded_assets.erase(it);
	}
}

void AssetRegistry::unload_all() {
	loaded_assets.clear();
}

void AssetRegistry::remove(const AssetHandle& handle) {
	const auto it = assets.find(handle);
	if (it != assets.end()) {
		assets.erase(it);
	}

	unload(handle);
}

bool AssetRegistry::exists(const AssetHandle& handle) {
	return assets.find(handle) != assets.end();
}

bool AssetRegistry::exists_as(const AssetHandle& handle, AssetType type) {
	const auto it = assets.find(handle);
	if (it == assets.end()) {
		return false;
	}

	return it->second.type == type;
}

bool AssetRegistry::is_loaded(const AssetHandle& handle) {
	return loaded_assets.find(handle) != loaded_assets.end();
}

AssetRegistryMap& AssetRegistry::get_loaded_assets() {
	return loaded_assets;
}

void AssetRegistry::serialize(const fs::path& path) {
	Json out = Json::array();

	for (const auto& [handle, asset] : assets) {
		out.push_back(Json{
				{ "handle", handle.value },
				{ "path", asset.path },
				{ "type", asset.type } });
	}

	json_utils::write_file(path, out);
}

bool AssetRegistry::deserialize(const fs::path& path) {
	assets.clear();
	loaded_assets.clear();

	Json json{};
	if (!json_utils::read_file(path, json)) {
		EVE_LOG_ENGINE_ERROR("Unable to deserialize asset registry to {}", path.string());
		return false;
	}

	for (const auto& asset_json : json) {
		const AssetHandle handle = asset_json["handle"].get<uint64_t>();
		const std::string path = asset_json["path"].get<std::string>();
		const AssetType type = asset_json["type"].get<AssetType>();

		assets[handle] = { path, type };
	}

	return true;
}
