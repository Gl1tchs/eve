#ifndef ASSET_H
#define ASSET_H

#include "core/json_utils.h"
#include "core/uid.h"

#define EVE_IMPL_ASSET(type)              \
	AssetType get_type() const override { \
		return type;                      \
	};

typedef UID AssetHandle;

enum class AssetType : uint8_t {
	NONE = 0,
	TEXTURE,
	FONT,
	SCENE
};

std::string serialize_asset_type(const AssetType type);

struct Asset {
	AssetHandle handle;
	std::string path;

	virtual ~Asset() = default;

	virtual AssetType get_type() const = 0;
};

#endif
