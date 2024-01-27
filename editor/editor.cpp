#include "editor.h"

#include "asset/asset.h"
#include "asset/asset_registry.h"
#include "core/color.h"
#include "core/entrypoint.h"
#include "core/input.h"
#include "project/project.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/scene_manager.h"
#include "scene/scene_renderer.h"

EditorApplication::EditorApplication(const ApplicationCreateInfo& info) :
		Application(info) {
	editor_camera = create_ref<EditorCamera>();
	scene_renderer = create_ref<SceneRenderer>(renderer);
}

void EditorApplication::_on_start() {
	Project::load("sample/sample.eve");

	Ref<Scene> scene = create_ref<Scene>();
	Scene::deserialize(scene, "sample/assets/scene.escn");

	SceneManager::set_active(scene);
}

void EditorApplication::_on_update(float dt) {
	// TODO cache
	editor_camera->aspect_ratio = window->get_aspect_ratio();

	editor_camera->update(dt);

	scene_renderer->on_viewport_resize(window->get_size());
	scene_renderer->render_editor(dt, editor_camera);
}

void EditorApplication::_on_destroy() {
}

Application* create_application(int argc, const char** argv) {
	ApplicationCreateInfo info{};
	info.name = "editor2d";
	info.argc = argc;
	info.argv = argv;
	return new EditorApplication(info);
}
