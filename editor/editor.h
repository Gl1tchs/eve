#ifndef EDITOR_H
#define EDITOR_H

#include "panels/console_panel.h"
#include "panels/content_browser.h"
#include "panels/hierarchy_panel.h"
#include "panels/inspector_panel.h"
#include "panels/stats_panel.h"
#include "panels/toolbar_panel.h"
#include "panels/viewport_panel.h"
#include "widgets/menu_bar.h"

#include "core/application.h"
#include "scene/editor_camera.h"
#include "scene/scene_renderer.h"

class EditorApplication : public Application {
public:
	EditorApplication(const ApplicationCreateInfo& info);
	~EditorApplication() = default;

protected:
	void _on_start() override;

	void _on_update(float dt) override;

	void _on_imgui_update(float dt) override;

	void _on_destroy() override;

private:
	void _setup_menubar();

	void _setup_toolbar();

	void _on_viewport_resize();

	// scene controls
	void _save_active_scene();

	void _save_active_scene_as();

	void _open_project();

	void _on_scene_save();

	// scene runtime
	void _set_scene_state(SceneState _state);

	void _on_scene_open();

	void _on_scene_play();

	void _on_scene_stop();

	void _on_scene_pause();

	void _on_scene_resume();

	void _on_scene_step();

	void _handle_shortcuts();

	// custom rendering beheaviours
	void _handle_entity_selection(Ref<FrameBuffer> frame_buffer);

	void _render_entity_bounds(Entity entity);

private:
	Ref<SceneRenderer> scene_renderer;
	Ref<EditorCamera> editor_camera;

	Ref<Scene> editor_scene = nullptr;
	SceneState state = SceneState::EDIT;

	// panels and widgets
	MenuBar menubar;
	ToolbarPanel toolbar;
	ViewportPanel viewport;
	HierarchyPanel hierarchy;
	InspectorPanel inspector;
	ContentBrowserPanel content_browser;
	ConsolePanel console;
	StatsPanel stats;

	friend class ViewportPanel;
};

#endif
