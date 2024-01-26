#include "editor.h"

#include "core/color.h"
#include "core/entrypoint.h"
#include "core/input.h"
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

EditorApplication::~EditorApplication() {}

void EditorApplication::_on_start() {
	Ref<Scene> scene = create_ref<Scene>("test");

	// Ref<Texture2D> texture = create_ref<Texture2D>("res/texture.png", true);

	// for (int i = -10; i <= 10; i++) {
	// 	for (int j = -10; j <= 10; j++) {
	// 		Color color;
	// 		color.r = static_cast<float>(i + 11) / 21.0f;
	// 		color.b = static_cast<float>(j + 11) / 21.0f;
	// 		color.g = (color.r + color.b) / 2.0f;
	// 		color.a = 1.0f;

	// 		Entity sprite = scene->create("sprite " + std::to_string(i) + std::to_string(j));
	// 		auto& sc = sprite.add_component<SpriteRendererComponent>();
	// 		sc.color = color;
	// 		sc.texture = texture;

	// 		sprite.get_transform().local_position = { i, j, 0 };
	// 	}
	// }

	// Entity parent = scene->create("parent");
	// auto& psc = parent.add_component<SpriteRendererComponent>();
	// psc.z_index = 1;
	// psc.color = COLOR_YELLOW;

	// parent.get_transform().local_position = { 0, 1, 0 };

	// Entity child = scene->create("child", parent.get_uid());
	// auto& csc = child.add_component<SpriteRendererComponent>();
	// csc.z_index = 1;
	// csc.color = COLOR_MAGENTA;

	// child.get_transform().local_position = { 0, 1, 0 };

	// Entity text_sample = scene->create("text sample");
	// auto& text_comp = text_sample.add_component<TextRendererComponent>();
	// text_comp.text = "lorem ipsum\ndolor sit amet";
	// text_comp.fg_color = COLOR_MAGENTA;

	// Scene::serialize(scene, "res/scene.escn");
	Scene::deserialize(scene, "res/scene.escn");

	// Ref<Font> font = Font::get_default();

	SceneManager::set_active(scene);
}

void EditorApplication::_on_update(float dt) {
	// TODO cache
	editor_camera->aspect_ratio = window->get_aspect_ratio();

	editor_camera->update(dt);

	scene_renderer->on_viewport_resize(window->get_size());
	scene_renderer->render_editor(dt, editor_camera);
}

void EditorApplication::_on_destroy() {}

Application* create_application(int argc, const char** argv) {
	ApplicationCreateInfo info{};
	info.name = "editor2d";
	info.argc = argc;
	info.argv = argv;
	return new EditorApplication(info);
}
