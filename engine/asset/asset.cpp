#include "asset/asset.h"

std::string deserialize_asset_type(const AssetType type) {
	switch (type) {
		case AssetType::TEXTURE:
			return "Texture";
		case AssetType::FONT:
			return "Font";
		default:
			return "";
	}
}
