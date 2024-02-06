#include "scene/scene_manager.h"

#include "asset/asset_registry.h"
#include "core/application.h"

Ref<Scene> SceneManager::s_active_scene = nullptr;

bool SceneManager::load_scene(const std::string& path) {
	//? TODO do not unload shared assets
	if (s_active_scene) {
		if (s_active_scene->is_running()) {
			Application::enque_main_thread([path]() {
				s_active_scene->stop();

				asset_registry::unload_all_assets();

				AssetHandle handle = asset_registry::load_asset(path, AssetType::SCENE);
				if (!handle) {
					EVE_LOG_ENGINE_ERROR("Unable to load scene from path: {}", path);
					return;
				}

				s_active_scene = asset_registry::get_asset<Scene>(handle);
				s_active_scene->start();
			});

			return true;
		} else {
			asset_registry::unload_all_assets();
		}
	}

	AssetHandle handle = asset_registry::load_asset(path, AssetType::SCENE);
	if (!handle) {
		EVE_LOG_ENGINE_ERROR("Unable to load scene from path: {}", path);
		return false;
	}

	s_active_scene = asset_registry::get_asset<Scene>(handle);

	return true;
}

Ref<Scene>& SceneManager::get_active() {
	return s_active_scene;
}
