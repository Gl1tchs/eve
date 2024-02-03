#include "panels/toolbar_panel.h"

#include "data/fonts/font_awesome.h"

#include <imgui.h>

ToolbarPanel::ToolbarPanel() :
		Panel(true) {
	set_flags(ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
}

void ToolbarPanel::set_state(SceneState _state) {
	state = _state;
}

void ToolbarPanel::_draw() {
	switch (state) {
		case SceneState::EDIT: {
			if (ImGui::Button(ICON_FA_PLAY) && on_play) {
				on_play();
			}
			break;
		}
		case SceneState::PLAY: {
			if (ImGui::Button(ICON_FA_PAUSE) && on_pause) {
				on_pause();
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_STOP) && on_stop) {
				on_stop();
			}
			break;
		}
		case SceneState::PAUSED: {
			if (ImGui::Button(ICON_FA_PLAY) && on_resume) {
				on_resume();
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_STEP_FORWARD) && on_step) {
				on_step();
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_STOP) && on_stop) {
				on_stop();
			}
			break;
		}
	}
}
