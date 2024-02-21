#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "scene/entity.h"

class SceneManager {
public:
	// TODO	Make an async version
	static bool load_scene(const std::string& path);

	static Ref<Scene>& get_active();

private:
	static Ref<Scene> s_active_scene;
};

#endif
