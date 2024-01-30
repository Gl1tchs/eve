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

AssetType get_asset_type_from_extension(const std::string& extension) {
	if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
			extension == ".bmp" || extension == ".tga" || extension == ".psd" ||
			extension == ".gif" || extension == ".hdr" || extension == ".pic" ||
			extension == ".pnm" || extension == ".ppm" || extension == ".pgm") {
		return AssetType::TEXTURE;
	} else if (extension == ".ttf" || extension == ".otf") {
		return AssetType::FONT;
	} else if (extension == ".escn") {
		return AssetType::SCENE;
	} else {
		return AssetType::NONE;
	}
}
