#include "panels/viewport_panel.h"

#include "scene/scene_manager.h"

#include <imgui.h>

ViewportPanel::ViewportPanel() {
	set_flags(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
}

void ViewportPanel::set_render_texture_id(uint32_t renderer_id) {
	texture_id = renderer_id;
}

const glm::vec2& ViewportPanel::get_min_bounds() const {
	return viewport_min_bounds;
}

const glm::vec2& ViewportPanel::get_max_bounds() const {
	return viewport_max_bounds;
}

void ViewportPanel::_draw() {
	const auto viewport_offset = get_pos();

	viewport_min_bounds = { get_min_region().x + viewport_offset.x, get_min_region().y + viewport_offset.y };
	viewport_max_bounds = { get_max_region().x + viewport_offset.x, get_max_region().y + viewport_offset.y };

	ImGui::Image(reinterpret_cast<void*>(texture_id),
			ImVec2{ get_size().x, get_size().y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	// load scenes just by droping into viewport
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload =
						ImGui::AcceptDragDropPayload("DND_PAYLOAD_SCENE")) {
			const char* recv_path = static_cast<const char*>(payload->Data);

			SceneManager::load_scene(recv_path);
		}

		ImGui::EndDragDropTarget();
	}
}
