#ifndef ASSET_REGISTRY_H
#define ASSET_REGISTRY_H

#include "asset/asset.h"

using LoadedAssetRegistryMap = std::unordered_map<AssetHandle, Ref<Asset>>;

class AssetRegistry final {
public:
	static void init();

	template <typename T>
	static Ref<T> get(const AssetHandle& handle) {
		if (handle == 0) {
			return nullptr;
		}

		Ref<Asset> asset = get(handle);
		return std::static_pointer_cast<T>(asset);
	}

	static Ref<Asset> get(const AssetHandle& handle);

	// TODO make an async version
	static AssetHandle load(const std::string& path, AssetType type);

	static void unload(const AssetHandle& handle);

	static void unload(const std::string& path);

	static void unload_all();

	static bool is_loaded(const AssetHandle& handle);

	static AssetHandle get_handle_from_path(const std::string& path);

	static void on_asset_rename(const fs::path& old_path, const fs::path& new_path);

	static LoadedAssetRegistryMap& get_loaded_assets();
};

#endif
