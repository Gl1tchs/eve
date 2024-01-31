#include "panels/content_browser.h"

#include "asset/asset_registry.h"
#include "core/json_utils.h"
#include "data/fonts/font_awesome.h"
#include "project/project.h"
#include "renderer/texture.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

ContentBrowserPanel::ContentBrowserPanel() {
	set_flags(ImGuiWindowFlags_HorizontalScrollbar);
}

ContentBrowserPanel::~ContentBrowserPanel() {
}

void ContentBrowserPanel::_draw() {
	if (!Project::get_active()) {
		return;
	}

	const fs::path asset_directory = Project::get_asset_directory();
	if (asset_directory.empty()) {
		return;
	}

	if (ImGui::TreeNodeEx("res://", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen)) {
		idx = 0;
		for (const auto& subpath : fs::directory_iterator(asset_directory)) {
			_draw_file(subpath);
		}

		// Clicked somewhere else, reset selected_idx to -1
		if (is_focused() && !ImGui::IsItemHovered() &&
				ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			selected_idx = -1;
		}

		ImGui::TreePop();
	}

	ImGui::Dummy(
			ImVec2(ImGui::GetWindowWidth(), std::max(ImGui::GetContentRegionAvail().y, 100.0f)));

	// reset renaming index
	if (ImGui::IsItemClicked() || (renaming_idx != -1 && ImGui::IsKeyPressed(ImGuiKey_Escape))) {
		renaming_idx = -1;
	}
}

void ContentBrowserPanel::_draw_file(const fs::path& path) {
	// hide .meta files
	// TODO make this a filter and make user define other filters
	if (path.extension() == ".meta") {
		return;
	}

	const std::string filename = path.filename().string();

	ImGui::PushID(idx);

	const bool is_selected = selected_idx == idx;
	const bool is_renaming = renaming_idx == idx;

	// is file an asset or not
	if (!fs::is_directory(path)) {
		const AssetHandle handle = AssetRegistry::get_handle_from_path(path.string());
		const bool is_loaded = handle && AssetRegistry::is_loaded(handle);

		if (!is_renaming) {
			const std::string label = std::format("{1}  {0}", filename, (is_loaded ? ICON_FA_CIRCLE : ICON_FA_CIRCLE_O));
			if (ImGui::Selectable(label.c_str(), is_selected)) {
				selected_idx = is_selected ? -1 : idx;
			}

			_draw_popup_context(path);
		} else {
			_draw_rename_file_dialog(path);
		}

		if (is_loaded) {
			if (ImGui::BeginDragDropSource()) {
				const AssetType type = get_asset_type_from_extension(path.extension().string());

				const std::string payload_name =
						"DND_PAYLOAD_" + serialize_asset_type(type);

				ImGui::SetDragDropPayload(payload_name.c_str(), &handle,
						sizeof(AssetHandle));

				ImGui::SetTooltip("%s", serialize_asset_type(type));

				ImGui::EndDragDropSource();
			}
		}
	} else {
		if (!is_renaming) {
			bool open = ImGui::TreeNodeEx(filename.c_str(),
					ImGuiTreeNodeFlags_OpenOnArrow | (is_selected ? ImGuiTreeNodeFlags_Selected : 0));

			_draw_popup_context(path);

			if (open) {
				// iterate throught elements
				for (const auto& subpath : fs::directory_iterator(path)) {
					idx++;
					_draw_file(subpath);
				}

				ImGui::TreePop();
			}

		} else {
			_draw_rename_file_dialog(path);
		}
	}

	// TODO another drag drop field to move files

	ImGui::PopID();

	idx++;
}

void ContentBrowserPanel::_draw_rename_file_dialog(const fs::path& path) {
	// focus keyboard here
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
			!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)) {
		ImGui::SetKeyboardFocusHere(0);
	}

	std::string new_name = path.filename().string();
	if (ImGui::InputText("##RenameInput", &new_name, ImGuiInputTextFlags_EnterReturnsTrue)) {
		fs::path new_path = path.parent_path() / new_name;

		// replace the extension so that they would be same
		if (!new_path.has_extension()) {
			new_path.replace_extension(path.extension());
		}

		// rename the file
		fs::rename(path, new_path);

		renaming_idx = -1;
	}
}

void ContentBrowserPanel::_draw_popup_context(const fs::path& path) {
	if (ImGui::BeginPopupContextItem()) {
		// import if not asset
		const AssetType type = get_asset_type_from_extension(path.extension().string());

		if (type != AssetType::NONE && ImGui::MenuItem("Load")) {
			AssetRegistry::load(
					Project::get_relative_asset_path(path.string()),
					type);
		}

		if (const AssetHandle handle = AssetRegistry::get_handle_from_path(path.string());
				AssetRegistry::is_loaded(handle) && ImGui::MenuItem("Unload")) {
			AssetRegistry::unload(handle);
		}

		if (ImGui::MenuItem("Rename")) {
			renaming_idx = idx;
		}

		if (ImGui::MenuItem("Delete")) {
			fs::remove_all(path);
		}

		ImGui::EndPopup();
	}
}

bool ContentBrowserPanel::_is_asset_file(const fs::path& path) {
	return get_asset_type_from_extension(path.extension().string()) != AssetType::NONE;
}
