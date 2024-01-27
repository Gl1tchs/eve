#include "editor.h"

#include "asset/asset.h"
#include "asset/asset_loader.h"
#include "asset/asset_registry.h"
#include "core/color.h"
#include "core/entrypoint.h"
#include "core/input.h"
#include "core/log.h"
#include "project/project.h"
#include "renderer/frame_buffer.h"
#include "renderer/renderer.h"
#include "renderer/renderer_api.h"
#include "renderer/texture.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/scene_manager.h"
#include "scene/scene_renderer.h"
#include "scene/transform.h"

#include "widgets/dock_space.h"

#include <imgui.h>

EditorApplication::EditorApplication(const ApplicationCreateInfo& info) :
		Application(info) {
	editor_camera = create_ref<EditorCamera>();
	scene_renderer = create_ref<SceneRenderer>(renderer);

	FrameBufferCreateInfo fb_info;
	fb_info.attachments = {
		FrameBufferTextureFormat::RGBA8,
		FrameBufferTextureFormat::RED_INT,
		FrameBufferTextureFormat::DEPTH24_STENCIL8,
	};
	fb_info.width = 1280;
	fb_info.height = 768;
	frame_buffer = create_ref<FrameBuffer>(fb_info);
}

void EditorApplication::_on_start() {
	Project::load("sample/sample.eve");
	SceneManager::set_active(Project::get_starting_scene_handle());
}

void EditorApplication::_on_update(float dt) {
	static bool pressed = false;
	if (!pressed && Input::is_key_pressed(KeyCode::SPACE)) {
		SceneManager::set_active(2173008177704578963);
		pressed = true;
	}

	frame_buffer->resize(viewport_width, viewport_height);
	editor_camera->aspect_ratio = (float)viewport_width / (float)viewport_height;
	editor_camera->update(dt);
	scene_renderer->on_viewport_resize(window->get_size());

	frame_buffer->bind();
	RendererAPI::set_clear_color(COLOR_GRAY);
	RendererAPI::clear(BUFFER_BITS_COLOR | BUFFER_BITS_DEPTH);
	{
		int data = -1;
		frame_buffer->clear_attachment(1, &data);

		scene_renderer->render_editor(dt, editor_camera);

		{
			auto [mx, my] = ImGui::GetMousePos();
			mx -= viewport_bounds[0].x;
			my -= viewport_bounds[0].y;
			glm::vec2 viewport_size = viewport_bounds[1] - viewport_bounds[0];
			my = viewport_size.y - my;
			int mouse_x = (int)mx;
			int mouse_y = (int)my;

			if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < (int)viewport_size.x && mouse_y < (int)viewport_size.y) {
				int pixel_data;
				frame_buffer->read_pixel(1, mouse_x, mouse_y, FrameBufferTextureFormat::RED_INT, &pixel_data);
				Entity hovered_entity = (pixel_data == -1) ? INVALID_ENTITY
														   : Entity((entt::entity)pixel_data, SceneManager::get_active().get());
				if (hovered_entity && Input::is_mouse_pressed(MouseCode::LEFT)) {
					EVE_LOG_ENGINE_INFO("Hovered entity: {}", hovered_entity.get_name());
				}
			}
		}
	}
	frame_buffer->unbind();
}

void EditorApplication::_on_imgui_update(float dt) {
	DockSpace::begin();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	const auto window_size = ImGui::GetWindowSize();
	viewport_width = window_size.x;
	viewport_height = window_size.y;

	auto viewport_min_region = ImGui::GetWindowContentRegionMin();
	auto viewport_max_region = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();
	viewport_bounds[0] = { viewport_min_region.x + viewportOffset.x, viewport_min_region.y + viewportOffset.y };
	viewport_bounds[1] = { viewport_max_region.x + viewportOffset.x, viewport_max_region.y + viewportOffset.y };

	ImGui::Image(reinterpret_cast<void*>(frame_buffer->get_color_attachment_renderer_id(0)),
			window_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	ImGui::End();
	ImGui::PopStyleVar();

	DockSpace::end();
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
