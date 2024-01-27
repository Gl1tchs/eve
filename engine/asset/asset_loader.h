#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H

#include "renderer/font.h"
#include "renderer/texture.h"
#include "scene/entity.h"

struct AssetLoader {
	static Ref<Texture2D> load_texture(const fs::path& path);

	static Ref<Font> load_font(const fs::path& path);

	static Ref<Scene> load_scene(const fs::path& path);
};

#endif