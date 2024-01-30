#include "asset/asset_loader.h"

#include "project/project.h"

Ref<Texture2D> AssetLoader::load_texture(const fs::path& path) {
	// TODO get metadata from registry
	Ref<Texture2D> texture = create_ref<Texture2D>(path);
	return texture;
}

Ref<Font> AssetLoader::load_font(const fs::path& path) {
	Ref<Font> font = create_ref<Font>(path);
	return font;
}

Ref<Scene> AssetLoader::load_scene(const fs::path& path) {
	Ref<Scene> scene = create_ref<Scene>();
	if (!Scene::deserialize(scene, path)) {
		return nullptr;
	}
	return scene;
}
