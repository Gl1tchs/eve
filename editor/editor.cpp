#include "editor.h"

#include "widgets/dock_space.h"

#include "asset/asset_registry.h"
#include "core/entrypoint.h"
#include "core/input.h"
#include "project/project.h"
#include "renderer/frame_buffer.h"
#include "renderer/renderer.h"
#include "scene/entity.h"
#include "scene/scene_manager.h"

#include <imgui.h>

EditorApplication::EditorApplication(const ApplicationCreateInfo& info) :
		Application(info) {
	stats = create_scope<StatsPanel>(renderer);

	hierarchy = create_ref<HierarchyPanel>();
	inspector = create_ref<InspectorPanel>(hierarchy);

	editor_camera = create_ref<EditorCamera>();
	scene_renderer = create_ref<SceneRenderer>(renderer);

	// bind entity selection beheaviour
	scene_renderer->push_beheaviour(RenderBeheaviourTickFormat::AFTER_RENDER, BIND_FUNC(_handle_entity_selection));

	_setup_menubar();
}

void EditorApplication::_on_start() {
	Project::load("sample/sample.eve");
	SceneManager::load_scene(Project::get_starting_scene_path());
}

void EditorApplication::_on_update(float dt) {
	static bool pressed = false;
	if (!pressed && Input::is_key_pressed(KeyCode::SPACE)) {
		SceneManager::load_scene("res://scene2.escn");
		pressed = true;
	}

	// resize
	_on_viewport_resize();

	if (viewport.is_focused()) {
		editor_camera->update(dt);
	}

	// render image to frame buffer
	scene_renderer->render_editor(dt, editor_camera);
}

void EditorApplication::_on_imgui_update(float dt) {
	DockSpace::begin();

	menubar.render();

	if (!SceneManager::get_active()) {
		DockSpace::end();
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	viewport.set_render_texture_id(scene_renderer->get_final_texture_id());
	viewport.render();
	ImGui::PopStyleVar();

	stats->render();

	hierarchy->render();
	inspector->render();

	content_browser.render();
	console.render();

	DockSpace::end();
}

void EditorApplication::_on_destroy() {
}

void EditorApplication::_setup_menubar() {
	Menu file_menu{ "File",
		{ { "Exit", "Ctrl+Shift+Q", [this]() { _quit(); } } } };
	menubar.push_menu(file_menu);

	Menu view_menu{
		"View",
		{
				{ "Viewport",
						[this]() { viewport.set_active(true); } },
				{ "Hierarchy",
						[this]() { hierarchy->set_active(true); } },
				{ "Inspector",
						[this]() { inspector->set_active(true); } },
				{ "Content Browser", [this]() { content_browser.set_active(true); } },
				{ "Console", [this]() { console.set_active(true); } },
				{ "Stats", [this]() { stats->set_active(true); } },
		}
	};

	menubar.push_menu(view_menu);
}

void EditorApplication::_on_viewport_resize() {
	const auto viewport_size = viewport.get_size();
	if (viewport_size.x > 0 && viewport_size.y > 0) {
		editor_camera->aspect_ratio = (float)viewport_size.x / (float)viewport_size.y;
		scene_renderer->on_viewport_resize({ viewport_size.x, viewport_size.y });
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

		// Convert the pixel data to entity
		Entity hovered_entity = (pixel_data == -1)
				? INVALID_ENTITY
				: Entity(entt::entity(pixel_data), SceneManager::get_active().get());

		// Check for left mouse click and a valid hovered entity
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && hovered_entity) {
			hierarchy->set_selected_entity(hovered_entity);
		}
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
