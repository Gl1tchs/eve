#include "panels/content_browser.h"

#include "data/fonts/font_awesome.h"
#include "project/project.h"
#include "scene/scene_manager.h"
#include "utils/platform_utils.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

ContentBrowserPanel::ContentBrowserPanel() {
	set_flags(ImGuiWindowFlags_HorizontalScrollbar);
}

ContentBrowserPanel::~ContentBrowserPanel() {}

const fs::path& ContentBrowserPanel::get_selected() const {
	return selected_path;
}

// get unique new scene path
inline static std::string get_new_file_path(
		const char* file_name, std::string extension = "") {
	// TODO refactor this function
	static uint32_t iteration = 0;

	const std::string iter_string =
			iteration > 0 ? std::to_string(iteration) : "";

	const std::string path = [&]() -> std::string {
		if (!extension.empty()) {
			return std::format(
					"res://{}{}.{}", file_name, iter_string, extension);
		} else {
			return std::format("res://{}{}", file_name, iter_string);
		}
	}();

	const fs::path fs_path = Project::get_asset_path(path);
	if (fs::exists(fs_path)) {
		iteration++;
		return get_new_file_path(file_name, extension);
	}

	iteration++;

	return path;
}

void ContentBrowserPanel::_draw() {
	const fs::path asset_directory = Project::get_asset_directory();
	if (asset_directory.empty()) {
		return;
	}

	idx = 0;
	for (const auto& subpath : fs::directory_iterator(asset_directory)) {
		_draw_file(subpath);
	}

	// Clicked somewhere else, reset selected_idx to -1
	if (is_focused() && !ImGui::IsItemHovered() &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		selected_idx = -1;
	}

	ImGui::Dummy(ImVec2(ImGui::GetWindowWidth(),
			std::max(ImGui::GetContentRegionAvail().y, 100.0f)));

	if (ImGui::BeginPopupContextItem("ContentBrowserContextMenu")) {
		if (ImGui::BeginMenu("Create")) {
			if (ImGui::MenuItem("Scene")) {
				Scene::serialize(create_ref<Scene>(),
						get_new_file_path("new_scene", "escn"));
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("New File")) {
			const fs::path file_path = Project::get_asset_path(
					get_new_file_path("new_file", "txt"));
			std::ofstream file(file_path);
			file.close();
		}

		if (ImGui::MenuItem("New Folder")) {
			const std::string file_path = get_new_file_path("new_folder", "");
			fs::create_directories(Project::get_asset_path(file_path));
		}

		if (ImGui::MenuItem("Open In Explorer")) {
			open_folder_in_explorer(Project::get_asset_directory());
		}

		ImGui::EndPopup();
	}

	// reset renaming index
	if (ImGui::IsItemClicked() ||
			(renaming_idx != -1 && ImGui::IsKeyPressed(ImGuiKey_Escape))) {
		renaming_idx = -1;
	}
}

void ContentBrowserPanel::_draw_file(const fs::path& path) {
	auto scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	// hide .meta files
	// TODO make this a filter and make user define other filters
	if (path.extension() == ".meta") {
		return;
	}

	ImGui::PushID(idx);
	{
		// is file an asset or not
		if (!fs::is_directory(path)) {
			_draw_asset(scene, path);
		} else {
			_draw_folder(path);
		}

		// TODO another drag drop field to move files
	}
	ImGui::PopID();

	idx++;
}

void ContentBrowserPanel::_draw_asset(Ref<Scene> scene, const fs::path& path) {
	const AssetHandle handle =
			scene->get_asset_registry().get_handle_from_path(path.string());
	const AssetType type =
			get_asset_type_from_extension(path.extension().string());

	const bool is_loaded =
			handle && scene->get_asset_registry().is_asset_loaded(handle);

	if (renaming_idx != idx) {
		const std::string label =
				std::format("{1}  {0}", path.filename().string(),
						(is_loaded ? ICON_FA_CIRCLE : ICON_FA_CIRCLE_O));
		if (ImGui::Selectable(label.c_str(), selected_idx == idx)) {
			selected_idx = idx;
			selected_path = path;
		}

		_draw_popup_context(path);
	} else {
		_draw_rename_file_dialog(path);
	}

	switch (type) {
		case AssetType::SCENE: {
			if (is_loaded) {
				break;
			}

			// make scenes importable just by dragging into viewport
			if (ImGui::BeginDragDropSource()) {
				const std::string path_str = path.string();

				ImGui::SetDragDropPayload("DND_PAYLOAD_SCENE", path_str.data(),
						path_str.size() + 1);

				ImGui::SetTooltip("%s", "SCENE");

				ImGui::EndDragDropSource();
			}
		}
		default: {
			if (!is_loaded) {
				break;
			}

			if (ImGui::BeginDragDropSource()) {
				const std::string payload_name =
						"DND_PAYLOAD_" + serialize_asset_type(type);

				ImGui::SetDragDropPayload(
						payload_name.c_str(), &handle, sizeof(AssetHandle));

				ImGui::SetTooltip("%s", serialize_asset_type(type).c_str());

				ImGui::EndDragDropSource();
			}
		}
	}
}

void ContentBrowserPanel::_draw_folder(const fs::path& path) {
	if (renaming_idx != idx) {
		ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

		const std::string label = (fs::is_empty(path) ? ICON_FA_FOLDER_O "  "
													  : ICON_FA_FOLDER "  ") +
				path.filename().string();
		const bool open = ImGui::TreeNodeEx(label.c_str(),
				(selected_idx == idx ? ImGuiTreeNodeFlags_Selected : 0));

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			selected_idx = idx;
		}

		ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

		_draw_popup_context(path);

		if (open && fs::exists(path)) {
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

void ContentBrowserPanel::_draw_rename_file_dialog(const fs::path& path) {
	// focus keyboard here
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
			!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)) {
		ImGui::SetKeyboardFocusHere(0);
	}

	std::string new_name = path.filename().string();
	if (ImGui::InputText("##RenameInput", &new_name,
				ImGuiInputTextFlags_EnterReturnsTrue)) {
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
	auto scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	if (ImGui::BeginPopupContextItem()) {
		// import if not asset
		const AssetType type =
				get_asset_type_from_extension(path.extension().string());

		// do not let scenes to be imported here
		if ((type != AssetType::NONE && type != AssetType::SCENE) &&
				ImGui::MenuItem("Load")) {
			scene->get_asset_registry().load_asset(
					Project::get_relative_asset_path(path.string()), type);
		}

		if (const AssetHandle handle =
						scene->get_asset_registry().get_handle_from_path(
								path.string());
				scene->get_asset_registry().is_asset_loaded(handle) &&
				ImGui::MenuItem("Unload")) {
			scene->get_asset_registry().unload_asset(handle);
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
	return get_asset_type_from_extension(path.extension().string()) !=
			AssetType::NONE;
}
