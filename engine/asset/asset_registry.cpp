#include "asset/asset_registry.h"

#include "asset/asset_loader.h"
#include "project/project.h"
#include "renderer/font.h"
#include "renderer/texture.h"
#include "scene/scene.h"

#include <FileWatch.hpp>

// necessarry to not get compiler error with the EVE_LOG__ERROR();
#undef ERROR

namespace asset_registry {

inline static void on_file_change(const fs::path& path_rel, const filewatch::Event change_type) {
	static fs::path s_old_path;

	// skip metadata files
	if (path_rel.extension() == ".meta") {
		return;
	}

	const fs::path path = Project::get_asset_directory() / path_rel;
	const std::string relative_path = Project::get_relative_asset_path(path.string());

	const AssetType type = get_asset_type_from_extension(path.extension().string());

	// if filename changed apply it
	switch (change_type) {
		case filewatch::Event::renamed_old: {
			s_old_path = path;
			break;
		}
		case filewatch::Event::renamed_new: {
			on_asset_rename(s_old_path, path);
			break;
		}
		case filewatch::Event::modified: {
			break;
		}
		case filewatch::Event::removed: {
			if (type == AssetType::NONE || fs::exists(path)) {
				break;
			}

			// if the removed type is scene and the scene is still running
			// it would stay existing as long as we dont exit
			unload_asset(path.string());

			if (type != AssetType::SCENE) {
				const fs::path meta_path = path.string() + ".meta";
				if (fs::exists(meta_path)) {
					fs::remove(meta_path);
				}
			}

			break;
		}
		default: {
			break;
		}
	}
}

inline static AssetRegistryMap s_loaded_assets;
inline static Ref<filewatch::FileWatch<std::string>> s_watcher;
inline static std::unordered_map<fs::path, AssetHandle> s_file_uid_cache;

void init() {
	if (s_watcher) {
		s_watcher.reset();
	}

	s_watcher = create_ref<filewatch::FileWatch<std::string>>(
			Project::get_asset_directory().string(),
			on_file_change);
}

Ref<Asset> get_asset(const AssetHandle& handle) {
	EVE_PROFILE_FUNCTION();

	const auto it = s_loaded_assets.find(handle);
	if (it == s_loaded_assets.end()) {
		return nullptr;
	}

	return it->second;
}

AssetHandle load_asset(const std::string& path, AssetType type) {
	EVE_PROFILE_FUNCTION();

	const fs::path path_abs = Project::get_asset_path(path);
	if (!fs::exists(path_abs)) {
		return INVALID_UID;
	}

	AssetHandle handle = get_handle_from_path(path_abs.string());
	if (s_loaded_assets.find(handle) != s_loaded_assets.end()) {
		return handle;
	}

	Ref<Asset> asset = nullptr;
	switch (type) {
		case AssetType::TEXTURE:
			asset = asset_loader::load_texture(path_abs);
			break;
		case AssetType::FONT:
			asset = asset_loader::load_font(path_abs);
			break;
		case AssetType::SCENE:
			asset = asset_loader::load_scene(path_abs);
			break;
		default:
			return INVALID_UID;
	}

	if (!asset) {
		EVE_LOG_ENGINE_ERROR("Unable to load asset from: {0}.", path_abs.string());
		return INVALID_UID;
	}

	// unload if already has that id to reload
	unload_asset(asset->handle);

	s_loaded_assets[asset->handle] = asset;

	return asset->handle;
}

void unload_asset(const AssetHandle& handle) {
	const auto it = s_loaded_assets.find(handle);
	if (it == s_loaded_assets.end()) {
		return;
	}

	s_loaded_assets.erase(it);
}

void unload_asset(const std::string& path) {
	AssetHandle handle = get_handle_from_path(path);
	unload_asset(handle);
}

void unload_all_assets() {
	s_loaded_assets.clear();
}

bool is_asset_loaded(const AssetHandle& handle) {
	return s_loaded_assets.find(handle) != s_loaded_assets.end();
}

AssetHandle get_handle_from_path(const std::string& path) {
	EVE_PROFILE_FUNCTION();

	const fs::path path_abs = Project::get_asset_path(path);

	// return handle from cache if found
	if (const auto it = s_file_uid_cache.find(path_abs); it != s_file_uid_cache.end()) {
		return it->second;
	}

	const AssetType type = get_asset_type_from_extension(path_abs.extension().string());

	const fs::path asset_path = (type == AssetType::SCENE)
			? path_abs
			: path_abs.string() + ".meta";

	Json json{};
	if (!json_utils::read_file(asset_path, json)) {
		return INVALID_UID;
	}

	if (!json["uid"].is_number()) {
		return INVALID_UID;
	}

	AssetHandle handle = json["uid"].get<AssetHandle>();

	// add handle to cache
	s_file_uid_cache[path_abs] = handle;

	return handle;
}

void on_asset_rename(const fs::path& old_path, const fs::path& new_path) {
	EVE_PROFILE_FUNCTION();

	const std::string old_path_rel = Project::get_relative_asset_path(old_path.string());
	const std::string new_path_rel = Project::get_relative_asset_path(new_path.string());

	const AssetType type = get_asset_type_from_extension(new_path.extension().string());
	if (type == AssetType::SCENE) {
		return;
	}

	const fs::path asset_path_old = old_path.string() + ".meta";
	const fs::path asset_path_new = new_path.string() + ".meta";

	fs::rename(asset_path_old, asset_path_new);

	Json json{};
	if (!json_utils::read_file(asset_path_new, json)) {
		return;
	}

	AssetHandle handle = json["uid"].get<AssetHandle>();
	if (is_asset_loaded(handle)) {
		Ref<Asset> asset = s_loaded_assets.at(handle);
		asset->path = new_path_rel;
	}

	json["path"] = Project::get_relative_asset_path(new_path.string());

	json_utils::write_file(asset_path_new, json);
}

AssetRegistryMap& get_loaded_assets() {
	return s_loaded_assets;
}

} //namespace asset_registry
