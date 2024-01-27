#ifndef ASSET_REGISTRY_H
#define ASSET_REGISTRY_H

#include "asset/asset.h"

struct AssetImportData {
	std::string path;
	AssetType type;
};

using AssetRegistryMap = std::unordered_map<AssetHandle, Ref<Asset>>;

class AssetRegistry {
public:
	AssetRegistry() = default;

	template <typename T>
	static Ref<T> get(const AssetHandle& handle) {
		if (handle == 0) {
			return nullptr;
		}

		Ref<Asset> asset = get(handle);
		return std::static_pointer_cast<T>(asset);
	}

	static Ref<Asset> get(const AssetHandle& handle);

	static AssetHandle subscribe(AssetImportData asset, AssetHandle handle = AssetHandle());

	// FIXME
	//	make an async version
	static AssetHandle load(const std::string& path, AssetType type, AssetHandle handle = AssetHandle());

	static void unload(const AssetHandle& handle);

	static void unload_all();

	static void remove(const AssetHandle& handle);

	static bool exists(const AssetHandle& handle);

	static bool exists_as(const AssetHandle& handle, AssetType type);

	static bool is_loaded(const AssetHandle& handle);

	static AssetRegistryMap& get_loaded_assets();

	static void serialize(const fs::path& path);

	static bool deserialize(const fs::path& path);

private:
	static std::unordered_map<AssetHandle, AssetImportData> assets;
	static AssetRegistryMap loaded_assets;
};

#endif
