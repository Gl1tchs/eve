#include "editor.h"

#include "widgets/dock_space.h"

#include "core/entrypoint.h"
#include "core/input.h"
#include "project/project.h"
#include "renderer/frame_buffer.h"
#include "renderer/primitives/text.h"
#include "renderer/renderer.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/scene_manager.h"
#include "scripting/script_engine.h"

#include <imgui.h>
#include <tinyfiledialogs.h>

EditorApplication::EditorApplication(const ApplicationCreateInfo& info) :
		Application(info) {
	scene_renderer = create_ref<SceneRenderer>();
	editor_camera = create_ref<EditorCamera>();

	_setup_menubar();
	_setup_toolbar();
}

void EditorApplication::_on_start() {
}

inline static bool s_default_title = true;

void EditorApplication::_on_update(float dt) {
	if (s_default_title && g_modify_info.modified) {
		window->set_title(Project::get_name() + " *");
		s_default_title = false;
	}

	// resize
	_on_viewport_resize();

	switch (state) {
		case SceneState::EDIT: {
			if (viewport.is_focused()) {
				editor_camera->update(dt);
			}

			if (Ref<Scene> scene = SceneManager::get_active(); scene) {
				// bind entity selection beheaviour
				scene_renderer->submit(RenderFuncTickFormat::AFTER_RENDER, BIND_FUNC(_handle_entity_selection));

				// TODO this should not be affected by post processing
				scene_renderer->submit(RenderFuncTickFormat::ON_RENDER, [this, &scene](const Ref<FrameBuffer>& fb) {
					for (auto entity : scene->get_selected_entities()) {
						_render_entity_bounds(entity);
					}
				});

				scene_renderer->render_editor(dt, editor_camera);
			}

			_handle_shortcuts();

			break;
		}
		case SceneState::PAUSED: {
			_handle_shortcuts();
		}
		case SceneState::PLAY: {
			if (Ref<Scene> scene = SceneManager::get_active(); scene) {
				scene->update(dt);
				scene_renderer->render_runtime(dt);
			}

			break;
		}
		default:
			break;
	}
}

void EditorApplication::_on_imgui_update(float dt) {
	DockSpace::begin();

	menubar.render();

	if (!SceneManager::get_active()) {
		DockSpace::end();
		return;
	}

	toolbar.render();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	viewport.set_render_texture_id(scene_renderer->get_final_texture_id());
	viewport.render();
	ImGui::PopStyleVar();

	hierarchy.render();
	inspector.render();

	content_browser.render();
	console.render();
	stats.render();

	DockSpace::end();
}

void EditorApplication::_on_destroy() {
}

void EditorApplication::_setup_menubar() {
	Menu file_menu{
		"File",
		{
				{ "Open Project", "Ctrl+Shift+O", BIND_FUNC(_open_project) },
				{ "Save", "Ctrl+S", BIND_FUNC(_save_active_scene) },
				{ "Save As", "Ctrl+Shift+S", BIND_FUNC(_save_active_scene_as) },
				{ "Exit", "Ctrl+Shift+Q", BIND_FUNC(quit) },
		}
	};
	menubar.push_menu(file_menu);

	Menu view_menu{
		"View",
		{
				{ "Viewport",
						[this]() { viewport.set_active(true); } },
				{ "Toolbar", [this]() { toolbar.set_active(true); } },
				{ "Hierarchy",
						[this]() { hierarchy.set_active(true); } },
				{ "Inspector",
						[this]() { inspector.set_active(true); } },
				{ "Content Browser", [this]() { content_browser.set_active(true); } },
				{ "Console", [this]() { console.set_active(true); } },
				{ "Stats", [this]() { stats.set_active(true); } },
		}
	};

	menubar.push_menu(view_menu);
}

void EditorApplication::_setup_toolbar() {
	toolbar.on_play = BIND_FUNC(_on_scene_play);
	toolbar.on_stop = BIND_FUNC(_on_scene_stop);
	toolbar.on_pause = BIND_FUNC(_on_scene_pause);
	toolbar.on_resume = BIND_FUNC(_on_scene_resume);
	toolbar.on_step = BIND_FUNC(_on_scene_step);
}

void EditorApplication::_on_viewport_resize() {
	const auto viewport_size = viewport.get_size();

	// this will cache it on it's own
	scene_renderer->on_viewport_resize({ viewport_size.x, viewport_size.y });

	if (viewport_size.x > 0 && viewport_size.y > 0) {
		editor_camera->aspect_ratio = (float)viewport_size.x / (float)viewport_size.y;
	}
}

void EditorApplication::_handle_entity_selection(Ref<FrameBuffer> frame_buffer) {
	auto [mx, my] = ImGui::GetMousePos();
	mx -= viewport.get_min_bounds().x;
	my -= viewport.get_min_bounds().y;

	const glm::vec2 viewport_size = viewport.get_max_bounds() - viewport.get_min_bounds();

	// Ensure the correct y-coordinate inversion
	my = viewport_size.y - my;

	const int mouse_x = static_cast<int>(mx);
	const int mouse_y = static_cast<int>(my);

	// Check if the mouse is within the viewport bounds
	if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < viewport_size.x && mouse_y < viewport_size.y) {
		int pixel_data;
		frame_buffer->read_pixel(1, mouse_x, mouse_y, FrameBufferTextureFormat::RED_INT, &pixel_data);

		auto scene = SceneManager::get_active();

		// Convert the pixel data to entity
		Entity hovered_entity = (pixel_data == -1)
				? INVALID_ENTITY
				: Entity(entt::entity(pixel_data), scene.get());

		// Check for left mouse click and a valid hovered entity
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			if (hovered_entity) {
				if (!ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
					scene->clear_selected_entities();
				}

				scene->toggle_entity_selection(hovered_entity);
			} else {
				scene->clear_selected_entities();
			}
		}
	}
}

void EditorApplication::_save_active_scene() {
	if (!editor_scene) {
		return;
	}

	if (!editor_scene->get_path().empty()) {
		Scene::serialize(editor_scene, editor_scene->get_path());

		_on_scene_save();
	} else {
		_save_active_scene_as();
	}
}

void EditorApplication::_save_active_scene_as() {
	if (!editor_scene) {
		return;
	}

	const char* filter_patterns[1] = { "*.escn" };
	const char* path = tinyfd_saveFileDialog("Save Scene", "scene.escn", 1,
			filter_patterns, "Eve Scene Files");

	if (!path) {
		EVE_LOG_ENGINE_ERROR("Unable to save scene to path.");
		return;
	}

	Scene::serialize(editor_scene, path);

	_on_scene_save();
}

void EditorApplication::_open_project() {
	const char* filter_patterns[1] = { "*.eve" };
	const char* path = tinyfd_openFileDialog(
			"Open Project", "", 1, filter_patterns, "Eve Project Files", 0);

	if (!path) {
		EVE_LOG_ENGINE_ERROR("Unable to open project from path.");
		return;
	}

	if (Project::load(fs::path(path))) {
		ScriptEngine::init();

		// load the first scene
		EVE_ASSERT_ENGINE(SceneManager::load_scene(Project::get_starting_scene_path()));

		editor_scene = SceneManager::get_active();
		editor_scene->clear_selected_entities();

		window->set_title(Project::get_name());
	}
}

void EditorApplication::_on_scene_save() {
	window->set_title(Project::get_name());
	s_default_title = true;

	g_modify_info.on_save();
}

void EditorApplication::_set_scene_state(SceneState _state) {
	state = _state;
	toolbar.set_state(state);
}

void EditorApplication::_on_scene_open() {
	editor_scene = SceneManager::get_active();
}

void EditorApplication::_on_scene_play() {
	_set_scene_state(SceneState::PLAY);

	SceneManager::get_active() = Scene::copy(editor_scene);
	SceneManager::get_active()->start();
}

void EditorApplication::_on_scene_stop() {
	_set_scene_state(SceneState::EDIT);

	SceneManager::get_active()->stop();

	SceneManager::get_active() = editor_scene;
}

void EditorApplication::_on_scene_pause() {
	_set_scene_state(SceneState::PAUSED);
	SceneManager::get_active()->set_paused(true);
}

void EditorApplication::_on_scene_resume() {
	_set_scene_state(SceneState::PLAY);
	SceneManager::get_active()->set_paused(false);
}

void EditorApplication::_on_scene_step() {
	SceneManager::get_active()->step();
}

void EditorApplication::_handle_shortcuts() {
	if (Input::is_key_pressed(KeyCode::LEFT_CONTROL)) {
		if (Input::is_key_pressed(KeyCode::S)) {
			_save_active_scene();
		}

		if (Input::is_key_pressed(KeyCode::P)) {
			if (editor_scene) {
				_on_scene_play();
			}
		}

		if (Input::is_key_pressed(KeyCode::LEFT_SHIFT)) {
			if (Input::is_key_pressed(KeyCode::S)) {
				_save_active_scene_as();
			}

			if (Input::is_key_pressed(KeyCode::O)) {
				_open_project();
			}

			if (Input::is_key_pressed(KeyCode::Q)) {
				quit();
			}
		}
	}
}

void EditorApplication::_render_entity_bounds(Entity entity) {
	const Transform& transform = entity.get_transform();

	if (entity.has_component<SpriteRenderer>()) {
		renderer::draw_box(transform, COLOR_GREEN);
	}

	if (entity.has_component<TextRenderer>()) {
		const TextRenderer& text_renderer = entity.get_component<TextRenderer>();
		if (text_renderer.is_screen_space) {
			return;
		}

		Transform text_transform = transform;
		glm::vec2 scale = text_transform.get_scale();

		Ref<Font> font = editor_scene->get_asset_registry().get_asset<Font>(text_renderer.font);

		glm::vec2 text_size = get_text_size(text_renderer.text, font, text_renderer.kerning) * scale;

		text_transform.local_position.y += (scale.y / 2.0f) - (text_size.y / 2.0f);

		text_transform.local_scale.x = text_size.x;
		text_transform.local_scale.y += text_size.y;

		renderer::draw_box(text_transform, COLOR_GREEN);
	}

	for (auto child : entity.get_children()) {
		_render_entity_bounds(child);
	}
}

// Application entrypoint
Application* create_application(int argc, const char** argv) {
	ApplicationCreateInfo info{};
	info.name = "editor";
	info.argc = argc;
	info.argv = argv;
	return new EditorApplication(info);
}
