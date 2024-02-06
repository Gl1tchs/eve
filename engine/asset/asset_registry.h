#ifndef ASSET_REGISTRY_H
#define ASSET_REGISTRY_H

#include "asset/asset.h"

namespace asset_registry {

using AssetRegistryMap = std::unordered_map<AssetHandle, Ref<Asset>>;

void init();

Ref<Asset> get_asset(const AssetHandle& handle);

template <typename T>
inline Ref<T> get_asset(const AssetHandle& handle) {
	if (handle == 0) {
		return nullptr;
	}

	Ref<Asset> asset = get_asset(handle);
	return std::static_pointer_cast<T>(asset);
}

// TODO make an async version
AssetHandle load_asset(const std::string& path, AssetType type);

void unload_asset(const AssetHandle& handle);

void unload_asset(const std::string& path);

void unload_all_assets();

bool is_asset_loaded(const AssetHandle& handle);

AssetHandle get_handle_from_path(const std::string& path);

void on_asset_rename(const fs::path& old_path, const fs::path& new_path);

AssetRegistryMap& get_loaded_assets();

} //namespace asset_registry

#endif
