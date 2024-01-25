#include "scene/scene_manager.h"

Ref<Scene> SceneManager::active_scene = nullptr;
bool SceneManager::running = false;
bool SceneManager::paused = false;
uint32_t SceneManager::step_frames = 0;

void SceneManager::play() {
	running = true;
}

void SceneManager::update(float dt) {
	if (paused && step_frames-- <= 0) {
		return;
	}
}

void SceneManager::stop() {
	running = false;
}

void SceneManager::set_paused(bool p_paused) {
	paused = p_paused;
}

void SceneManager::step(uint32_t frames) {
	step_frames = frames;
}

bool SceneManager::is_running() {
	return running;
}

bool SceneManager::is_paused() {
	return paused;
}

void SceneManager::set_active(Ref<Scene> scene) {
	active_scene = scene;
}

Ref<Scene> SceneManager::get_active() {
	return active_scene;
}
