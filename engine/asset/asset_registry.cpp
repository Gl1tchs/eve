#include "asset/asset_registry.h"

#include "asset/asset_loader.h"
#include "asset_registry.h"
#include "core/json_utils.h"
#include "project/project.h"
#include "scene/scene_manager.h"

NLOHMANN_JSON_SERIALIZE_ENUM(AssetType,
		{
				{ AssetType::NONE, "none" },
				{ AssetType::TEXTURE, "texture" },
				{ AssetType::FONT, "font" },
				{ AssetType::SCENE, "scene" },
		});

std::unordered_map<AssetHandle, AssetImportData> AssetRegistry::assets = {};
LoadedAssetRegistryMap AssetRegistry::loaded_assets = {};

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

Ref<Asset> AssetRegistry::get(const std::string& path) {
	const AssetHandle handle = get_handle_from_path(path);
	if (!handle) {
		return nullptr;
	}

	return get(handle);
}

AssetHandle AssetRegistry::subscribe(AssetImportData asset, AssetHandle handle) {
	assets[handle] = asset;
	return handle;
}

AssetHandle AssetRegistry::load(const std::string& path, AssetType type, AssetHandle handle) {
	const fs::path path_abs = Project::get_asset_path(path);
	if (!fs::exists(path_abs)) {
		return INVALID_UID;
	}

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
			return INVALID_UID;
	}

	if (!asset) {
		EVE_LOG_ENGINE_ERROR("Unable to load asset from: {}.", path);
		return INVALID_UID;
	}

	asset->handle = handle;
	asset->path = path;

	loaded_assets[asset->handle] = asset;

	return handle;
}

void AssetRegistry::unload(const AssetHandle& handle) {
	const auto it = loaded_assets.find(handle);
	if (it == loaded_assets.end()) {
		return;
	}

	loaded_assets.erase(it);
}

void AssetRegistry::unload_all() {
	loaded_assets.clear();
}

void AssetRegistry::remove(const AssetHandle& handle) {
	const auto it = assets.find(handle);
	if (it == assets.end()) {
		return;
	}

	assets.erase(it);
	unload(handle);
}

void AssetRegistry::remove(const std::string& path) {
	const AssetHandle handle = get_handle_from_path(path);
	if (!handle) {
		return;
	}

	remove(handle);
}

bool AssetRegistry::exists(const AssetHandle& handle) {
	const auto it = assets.find(handle);
	if (it == assets.end()) {
		return false;
	}

	return fs::exists(Project::get_asset_path(it->second.path));
}

bool AssetRegistry::exists_as(const AssetHandle& handle, const AssetType type) {
	const auto it = assets.find(handle);
	if (it == assets.end()) {
		return false;
	}

	if (it->second.type != type) {
		return false;
	}

	return fs::exists(Project::get_asset_path(it->second.path));
}

bool AssetRegistry::is_loaded(const AssetHandle& handle) {
	return loaded_assets.find(handle) != loaded_assets.end();
}

void AssetRegistry::on_asset_rename(const fs::path& old_path, const fs::path& new_path) {
	const auto it = std::find_if(assets.begin(), assets.end(), [old_path](const auto& pair) {
		return old_path == Project::get_asset_path(pair.second.path);
	});
	if (it == assets.end()) {
		return;
	}

	const std::string old_path_rel = Project::get_relative_asset_path(old_path.string());
	const std::string new_path_rel = Project::get_relative_asset_path(new_path.string());

	it->second.path = new_path_rel;

	if (is_loaded(it->first)) {
		Ref<Asset> asset = loaded_assets.at(it->first);
		asset->path = new_path_rel;
	}
}

AssetRegistryMap& AssetRegistry::get_assets() {
	return assets;
}

LoadedAssetRegistryMap& AssetRegistry::get_loaded_assets() {
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
		const std::string path = asset_json["path"].get<std::string>();
		if (!fs::exists(Project::get_asset_path(path))) {
			continue;
		}

		const AssetHandle handle = asset_json["handle"].get<uint64_t>();
		const AssetType type = asset_json["type"].get<AssetType>();

		assets[handle] = { path, type };
	}

	return true;
}

AssetHandle AssetRegistry::get_handle_from_path(const std::string& path) {
	const auto it = std::find_if(assets.begin(), assets.end(), [path](const auto& pair) {
		return path == pair.second.path;
	});

	if (it == assets.end()) {
		return INVALID_UID;
	}

	return it->first;
}
