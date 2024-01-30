#include "panels/inspector_panel.h"

#include "asset/asset_registry.h"
#include "data/fonts/font_awesome.h"
#include "renderer/font.h"
#include "renderer/texture.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/transform.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

template <typename T>
inline static void display_add_component_entry(Entity& selected_entity,
		const std::string& component_name) {
	if (!selected_entity.has_component<T>()) {
		if (ImGui::MenuItem(component_name.c_str())) {
			selected_entity.add_component<T>();
			ImGui::CloseCurrentPopup();
		}
	}
}

template <typename T, typename UIFunction>
inline static void draw_component(const std::string& name, Entity entity,
		UIFunction ui_function) {
	if (!entity.has_component<T>()) {
		return;
	}

	constexpr ImGuiTreeNodeFlags tree_node_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowOverlap;

	const ImVec2 avail_region_before = ImGui::GetContentRegionAvail();

	ImGui::PushID(name.c_str());

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 2, 2 });

	const ImVec2 frame_padding = ImGui::GetStyle().FramePadding;
	const float line_height =
			ImGui::GetIO().FontDefault->FontSize + frame_padding.y * 2.0f;

	bool open = ImGui::TreeNodeEx("##ComponentTreeNode", tree_node_flags, "%s",
			name.c_str());

	ImGui::PopStyleVar();

	bool remove_component = false;

	{
		static const char* button_label = ICON_FA_COG;

		ImGui::SameLine(
				avail_region_before.x -
				(ImGui::CalcTextSize(button_label).x + 2 * frame_padding.x));

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
		ImGui::PushStyleColor(ImGuiCol_Button, 0);

		if (ImGui::Button(button_label, { line_height, line_height })) {
			ImGui::OpenPopup("ComponentSettingsPopup");
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		if (ImGui::BeginPopup("ComponentSettingsPopup")) {
			if (ImGui::MenuItem("Remove component")) {
				remove_component = true;
			}

			ImGui::EndPopup();
		}
	}

	if (open) {
		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 75);

		ui_function(entity.get_component<T>());
		ImGui::TreePop();

		ImGui::Columns();
	}

	if (remove_component) {
		entity.remove_component<T>();
	}

	ImGui::PopID();
}

InspectorPanel::InspectorPanel(Ref<HierarchyPanel> hierarchy) :
		hierarchy(hierarchy) {}

void InspectorPanel::_draw() {
	auto selected_entity = hierarchy->get_selected_entity();
	if (!selected_entity) {
		return;
	}

	ImGui::PushID((uint32_t)selected_entity);

	// display entity header

	auto& id_comp = selected_entity.get_component<IdComponent>();

	const float padding = 5.0f;

	const uint32_t plus_button_width = ImGui::CalcTextSize(ICON_FA_PLUS).x;

	ImGui::PushItemWidth(ImGui::GetContentRegionMax().x - (plus_button_width + 3 * padding));

	ImGui::InputText("##tag", &id_comp.tag);

	ImGui::PopItemWidth();

	ImGui::SameLine(
			ImGui::GetContentRegionMax().x - (plus_button_width + padding));

	if (ImGui::Button(ICON_FA_PLUS)) {
		ImGui::OpenPopup("AddComponentPopup");
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		display_add_component_entry<CameraComponent>(selected_entity, "Camera");
		display_add_component_entry<SpriteRendererComponent>(selected_entity,
				"Sprite Renderer");
		display_add_component_entry<TextRendererComponent>(selected_entity, "Model");

		ImGui::EndPopup();
	}

	// draw component controls

#define BEGIN_FIELD(name)                                           \
	{                                                               \
		const float field_width = ImGui::GetColumnWidth(1) - 10.0f; \
		ImGui::TextUnformatted(name);                               \
		ImGui::NextColumn();                                        \
		ImGui::PushItemWidth(field_width);

#define END_FIELD()        \
	ImGui::PopItemWidth(); \
	}

	draw_component<TransformComponent>("Transform", selected_entity,
			[](TransformComponent& transform) {
				BEGIN_FIELD("Position");
				{
					ImGui::DragFloat3("##PositionControl",
							glm::value_ptr(transform.local_position), 0.1f);
				}
				END_FIELD();

				ImGui::NextColumn();

				BEGIN_FIELD("Rotation");
				{
					ImGui::DragFloat3("##RotationControl",
							glm::value_ptr(transform.local_rotation), 0.1f);
				}
				END_FIELD();

				ImGui::NextColumn();

				BEGIN_FIELD("Scale");
				{
					ImGui::DragFloat3("##ScaleControl", glm::value_ptr(transform.local_scale),
							0.1f);
				}
				END_FIELD();
			});

	draw_component<CameraComponent>(
			"Camera", selected_entity,
			[](CameraComponent& camera_comp) {
				auto& camera = camera_comp.camera;

				BEGIN_FIELD("Zoom Level");
				{
					ImGui::DragFloat("##ZoomLevelControl", &camera.zoom_level);
				}
				END_FIELD();

				ImGui::NextColumn();

				BEGIN_FIELD("Near Clip");
				{
					ImGui::DragFloat("##NearClipControl", &camera.near_clip);
				}
				END_FIELD();

				ImGui::NextColumn();

				BEGIN_FIELD("Far Clip");
				{
					ImGui::DragFloat("##FarClipControl", &camera.far_clip);
				}
				END_FIELD();

				ImGui::NextColumn();

				BEGIN_FIELD("Is Primary");
				{
					ImGui::Checkbox("##IsPrimaryControl", &camera_comp.is_primary);
				}
				END_FIELD();

				ImGui::NextColumn();

				BEGIN_FIELD("Is Fixed");
				{
					ImGui::Checkbox("##IsFixedControl", &camera_comp.is_fixed_aspect_ratio);
				}
				END_FIELD();

				if (camera_comp.is_fixed_aspect_ratio) {
					ImGui::NextColumn();

					BEGIN_FIELD("Aspect Ratio");
					{
						ImGui::DragFloat("##AspectRatioControl", &camera_comp.camera.aspect_ratio, 0.05f);
					}
					END_FIELD();
				}
			});

	draw_component<SpriteRendererComponent>(
			"Sprite Renderer", selected_entity,
			[this](SpriteRendererComponent& sprite_comp) {
				Ref<Texture2D> texture =
						sprite_comp.texture != 0
						? AssetRegistry::get<Texture2D>(sprite_comp.texture)
						: nullptr;

				if (!texture) {
					BEGIN_FIELD("Texture");
					{
						std::string placeholder = "Drag Texture Here";
						ImGui::InputText("##TextureControl", &placeholder, ImGuiInputTextFlags_ReadOnly);
					}
					END_FIELD();
				} else {
					BEGIN_FIELD("Texture");
					{
						ImGui::InputText("##TextureControl", &texture->path,
								ImGuiInputTextFlags_ReadOnly);
						if (ImGui::Button(ICON_FA_MINUS, ImVec2(field_width, 0))) {
							sprite_comp.texture = 0;
						}
					}
					END_FIELD();
				}

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload =
									ImGui::AcceptDragDropPayload("DND_PAYLOAD_TEXTURE")) {
						const AssetHandle handle = *(const AssetHandle*)payload->Data;
						if (AssetRegistry::exists(handle)) {
							sprite_comp.texture = handle;
						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::NextColumn();

				BEGIN_FIELD("Color");
				{
					ImGui::ColorEdit4("##ColorControl", &sprite_comp.color.r);
				}
				END_FIELD();

				ImGui::NextColumn();

				BEGIN_FIELD("Tiling");
				{
					ImGui::DragFloat2("##TilingControl", &sprite_comp.tex_tiling.x);
				}
				END_FIELD();
			});

	draw_component<TextRendererComponent>(
			"Text Renderer", selected_entity,
			[this](TextRendererComponent& text_comp) {
				BEGIN_FIELD("Text");
				{
					ImGui::InputTextMultiline("##TextControl", &text_comp.text);
				}
				END_FIELD();

				ImGui::NextColumn();

				Ref<Font> font =
						text_comp.font != 0
						? AssetRegistry::get<Font>(text_comp.font)
						: nullptr;

				if (!font) {
					BEGIN_FIELD("Font");
					{
						std::string placeholder = "Default Font";
						ImGui::InputText("##FontControl", &placeholder, ImGuiInputTextFlags_ReadOnly);
					}
					END_FIELD();
				} else {
					BEGIN_FIELD("Font");
					{
						ImGui::InputText("##FontControl", &font->path,
								ImGuiInputTextFlags_ReadOnly);
						if (ImGui::Button(ICON_FA_MINUS, ImVec2(field_width, 0))) {
							text_comp.font = 0;
						}
					}
					END_FIELD();
				}

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload =
									ImGui::AcceptDragDropPayload("DND_PAYLOAD_FONT")) {
						const AssetHandle handle = *(const AssetHandle*)payload->Data;
						if (AssetRegistry::exists(handle)) {
							text_comp.font = handle;
						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::NextColumn();

				BEGIN_FIELD("Foreground Color");
				{
					ImGui::ColorEdit4("##ForegroundColorControl", &text_comp.fg_color.r);
				}
				END_FIELD();

				ImGui::NextColumn();

				BEGIN_FIELD("Background Color");
				{
					ImGui::ColorEdit4("##BackgroundColorControl", &text_comp.bg_color.r);
				}
				END_FIELD();

				ImGui::NextColumn();

				BEGIN_FIELD("Kerning");
				{
					ImGui::DragFloat("##KerningControl", &text_comp.kerning);
				}
				END_FIELD();

				ImGui::NextColumn();

				BEGIN_FIELD("Line Spacing");
				{
					ImGui::DragFloat("##LineSpacingControl", &text_comp.line_spacing);
				}
				END_FIELD();
			});

	ImGui::PopID();
}
