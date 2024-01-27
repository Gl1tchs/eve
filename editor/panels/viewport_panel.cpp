#include "panels/viewport_panel.h"

#include <imgui.h>

ViewportPanel::ViewportPanel(Ref<FrameBuffer> frame_buffer) :
		Panel(true, frame_buffer->get_size()),
		frame_buffer(frame_buffer) {
	set_flags(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
}

glm::vec2 ViewportPanel::get_min_bounds() {
	return viewport_min_bounds;
}

glm::vec2 ViewportPanel::get_max_bounds() {
	return viewport_max_bounds;
}

void ViewportPanel::_draw() {
	const auto viewport_offset = get_pos();

	viewport_min_bounds = { get_min_region().x + viewport_offset.x, get_min_region().y + viewport_offset.y };
	viewport_max_bounds = { get_max_region().x + viewport_offset.x, get_max_region().y + viewport_offset.y };

	uint64_t texture_id = frame_buffer->get_color_attachment_renderer_id(0);
	ImGui::Image(reinterpret_cast<void*>(texture_id),
			ImVec2{ get_size().x, get_size().y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
}
