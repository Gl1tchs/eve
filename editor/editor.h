#ifndef EDITOR_H
#define EDITOR_H

#include "panels/console_panel.h"
#include "panels/content_browser.h"
#include "panels/hierarchy_panel.h"
#include "panels/inspector_panel.h"
#include "panels/stats_panel.h"
#include "panels/viewport_panel.h"
#include "widgets/menu_bar.h"

#include "core/application.h"
#include "renderer/frame_buffer.h"
#include "scene/editor_camera.h"
#include "scene/scene_renderer.h"

#include "renderer/post_processor.h"

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

	void _on_viewport_resize();

	void _handle_entity_selection();

private:
	Ref<SceneRenderer> scene_renderer;
	Ref<EditorCamera> editor_camera;

	Ref<FrameBuffer> frame_buffer;

	//! TODO move this inside of the renderer
	Ref<PostProcessor> post_processor;
	//! TODO move this into its own panel or global setting
	uint16_t post_process_effects = 0;

	// panels and widgets
	MenuBar menubar;
	ContentBrowserPanel content_browser;
	ConsolePanel console;

	Scope<ViewportPanel> viewport;
	Scope<StatsPanel> stats;
	Ref<HierarchyPanel> hierarchy;
	Ref<InspectorPanel> inspector;
};

#endif
