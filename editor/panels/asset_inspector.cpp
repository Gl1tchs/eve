#include "panels/asset_inspector.h"

#include "asset/asset.h"
#include "renderer/texture.h"
#include "scene/scene_manager.h"

#include <imgui.h>

AssetInspectorPanel::AssetInspectorPanel() {}

void AssetInspectorPanel::set_selected(const fs::path& path) {
	asset_path = path;
}

static const char* filtering_modes[] = { "nearest", "linear" };

static const char* wrapping_modes[] = {
	"repeat",
	"mirrored_repeat",
	"clamp_to_edge",
	"clamp_to_border",
};

void AssetInspectorPanel::_draw() {
	if (asset_path.empty() || !fs::exists(asset_path)) {
		return;
	}

	Ref<Scene> scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	fs::path meta_path = asset_path;
	meta_path = meta_path.replace_extension(
			meta_path.extension().string() + ".meta");

	if (!fs::exists(meta_path)) {
		return;
	}

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

	const AssetHandle handle = json["uid"].get<AssetHandle>();
	const AssetType type = json["type"].get<AssetType>();

	switch (type) {
		case AssetType::TEXTURE: {
			// display the texture if loaded
			if (Ref<Texture2D> texture =
							scene->get_asset_registry().get_asset<Texture2D>(
									handle);
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

					ImGui::Text("%s, %dx%d",
							json["path"].get<std::string>().c_str(), tex_size.x,
							tex_size.y);

					ImGui::Separator();

					ImGui::Dummy(ImVec2(0, 10));

					ImGui::SetCursorPosX(padding);

					ImGui::Image(reinterpret_cast<ImTextureID>(
										 texture->get_renderer_id()),
							ImVec2(image_width, tex_width), ImVec2(0, 1),
							ImVec2(1, 0));

					ImGui::Dummy(ImVec2(0, 10));
				}
			}

			EVE_BEGIN_FIELD("Min Filter");
			{
				std::string selected_min_filter = json["metadata"]["min_filter"]
														  .get<std::string>()
														  .c_str();

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
			}
			EVE_END_FIELD();

			EVE_BEGIN_FIELD("Mag Filter");
			{
				std::string selected_mag_filter = json["metadata"]["mag_filter"]
														  .get<std::string>()
														  .c_str();

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
			}
			EVE_END_FIELD();

			EVE_BEGIN_FIELD("Wrap S");
			{
				std::string selected_wrap_s =
						json["metadata"]["wrap_s"].get<std::string>();

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
			}
			EVE_END_FIELD();

			EVE_BEGIN_FIELD("Wrap T");
			{
				std::string selected_wrap_t =
						json["metadata"]["wrap_t"].get<std::string>();

				if (ImGui::BeginCombo("##wrap_t", selected_wrap_t.c_str())) {
					for (int i = 0; i < IM_ARRAYSIZE(wrapping_modes); i++) {
						bool is_selected =
								(selected_wrap_t == wrapping_modes[i]);

						if (ImGui::Selectable(wrapping_modes[i], is_selected)) {
							selected_wrap_t = wrapping_modes[i];

							if (is_texture_wrapping_mode_valid(
										selected_wrap_t.c_str())) {
								json["metadata"]["wrap_t"] = selected_wrap_t;
							}
						}
					}
					ImGui::EndCombo();
				}
			}
			EVE_END_FIELD();

			EVE_BEGIN_FIELD("Mipmaps");
			{
				bool selected_mipmaps =
						json["metadata"]["generate_mipmaps"].get<bool>();

				if (ImGui::Checkbox("##generate_mipmaps", &selected_mipmaps)) {
					json["metadata"]["generate_mipmaps"] = selected_mipmaps;
				}
			}
			EVE_END_FIELD();

			if (ImGui::Button("Save", ImVec2(-1, 0))) {
				json_utils::write_file(meta_path, json);

				scene->get_asset_registry().load_asset(
						json["path"].get<std::string>(), type);
			}

			break;
		}
		default:
			break;
	}
}
