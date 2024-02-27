#include "panels/content_browser.h"

#include "core/json_utils.h"
#include "data/fonts/font_awesome.h"
#include "project/project.h"
#include "renderer/texture.h"
#include "scene/scene_manager.h"
#include "utils/platform_utils.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

ContentBrowserPanel::ContentBrowserPanel() {
	set_flags(ImGuiWindowFlags_HorizontalScrollbar);
}

ContentBrowserPanel::~ContentBrowserPanel() {}

// get unique new scene path
inline static std::string get_new_file_path(
		const char* file_name, const char* extension = "") {
	static uint32_t iteration = 0;

	std::string path = std::format("res://{}{}.{}", file_name,
			iteration > 0 ? std::to_string(iteration) : "", extension);

	const fs::path fs_path = Project::get_asset_path(path);
	if (fs::exists(fs_path)) {
		iteration++;
		return get_new_file_path(file_name, extension);
	}

	iteration++;

	return path;
}

void ContentBrowserPanel::_draw() {
	if (!Project::get_active()) {
		return;
	}

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
	// hide .meta files
	// TODO make this a filter and make user define other filters
	auto scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	if (path.extension() == ".meta") {
		return;
	}

	const std::string filename = path.filename().string();

	ImGui::PushID(idx);

	const bool is_selected = (selected_idx == idx);
	const bool is_renaming = (renaming_idx == idx);

	// is file an asset or not
	if (!fs::is_directory(path)) {
		const AssetHandle handle =
				scene->get_asset_registry().get_handle_from_path(path.string());

		const bool is_loaded =
				handle && scene->get_asset_registry().is_asset_loaded(handle);

		if (!is_renaming) {
			const std::string label = std::format("{1}  {0}", filename,
					(is_loaded ? ICON_FA_CIRCLE : ICON_FA_CIRCLE_O));
			if (ImGui::Selectable(label.c_str(), is_selected)) {
				selected_idx = idx;
			}

			_draw_asset_modal(scene, handle, path);

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
				ImGui::OpenPopup("AssetSettingsModal");
			}

			_draw_popup_context(path);
		} else {
			_draw_rename_file_dialog(path);
		}

		const AssetType type =
				get_asset_type_from_extension(path.extension().string());

		// make scenes importable just by dragging into viewport
		if (!is_loaded && type == AssetType::SCENE) {
			if (ImGui::BeginDragDropSource()) {
				const std::string path_str = path.string();

				ImGui::SetDragDropPayload("DND_PAYLOAD_SCENE", path_str.data(),
						path_str.size() + 1);

				ImGui::SetTooltip("%s", "SCENE");

				ImGui::EndDragDropSource();
			}
		} else if (is_loaded && type != AssetType::SCENE) {
			if (ImGui::BeginDragDropSource()) {
				const std::string payload_name =
						"DND_PAYLOAD_" + serialize_asset_type(type);

				ImGui::SetDragDropPayload(
						payload_name.c_str(), &handle, sizeof(AssetHandle));

				ImGui::SetTooltip("%s", serialize_asset_type(type).c_str());

				ImGui::EndDragDropSource();
			}
		}
	} else {
		if (!is_renaming) {
			bool open = ImGui::TreeNodeEx(filename.c_str(),
					ImGuiTreeNodeFlags_OpenOnArrow |
							(is_selected ? ImGuiTreeNodeFlags_Selected : 0));

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

	// TODO another drag drop field to move files

	ImGui::PopID();

	idx++;
}

void ContentBrowserPanel::_draw_asset_modal(
		Ref<Scene> scene, AssetHandle handle, const fs::path& path) {
	if (!handle) {
		return;
	}

	if (ImGui::BeginPopupModal("AssetSettingsModal", nullptr)) {
		fs::path meta_path = Project::get_asset_path(path);
		meta_path = meta_path.replace_extension(
				meta_path.extension().string() + ".meta");

		const auto cache_it = asset_cache_map.find(meta_path);
		if (cache_it == asset_cache_map.end()) {
			Json json{};
			if (!json_utils::read_file(meta_path, json)) {
				ImGui::EndPopup();
				return;
			}

			asset_cache_map[meta_path] = json;
		}

		Json& json = asset_cache_map.at(meta_path);

		const AssetType type = json["type"].get<AssetType>();

		switch (type) {
			case AssetType::TEXTURE: {
				// display the texture if loaded
				if (Ref<Texture2D> texture =
								scene->get_asset_registry()
										.get_asset<Texture2D>(handle);
						texture) {
					const glm::ivec2& tex_size = texture->get_size();
					if (tex_size.x != 0 || tex_size.y != 0) {
						const float texture_ratio =
								static_cast<float>(texture->get_size().x) /
								texture->get_size().y;
						const float tex_width = ImGui::GetWindowWidth() / 2.0f;
						const float image_width = tex_width * texture_ratio;

						float padding =
								(ImGui::GetWindowWidth() - image_width) / 2.0f;
						if (padding < 0) {
							padding = 0;
						}

						ImGui::SetCursorPosX(padding);

						ImGui::Image(reinterpret_cast<ImTextureID>(
											 texture->get_renderer_id()),
								ImVec2(image_width, tex_width), ImVec2(0, 1),
								ImVec2(1, 0));

						ImGui::Dummy(ImVec2(0, 10));
					}
				}

				ImGui::Columns(2, nullptr, false);

				static const char* filtering_modes[] = { "nearest", "linear" };

				static const char* wrapping_modes[] = {
					"repeat",
					"mirrored_repeat",
					"clamp_to_edge",
					"clamp_to_border",
				};

				std::string selected_min_filter = json["metadata"]["min_filter"]
														  .get<std::string>()
														  .c_str();

				std::string selected_mag_filter = json["metadata"]["mag_filter"]
														  .get<std::string>()
														  .c_str();

				ImGui::TextUnformatted("Min Filter");
				ImGui::NextColumn();
				if (ImGui::BeginCombo(
							"##min_filter", selected_min_filter.c_str())) {
					for (int i = 0; i < IM_ARRAYSIZE(filtering_modes); i++) {
						bool is_selected =
								(selected_min_filter == filtering_modes[i]);

						if (ImGui::Selectable(
									filtering_modes[i], is_selected)) {
							selected_min_filter = filtering_modes[i];

							if (is_texture_filtering_mode_valid(
										selected_min_filter.c_str())) {
								json["metadata"]["min_filter"] =
										selected_min_filter;
							}
						}

						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::NextColumn();

				ImGui::TextUnformatted("Mag Filter");
				ImGui::NextColumn();
				if (ImGui::BeginCombo(
							"##mag_filter", selected_mag_filter.c_str())) {
					for (int i = 0; i < IM_ARRAYSIZE(filtering_modes); i++) {
						bool is_selected =
								(selected_mag_filter == filtering_modes[i]);

						if (ImGui::Selectable(
									filtering_modes[i], is_selected)) {
							selected_mag_filter = filtering_modes[i];

							if (is_texture_filtering_mode_valid(
										selected_mag_filter.c_str())) {
								json["metadata"]["mag_filter"] =
										selected_mag_filter;
							}
						}

						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::NextColumn();

				// Texture Wrapping Modes
				std::string selected_wrap_s =
						json["metadata"]["wrap_s"].get<std::string>();

				std::string selected_wrap_t =
						json["metadata"]["wrap_t"].get<std::string>();

				ImGui::TextUnformatted("Wrap S");
				ImGui::NextColumn();
				if (ImGui::BeginCombo("##wrap_s", selected_wrap_s.c_str())) {
					for (int i = 0; i < IM_ARRAYSIZE(wrapping_modes); i++) {
						bool is_selected =
								(selected_wrap_s == wrapping_modes[i]);

						if (ImGui::Selectable(wrapping_modes[i], is_selected)) {
							selected_wrap_s = wrapping_modes[i];

							if (is_texture_wrapping_mode_valid(
										selected_wrap_s.c_str())) {
								json["metadata"]["wrap_s"] = selected_wrap_s;
							}
						}
					}
					ImGui::EndCombo();
				}
				ImGui::NextColumn();

				ImGui::TextUnformatted("Wrap T");
				ImGui::NextColumn();
				if (ImGui::BeginCombo("##wrap_t", selected_wrap_t.c_str())) {
					for (int i = 0; i < IM_ARRAYSIZE(wrapping_modes); i++) {
						bool is_selected =
								(selected_wrap_t == wrapping_modes[i]);

						if (ImGui::Selectable(wrapping_modes[i], is_selected)) {
							selected_wrap_t = wrapping_modes[i];

							if (is_texture_wrapping_mode_valid(
										selected_wrap_t.c_str())) {
								json["metadata"]["wrap_t"] = selected_wrap_s;
							}
						}
					}
					ImGui::EndCombo();
				}
				ImGui::NextColumn();

				bool selected_mipmaps =
						json["metadata"]["generate_mipmaps"].get<bool>();

				ImGui::TextUnformatted("Generate Mipmaps");
				ImGui::NextColumn();
				if (ImGui::Checkbox("##generate_mipmaps", &selected_mipmaps)) {
					json["metadata"]["generate_mipmaps"] = selected_mipmaps;
				}

				ImGui::Columns();

				break;
			}
			default:
				break;
		}

		if (ImGui::Button("Save", ImVec2(-1, 0))) {
			json_utils::write_file(meta_path, json);

			scene->get_asset_registry().load_asset(
					json["path"].get<std::string>(), type);

			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Close", ImVec2(-1, 0))) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
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
