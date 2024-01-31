#include "scene/scene_manager.h"

#include "asset/asset_registry.h"
#include "core/application.h"

Ref<Scene> SceneManager::active_scene = nullptr;

void SceneManager::load_scene(const std::string& path) {
	//? TODO do not unload shared assets
	if (active_scene) {
		if (active_scene->is_running()) {
			Application::enque_main_thread([path]() {
				active_scene->stop();

				AssetRegistry::unload_all();

				AssetHandle handle = AssetRegistry::load(path, AssetType::SCENE);
				if (!handle) {
					EVE_LOG_ENGINE_ERROR("Unable to load scene from path: {}", path);
					return;
				}

				active_scene = AssetRegistry::get<Scene>(handle);
				active_scene->start();
			});

			return;
		} else {
			AssetRegistry::unload_all();
		}
	}

	AssetHandle handle = AssetRegistry::load(path, AssetType::SCENE);
	if (!handle) {
		EVE_LOG_ENGINE_ERROR("Unable to load scene from path: {}", path);
		return;
	}

	active_scene = AssetRegistry::get<Scene>(handle);
}

void SceneManager::set_active(AssetHandle handle) {
	//? TODO do not unload shared assets
	if (active_scene) {
		if (active_scene->is_running()) {
			Application::enque_main_thread([handle]() {
				active_scene->stop();

				AssetRegistry::unload_all();

				active_scene = AssetRegistry::get<Scene>(handle);
				active_scene->start();
			});

			return;
		} else {
			AssetRegistry::unload_all();
		}
	}

	active_scene = AssetRegistry::get<Scene>(handle);
}

Ref<Scene> SceneManager::get_active() {
	return active_scene;
}
