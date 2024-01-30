#include "panels/content_browser.h"

#include "asset/asset_registry.h"
#include "data/fonts/font_awesome.h"
#include "project/project.h"

#include <imgui.h>

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

	if (ImGui::TreeNodeEx("res://", ImGuiTreeNodeFlags_DefaultOpen)) {
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
}

void ContentBrowserPanel::_draw_file(const fs::path& path) {
	const std::string filename = path.filename().string();

	ImGui::PushID(filename.c_str());

	const bool is_selected = selected_idx == idx;

	if (fs::is_directory(path)) {
		// if path is an directory create a tree node
		if (ImGui::TreeNode(filename.c_str())) {
			// iterate throught elements
			for (const auto& subpath : fs::directory_iterator(path)) {
				_draw_file(subpath);
			}

			ImGui::TreePop();
		}
	} else {
		// otherwise draw the file
		const auto it = asset_paths.find(path);
		const bool is_asset =
				it != asset_paths.end();

		const std::string label = std::format("{}  {}", is_asset ? ICON_FA_CIRCLE : ICON_FA_CIRCLE_O, filename);
		if (ImGui::Selectable(label.c_str(), is_selected)) {
			selected_idx = is_selected ? -1 : idx;
		}

		// if an asset
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
	}

	ImGui::PopID();

	idx++;
}

void ContentBrowserPanel::_refresh_asset_tree() {
	const AssetRegistryMap& asset_registry = AssetRegistry::get_loaded_assets();
	for (const auto& [handle, asset] : asset_registry) {
		asset_paths[Project::get_asset_path(asset->path)] = { asset->handle,
			asset->get_type() };
	}
}
