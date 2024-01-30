#include "panels/panel.h"

#include <imgui.h>

inline static ImVec2 vec_to_imvec(const glm::vec2& lhs) {
	return ImVec2(lhs.x, lhs.y);
}

inline static glm::vec2 imvec_to_vec(const ImVec2& lhs) {
	return glm::vec2(lhs.x, lhs.y);
}

Panel::Panel(bool default_active, glm::vec2 size, glm::vec2 pos) :
		active(default_active), panel_size(size), panel_pos(pos) {}

void Panel::render() {
	if (!active) {
		return;
	}

	bool* p_active = nullptr;
	if (!statik) {
		p_active = &active;
	} else {
		ImGui::SetNextWindowPos(vec_to_imvec(panel_pos));
		ImGui::SetNextWindowPos(vec_to_imvec(panel_size));
	}

	ImGui::Begin(_get_name().c_str(), p_active, window_flags);

	focused = ImGui::IsWindowFocused();
	hovered = ImGui::IsWindowHovered();

	if (!statik) {
		panel_size = imvec_to_vec(ImGui::GetContentRegionAvail());
		panel_pos = imvec_to_vec(ImGui::GetWindowPos());
	}

	min_region = imvec_to_vec(ImGui::GetWindowContentRegionMin());
	max_region = imvec_to_vec(ImGui::GetWindowContentRegionMax());

	_draw();

	ImGui::End();
}

void Panel::set_flags(int flags) {
	window_flags = flags;
}

void Panel::set_static(bool value) { statik = value; }

bool Panel::is_active() const { return active; }

void Panel::set_active(bool _active) { active = _active; };

bool Panel::is_focused() const { return focused; }

bool Panel::is_hovered() const { return hovered; }

void Panel::set_pos(const glm::vec2& pos) { panel_pos = pos; }

const glm::vec2& Panel::get_pos() const { return panel_pos; }

void Panel::set_size(const glm::vec2& size) { panel_size = size; }

const glm::vec2& Panel::get_size() const { return panel_size; }

const glm::vec2& Panel::get_min_region() const {
	return min_region;
}

const glm::vec2& Panel::get_max_region() const {
	return max_region;
}
