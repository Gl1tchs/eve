#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "scene/scene.h"
#include "scene/entity.h"

class SceneManager {
public:
	// FIXME
	//	Make an async version
	static void set_active(AssetHandle handle);
	static Ref<Scene> get_active();

private:
	static Ref<Scene> active_scene;
};

#endif
