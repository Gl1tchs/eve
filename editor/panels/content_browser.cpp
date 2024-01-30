#include "panels/content_browser.h"

#include "asset/asset_registry.h"
#include "core/json_utils.h"
#include "data/fonts/font_awesome.h"
#include "project/project.h"
#include "renderer/texture.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <FileWatch.hpp>

inline static void on_file_change(const fs::path& path_rel, const filewatch::Event change_type) {
	static fs::path s_old_path;

	const fs::path path = Project::get_asset_directory() / path_rel;
	const std::string relative_path = Project::get_relative_asset_path(path.string());

	const AssetType type = get_asset_type_from_extension(path.extension().string());

	// if filename changed apply it
	switch (change_type) {
		case filewatch::Event::added: {
			if (type == AssetType::NONE) {
				break;
			}

			AssetImportData import_data;
			import_data.path = relative_path;
			import_data.type = type;
			AssetRegistry::subscribe(import_data);
			break;
		}
		case filewatch::Event::renamed_old: {
			s_old_path = path;
			break;
		}
		case filewatch::Event::renamed_new: {
			AssetRegistry::on_asset_rename(s_old_path, path);
			break;
		}
		case filewatch::Event::modified: {
			break;
		}
		case filewatch::Event::removed: {
			if (type == AssetType::NONE || fs::exists(path)) {
				break;
			}

			// if the removed type is scene and the scene is still running
			// it would stay existing as long as we dont exit
			AssetRegistry::remove(relative_path);

			break;
		}
		default:
			break;
	}
}

ContentBrowserPanel::ContentBrowserPanel() {
	set_flags(ImGuiWindowFlags_HorizontalScrollbar);
}

ContentBrowserPanel::~ContentBrowserPanel() {
}

void ContentBrowserPanel::_draw() {
	if (!Project::get_active()) {
		return;
	}

	_refresh_asset_tree();

	const fs::path asset_directory = Project::get_asset_directory();
	if (asset_directory.empty()) {
		return;
	}

	static filewatch::FileWatch<std::string>
			watcher(asset_directory.string(), on_file_change);

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
	const std::string filename = path.filename().string();

	ImGui::PushID(idx);

	const bool is_selected = selected_idx == idx;
	const bool is_renaming = renaming_idx == idx;

	if (!fs::is_directory(path)) {
		// is file an asset or not
		const auto it = asset_paths.find(path);
		const bool is_asset =
				it != asset_paths.end();

		const std::string label = std::format("{}  {}", is_asset ? ICON_FA_CIRCLE : ICON_FA_CIRCLE_O, filename);
		if (!is_renaming) {
			if (ImGui::Selectable(label.c_str(), is_selected)) {
				selected_idx = is_selected ? -1 : idx;
			}

			_draw_popup_context(path);
		} else {
			_draw_rename_file_dialog(path);
		}

		if (is_asset) {
			if (ImGui::BeginDragDropSource()) {
				const AssetData& data = it->second;

				const std::string payload_name =
						"DND_PAYLOAD_" + serialize_asset_type(data.type);

				ImGui::SetDragDropPayload(payload_name.c_str(), &data.handle,
						sizeof(AssetHandle));

				ImGui::SetTooltip("%s", serialize_asset_type(data.type));

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

void ContentBrowserPanel::_refresh_asset_tree() {
	asset_paths.clear();

	// TODO optimize this
	const AssetRegistryMap& asset_registry = AssetRegistry::get_assets();
	for (const auto& [handle, asset] : asset_registry) {
		asset_paths[Project::get_asset_path(asset.path)] = {
			AssetRegistry::get_handle_from_path(asset.path),
			asset.type
		};
	}
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
		if (!_is_asset(path) && ImGui::MenuItem("Import")) {
			AssetImportData import_data;
			import_data.path = Project::get_relative_asset_path(path.string());
			import_data.type = get_asset_type_from_extension(path.extension().string());

			AssetRegistry::subscribe(import_data);
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

bool ContentBrowserPanel::_is_asset(const fs::path& path) const {
	return asset_paths.find(path) != asset_paths.end();
}
