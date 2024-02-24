#ifndef ASSET_REGISTRY_H
#define ASSET_REGISTRY_H

#include "asset/asset.h"

using AssetPack = std::unordered_map<AssetHandle, Ref<Asset>>;

class AssetRegistry {
public:
	AssetRegistry();
	~AssetRegistry();

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

	AssetHandle load_asset(const std::string& path, AssetType type,
			AssetHandle handle = AssetHandle());

	void unload_asset(const AssetHandle& handle);

	void unload_asset(const std::string& path);

	void unload_all_assets();

	bool is_asset_loaded(const AssetHandle& handle);

	AssetHandle get_handle_from_path(const std::string& path);

	AssetPack& get_assets();

	AssetPack::iterator begin();
	AssetPack::const_iterator begin() const;

	AssetPack::iterator end();
	AssetPack::const_iterator end() const;

private:
	void _on_asset_rename(const fs::path& old_path, const fs::path& new_path);

private:
	AssetPack assets;
	std::unordered_map<fs::path, AssetHandle> file_uid_cache;
};

#endif
