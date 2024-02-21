#include "scene/scene_manager.h"

#include "core/application.h"

Ref<Scene> SceneManager::s_active_scene = nullptr;

bool SceneManager::load_scene(const std::string& path) {
	//? TODO do not unload shared assets
	if (s_active_scene && s_active_scene->is_running()) {
		Application::enque_main_thread([&]() {
			s_active_scene->stop();

			if (!Scene::deserialize(
						s_active_scene, path)) {
				EVE_LOG_ENGINE_ERROR("Unable to load scene from path: {}", path);
				return;
			}

			s_active_scene->start();
		});

		return true;
	}

	Ref<Scene> scene = create_ref<Scene>();
	if (!Scene::deserialize(
				scene, path)) {
		EVE_LOG_ENGINE_ERROR("Unable to load scene from path: {}", path);
		return false;
	}

	s_active_scene = scene;

	return true;
}

Ref<Scene>& SceneManager::get_active() {
	return s_active_scene;
}
