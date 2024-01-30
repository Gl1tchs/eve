#include "asset/asset.h"

std::string serialize_asset_type(const AssetType type) {
	switch (type) {
		case AssetType::TEXTURE:
			return "TEXTURE";
		case AssetType::FONT:
			return "FONT";
		case AssetType::SCENE:
			return "SCENE";
		default:
			return "";
	}
}
