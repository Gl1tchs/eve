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

	/**
	 * @brief loads asset to memory
	 *
	 * @param path path of the asset
	 * @param type type of the asset
	 * @param handle optional id of the asset default will be a random UID
	 * @return AssetHandle new handle of the asset
	 */
	static AssetHandle load(const std::string& path, AssetType type, AssetHandle handle = AssetHandle());

	/**
	 * @brief removes asset data from loaded assets
	 *
	 * @param handle asset to unload
	 */
	static void unload(const AssetHandle& handle);

	/**
	 * @brief removes asset data from both assets and loaded assets
	 *
	 * @param handle asset to remoev
	 */
	static void remove(const AssetHandle& handle);

	static bool exists(const AssetHandle& handle);

	static bool is_loaded(const AssetHandle& handle);

	static AssetRegistryMap& get_loaded_assets();

	static void serialize(const fs::path& path);

	static bool deserialize(const fs::path& path);

private:
	static std::unordered_map<AssetHandle, AssetImportData> assets;

	static AssetRegistryMap loaded_assets;
};

#endif
