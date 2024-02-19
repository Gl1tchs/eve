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

			g_modify_info.set_modified();

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

		g_modify_info.set_modified();
	}

	ImGui::PopID();
}

InspectorPanel::InspectorPanel() {}

void InspectorPanel::_draw() {
	const auto scene = SceneManager::get_active();
	if (!scene || scene->get_selected_entities().empty()) {
		return;
	}

	// get last selected entity
	auto selected_entity = scene->get_selected_entities().back();
	if (!selected_entity) {
		return;
	}

	ImGui::PushID((uint32_t)selected_entity);

	// display entity header

	auto& id_comp = selected_entity.get_component<IdComponent>();

	const float padding = 5.0f;

	const uint32_t plus_button_width = ImGui::CalcTextSize(ICON_FA_PLUS).x;

	ImGui::PushItemWidth(ImGui::GetContentRegionMax().x - (plus_button_width + 3 * padding));

	if (ImGui::InputText("##tag", &id_comp.tag)) {
		g_modify_info.set_modified();
	}

	ImGui::PopItemWidth();

	ImGui::SameLine(
			ImGui::GetContentRegionMax().x - (plus_button_width + padding));

	if (ImGui::Button(ICON_FA_PLUS)) {
		ImGui::OpenPopup("AddComponentPopup");
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		display_add_component_entry<CameraComponent>(selected_entity, "Camera");
		display_add_component_entry<SpriteRenderer>(selected_entity,
				"Sprite Renderer");
		display_add_component_entry<TextRenderer>(selected_entity, "TextRenderer");
		display_add_component_entry<Rigidbody2D>(selected_entity, "Rigidbody2D");
		display_add_component_entry<BoxCollider2D>(selected_entity, "BoxCollider2D");
		display_add_component_entry<CircleCollider2D>(selected_entity, "CircleCollider2D");
		display_add_component_entry<PostProcessVolume>(selected_entity, "PostProcessVolume");

		ImGui::EndPopup();
	}

	// draw component controls

#define BEGIN_FIELD(name)                                           \
	{                                                               \
		ImGui::Columns(2, nullptr, false);                          \
		ImGui::SetColumnWidth(0, 75);                               \
		const float field_width = ImGui::GetColumnWidth(1) - 10.0f; \
		ImGui::TextUnformatted(name);                               \
		ImGui::NextColumn();                                        \
		ImGui::PushItemWidth(field_width);

#define END_FIELD()        \
	ImGui::PopItemWidth(); \
	ImGui::Columns();      \
	}

	draw_component<Transform>("Transform", selected_entity,
			[](Transform& transform) {
				BEGIN_FIELD("Position");
				{
					if (ImGui::DragFloat3("##PositionControl",
								glm::value_ptr(transform.local_position), 0.1f)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				BEGIN_FIELD("Rotation");
				{
					if (ImGui::DragFloat3("##RotationControl",
								glm::value_ptr(transform.local_rotation), 0.1f)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				BEGIN_FIELD("Scale");
				{
					if (ImGui::DragFloat3("##ScaleControl", glm::value_ptr(transform.local_scale),
								0.1f)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();
			});

	draw_component<CameraComponent>(
			"Camera", selected_entity,
			[](CameraComponent& camera_comp) {
				auto& camera = camera_comp.camera;

				BEGIN_FIELD("Zoom Level");
				{
					if (ImGui::DragFloat("##ZoomLevelControl", &camera.zoom_level)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				BEGIN_FIELD("Near Clip");
				{
					if (ImGui::DragFloat("##NearClipControl", &camera.near_clip)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				BEGIN_FIELD("Far Clip");
				{
					if (ImGui::DragFloat("##FarClipControl", &camera.far_clip)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				BEGIN_FIELD("Is Primary");
				{
					if (ImGui::Checkbox("##IsPrimaryControl", &camera_comp.is_primary)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				BEGIN_FIELD("Is Fixed");
				{
					if (ImGui::Checkbox("##IsFixedControl", &camera_comp.is_fixed_aspect_ratio)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				if (camera_comp.is_fixed_aspect_ratio) {
					BEGIN_FIELD("Aspect Ratio");
					{
						if (ImGui::DragFloat("##AspectRatioControl", &camera_comp.camera.aspect_ratio, 0.05f)) {
							g_modify_info.set_modified();
						}
					}
					END_FIELD();
				}
			});

	draw_component<SpriteRenderer>(
			"Sprite Renderer", selected_entity,
			[this](SpriteRenderer& sprite_comp) {
				Ref<Texture2D> texture =
						sprite_comp.texture != 0
						? asset_registry::get_asset<Texture2D>(sprite_comp.texture)
						: nullptr;

				if (!texture) {
					BEGIN_FIELD("Texture");
					{
						std::string placeholder = "Drag Texture Here";
						ImGui::InputText("##TextureControl", &placeholder, ImGuiInputTextFlags_ReadOnly);
					}
					END_FIELD();

					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload =
										ImGui::AcceptDragDropPayload("DND_PAYLOAD_TEXTURE")) {
							const AssetHandle handle = *(const AssetHandle*)payload->Data;
							if (asset_registry::is_asset_loaded(handle)) {
								sprite_comp.texture = handle;

								g_modify_info.set_modified();
							}
						}
						ImGui::EndDragDropTarget();
					}
				} else {
					BEGIN_FIELD("Texture");
					{
						ImGui::InputText("##TextureControl", &texture->path,
								ImGuiInputTextFlags_ReadOnly);

						if (ImGui::BeginDragDropTarget()) {
							if (const ImGuiPayload* payload =
											ImGui::AcceptDragDropPayload("DND_PAYLOAD_TEXTURE")) {
								const AssetHandle handle = *(const AssetHandle*)payload->Data;
								if (asset_registry::is_asset_loaded(handle)) {
									sprite_comp.texture = handle;

									g_modify_info.set_modified();
								}
							}
							ImGui::EndDragDropTarget();
						}

						if (ImGui::Button(ICON_FA_MINUS, ImVec2(field_width, 0))) {
							sprite_comp.texture = 0;

							g_modify_info.set_modified();
						}
					}
					END_FIELD();
				}

				BEGIN_FIELD("Color");
				{
					if (ImGui::ColorEdit4("##ColorControl", &sprite_comp.color.r)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				BEGIN_FIELD("Tiling");
				{
					if (ImGui::DragFloat2("##TilingControl", &sprite_comp.tex_tiling.x)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();
			});

	draw_component<TextRenderer>(
			"Text Renderer", selected_entity,
			[this](TextRenderer& text_comp) {
				BEGIN_FIELD("Text");
				{
					if (ImGui::InputTextMultiline("##TextControl", &text_comp.text)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				Ref<Font> font =
						text_comp.font != 0
						? asset_registry::get_asset<Font>(text_comp.font)
						: nullptr;

				if (!font) {
					BEGIN_FIELD("Font");
					{
						std::string placeholder = "Default Font";
						ImGui::InputText("##FontControl", &placeholder, ImGuiInputTextFlags_ReadOnly);
					}
					END_FIELD();

					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload =
										ImGui::AcceptDragDropPayload("DND_PAYLOAD_FONT")) {
							const AssetHandle handle = *(const AssetHandle*)payload->Data;
							if (asset_registry::is_asset_loaded(handle)) {
								text_comp.font = handle;

								g_modify_info.set_modified();
							}
						}
						ImGui::EndDragDropTarget();
					}
				} else {
					BEGIN_FIELD("Font");
					{
						ImGui::InputText("##FontControl", &font->path,
								ImGuiInputTextFlags_ReadOnly);

						if (ImGui::BeginDragDropTarget()) {
							if (const ImGuiPayload* payload =
											ImGui::AcceptDragDropPayload("DND_PAYLOAD_FONT")) {
								const AssetHandle handle = *(const AssetHandle*)payload->Data;
								if (asset_registry::is_asset_loaded(handle)) {
									text_comp.font = handle;

									g_modify_info.set_modified();
								}
							}
							ImGui::EndDragDropTarget();
						}

						if (ImGui::Button(ICON_FA_MINUS, ImVec2(field_width, 0))) {
							text_comp.font = 0;

							g_modify_info.set_modified();
						}
					}
					END_FIELD();
				}

				BEGIN_FIELD("Foreground Color");
				{
					if (ImGui::ColorEdit4("##ForegroundColorControl", &text_comp.fg_color.r)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				BEGIN_FIELD("Background Color");
				{
					if (ImGui::ColorEdit4("##BackgroundColorControl", &text_comp.bg_color.r)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				BEGIN_FIELD("Kerning");
				{
					if (ImGui::DragFloat("##KerningControl", &text_comp.kerning)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				BEGIN_FIELD("Line Spacing");
				{
					if (ImGui::DragFloat("##LineSpacingControl", &text_comp.line_spacing)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();

				BEGIN_FIELD("Screen Space");
				{
					if (ImGui::Checkbox("##ScreenSpaceControl", &text_comp.is_screen_space)) {
						g_modify_info.set_modified();
					}
				}
				END_FIELD();
			});

	draw_component<Rigidbody2D>("Rigidbody2D", selected_entity, [](Rigidbody2D& rb2d) {
		static const char* items[] = {
			"Static",
			"Dynamic",
			"Kinematic"
		};

		static const Rigidbody2D::BodyType body_types[] = {
			Rigidbody2D::BodyType::STATIC,
			Rigidbody2D::BodyType::DYNAMIC,
			Rigidbody2D::BodyType::KINEMATIC,
		};

		const char* current_item = items[static_cast<int>(rb2d.type)];

		BEGIN_FIELD("Body Type");
		{
			if (ImGui::BeginCombo("##Rb2DBodyType", current_item)) {
				for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
					bool is_selected = (current_item == items[n]);
					if (ImGui::Selectable(items[n], is_selected)) {
						current_item = items[n];
						rb2d.type = body_types[n];

						g_modify_info.set_modified();
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}
		END_FIELD();

		BEGIN_FIELD("Fixed Rotation");
		{
			if (ImGui::Checkbox("##Rb2DFixedRotation", &rb2d.fixed_rotation)) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();
	});

	draw_component<BoxCollider2D>("BoxCollider2D", selected_entity, [](BoxCollider2D& box_collider) {
		BEGIN_FIELD("Offset");
		{
			if (ImGui::DragFloat2("##BoxColliderOffset", glm::value_ptr(box_collider.offset))) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();

		BEGIN_FIELD("Size");
		{
			if (ImGui::DragFloat2("##BoxColliderSize", glm::value_ptr(box_collider.size))) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();

		BEGIN_FIELD("IsTrigger");
		{
			if (ImGui::Checkbox("##BoxColliderIsTrigger", &box_collider.is_trigger)) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();

		BEGIN_FIELD("Density");
		{
			if (ImGui::DragFloat("##BoxColliderDensity", &box_collider.density)) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();

		BEGIN_FIELD("Friction");
		{
			if (ImGui::DragFloat("##BoxColliderFriciton", &box_collider.friction)) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();

		BEGIN_FIELD("Restitution");
		{
			if (ImGui::DragFloat("##BoxColliderRestitution", &box_collider.restitution)) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();

		BEGIN_FIELD("Threshold");
		{
			if (ImGui::DragFloat("##BoxColliderRestitutionThreshold", &box_collider.restitution_threshold)) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();
	});

	draw_component<CircleCollider2D>("CircleCollider2D", selected_entity, [](CircleCollider2D& circle_collider) {
		BEGIN_FIELD("Offset");
		{
			if (ImGui::DragFloat2("##CircleColliderOffset", glm::value_ptr(circle_collider.offset))) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();

		BEGIN_FIELD("Size");
		{
			if (ImGui::DragFloat("##CircleColliderSize", &circle_collider.radius)) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();

		BEGIN_FIELD("IsTrigger");
		{
			if (ImGui::Checkbox("##BoxColliderIsTrigger", &circle_collider.is_trigger)) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();

		BEGIN_FIELD("Density");
		{
			if (ImGui::DragFloat("##CircleColliderDensity", &circle_collider.density)) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();

		BEGIN_FIELD("Friction");
		{
			if (ImGui::DragFloat("##CircleColliderFriciton", &circle_collider.friction)) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();

		BEGIN_FIELD("Restitution");
		{
			if (ImGui::DragFloat("##CircleColliderRestitution", &circle_collider.restitution)) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();

		BEGIN_FIELD("Threshold");
		{
			if (ImGui::DragFloat("##CircleColliderRestitutionThreshold", &circle_collider.restitution_threshold)) {
				g_modify_info.set_modified();
			}
		}
		END_FIELD();
	});

	draw_component<PostProcessVolume>("Post Processing Volume", selected_entity, [](PostProcessVolume& volume) {
		ImGui::Columns();
		if (ImGui::TreeNode("Gray Scale")) {
			BEGIN_FIELD("Enabled");
			{
				if (ImGui::Checkbox("##GrayScaleEnabled", &volume.gray_scale.enabled)) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Chromatic Aberration")) {
			BEGIN_FIELD("Enabled");
			{
				if (ImGui::Checkbox("##ChromaticAberrationEnabled", &volume.chromatic_aberration.enabled)) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			BEGIN_FIELD("Offset");
			{
				if (ImGui::DragFloat3("##ChromaticAberrationOffset", glm::value_ptr(volume.chromatic_aberration.offset), 0.001f, -1.0f, 1.0f)) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Blur")) {
			BEGIN_FIELD("Enabled");
			{
				if (ImGui::Checkbox("##BlurEnabled", &volume.blur.enabled)) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			BEGIN_FIELD("Size");
			{
				if (ImGui::DragScalar("##BlurSize", ImGuiDataType_U32, &volume.blur.size)) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			BEGIN_FIELD("Seperation");
			{
				if (ImGui::DragFloat("##BlurSeperation",
							&volume.blur.seperation, 0.1f, 1.0f, std::numeric_limits<float>::max())) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Sharpen")) {
			BEGIN_FIELD("Enabled");
			{
				if (ImGui::Checkbox("##SharpenEnabled", &volume.sharpen.enabled)) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			BEGIN_FIELD("Amount");
			{
				if (ImGui::DragFloat("##SharpenAmount", &volume.sharpen.amount, 0.01f)) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Vignette")) {
			BEGIN_FIELD("Enabled");
			{
				if (ImGui::Checkbox("##VignetteEnabled", &volume.vignette.enabled)) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			BEGIN_FIELD("Inner");
			{
				if (ImGui::DragFloat("##VignetteInner", &volume.vignette.inner, 0.01f)) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			BEGIN_FIELD("Outer");
			{
				if (ImGui::DragFloat("##VignetteOuter", &volume.vignette.outer, 0.01f)) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			BEGIN_FIELD("Strengh");
			{
				if (ImGui::DragFloat("##VignetteStrengh", &volume.vignette.strength, 0.01f)) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			BEGIN_FIELD("Curvature");
			{
				if (ImGui::DragFloat("##VignetteCurvature", &volume.vignette.curvature, 0.01f)) {
					g_modify_info.set_modified();
				}
			}
			END_FIELD();

			ImGui::TreePop();
		}
	});

	ImGui::PopID();
}
