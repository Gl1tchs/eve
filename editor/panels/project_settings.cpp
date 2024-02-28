#include "panels/project_settings.h"

#include "core/application.h"
#include "project/project.h"
#include "scripting/script_engine.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

ProjectSettingsPanel::ProjectSettingsPanel() : Panel(false) {}

void ProjectSettingsPanel::_draw() {
	// Sidebar
	ImGui::BeginChild("Sidebar", ImVec2(150, 0), true);

	if (ImGui::Selectable("General",
				selected_section == ProjectSettingSection::GENERAL)) {
		selected_section = ProjectSettingSection::GENERAL;
	}
	if (ImGui::Selectable(
				"Input", selected_section == ProjectSettingSection::INPUT)) {
		selected_section = ProjectSettingSection::INPUT;
	}
	if (ImGui::Selectable("Physics",
				selected_section == ProjectSettingSection::PHYSICS)) {
		selected_section = ProjectSettingSection::PHYSICS;
	}
	if (ImGui::Selectable("Scripting",
				selected_section == ProjectSettingSection::SCRIPTING)) {
		selected_section = ProjectSettingSection::SCRIPTING;
	}
	if (ImGui::Selectable("Shipping",
				selected_section == ProjectSettingSection::SHIPPING)) {
		selected_section = ProjectSettingSection::SHIPPING;
	}

	ImGui::EndChild();

	// Main content
	ImGui::SameLine();

	ImGui::BeginChild("Content", ImVec2(0, 0), true);

	switch (selected_section) {
		case ProjectSettingSection::GENERAL:
			_draw_general_settings();
			break;
		case ProjectSettingSection::INPUT:
			_draw_input_settings();
			break;
		case ProjectSettingSection::PHYSICS:
			_draw_physics_settings();
			break;
		case ProjectSettingSection::SCRIPTING:
			_draw_physics_settings();
			break;
		case ProjectSettingSection::SHIPPING:
			_draw_shipping_settings();
			break;
	}

	ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x,
			ImGui::GetContentRegionAvail().y - 30));

	if (ImGui::Button("Save", ImVec2(-1, 0))) {
		Project::save_active(Project::get_project_path());

		Application::get_instance()->get_window()->set_title(
				Project::get_name());
	}

	ImGui::EndChild();
};

void ProjectSettingsPanel::_draw_general_settings() {
	ImGui::SeparatorText("General Settings");

	Ref<Project> project = Project::get_active();

	auto& config = project->config;

	EVE_BEGIN_FIELD("Name");
	{ ImGui::InputText("##Name", &config.name); }
	EVE_END_FIELD();

	EVE_BEGIN_FIELD("Asset Directory");
	{ ImGui::InputText("##AssetDirectory", &config.asset_directory); }
	EVE_END_FIELD();

	EVE_BEGIN_FIELD("Script DLL");
	{ ImGui::InputText("##ScriptDLL", &config.script_dll); }
	EVE_END_FIELD();

	EVE_BEGIN_FIELD("Starting Scene");
	{ ImGui::InputText("##StartingScene", &config.starting_scene); }
	EVE_END_FIELD();
}

void ProjectSettingsPanel::_draw_input_settings() {
	ImGui::SeparatorText("Input Settings");
}

void ProjectSettingsPanel::_draw_physics_settings() {
	ImGui::SeparatorText("Physics Settings");
}

void ProjectSettingsPanel::_draw_scripting_settings() {
	ImGui::SeparatorText("Scripting Settings");

	EVE_BEGIN_FIELD("Name");
	{
		if (ImGui::Button("Reload Assembly", ImVec2(-1, 0))) {
			ScriptEngine::reload_assembly();
		}
	}
	EVE_END_FIELD();

	EVE_BEGIN_FIELD("Name");
	{
		if (ImGui::Button("Reinit", ImVec2(-1, 0))) {
			ScriptEngine::reinit();
		}
	}
	EVE_END_FIELD();
}

void ProjectSettingsPanel::_draw_shipping_settings() {
	ImGui::SeparatorText("Shipping Settings");
}
