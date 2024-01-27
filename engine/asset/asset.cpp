#include "asset/asset.h"

std::string deserialize_asset_type(const AssetType type) {
	switch (type) {
		case AssetType::TEXTURE:
			return "texture";
		case AssetType::FONT:
			return "font";
		case AssetType::SCENE:
			return "scene";
		default:
			return "";
	}
}
