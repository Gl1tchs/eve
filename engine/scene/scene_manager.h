#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "scene/scene.h"

class SceneManager {
public:
	static void init();

	static void play();

	static void update(float dt);

	static void stop();

	static void set_paused(bool paused);

	static void step(uint32_t frames);

	static bool is_running();

	static bool is_paused();

	static void set_active(Ref<Scene> scene);
	static Ref<Scene> get_active();

private:
	static Ref<Scene> active_scene;
	static bool running;
	static bool paused;
	static uint32_t step_frames;
};

#endif
