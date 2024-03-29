#include "panels/inspector_panel.h"

#include "asset/asset_registry.h"
#include "data/fonts/font_awesome.h"
#include "renderer/font.h"
#include "renderer/texture.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/scene_manager.h"
#include "scene/transform.h"
#include "scripting/script_engine.h"
#include "utils/modify_info.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

static void draw_script_field(const std::string& name,
		ScriptFieldInstance& script_field, bool use_default = false);

static void draw_script_field_runtime(const std::string& name,
		const ScriptField& field, Ref<ScriptInstance>& script_instance);

template <typename T>
inline static void display_add_component_entry(
		Entity& selected_entity, const std::string& component_name) {
	if (!selected_entity.has_component<T>()) {
		if (ImGui::MenuItem(component_name.c_str())) {
			selected_entity.add_component<T>();

			g_modify_info.set_modified();

			ImGui::CloseCurrentPopup();
		}
	}
}

template <typename T, typename UIFunction>
inline static void draw_component(
		const std::string& name, Entity entity, UIFunction ui_function) {
	if (!entity.has_component<T>()) {
		return;
	}

	constexpr ImGuiTreeNodeFlags tree_node_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowOverlap;

	const ImVec2 avail_region_before = ImGui::GetContentRegionAvail();

	ImGui::PushID(name.c_str());

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 2, 2 });

	const ImVec2 frame_padding = ImGui::GetStyle().FramePadding;
	const float line_height =
			ImGui::GetIO().FontDefault->FontSize + frame_padding.y * 2.0f;

	bool open = ImGui::TreeNodeEx(
			"##ComponentTreeNode", tree_node_flags, "%s", name.c_str());

	ImGui::PopStyleVar();

	bool remove_component = false;

	{
		static const char* button_label = ICON_FA_COG;

		ImGui::SameLine(avail_region_before.x -
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
	auto scene = SceneManager::get_active();
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

	ImGui::PushItemWidth(
			ImGui::GetContentRegionMax().x - (plus_button_width + 3 * padding));

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
		display_add_component_entry<SpriteRenderer>(
				selected_entity, "Sprite Renderer");
		display_add_component_entry<TextRenderer>(
				selected_entity, "TextRenderer");
		display_add_component_entry<Rigidbody2D>(
				selected_entity, "Rigidbody2D");
		display_add_component_entry<BoxCollider2D>(
				selected_entity, "BoxCollider2D");
		display_add_component_entry<CircleCollider2D>(
				selected_entity, "CircleCollider2D");
		display_add_component_entry<PostProcessVolume>(
				selected_entity, "PostProcessVolume");
		display_add_component_entry<ScriptComponent>(selected_entity, "Script");

		ImGui::EndPopup();
	}

	// draw component controls

	draw_component<Transform>(
			"Transform", selected_entity, [](Transform& transform) {
				EVE_BEGIN_FIELD("Position");
				{
					if (ImGui::DragFloat3("##PositionControl",
								glm::value_ptr(transform.local_position),
								0.1f)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Rotation");
				{
					if (ImGui::DragFloat3("##RotationControl",
								glm::value_ptr(transform.local_rotation),
								0.1f)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Scale");
				{
					if (ImGui::DragFloat3("##ScaleControl",
								glm::value_ptr(transform.local_scale), 0.1f)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();
			});

	draw_component<CameraComponent>(
			"Camera", selected_entity, [](CameraComponent& camera_comp) {
				auto& camera = camera_comp.camera;

				EVE_BEGIN_FIELD("Zoom Level");
				{
					if (ImGui::DragFloat(
								"##ZoomLevelControl", &camera.zoom_level)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Near Clip");
				{
					if (ImGui::DragFloat(
								"##NearClipControl", &camera.near_clip)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Far Clip");
				{
					if (ImGui::DragFloat(
								"##FarClipControl", &camera.far_clip)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Is Primary");
				{
					if (ImGui::Checkbox("##IsPrimaryControl",
								&camera_comp.is_primary)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Is Fixed");
				{
					if (ImGui::Checkbox("##IsFixedControl",
								&camera_comp.is_fixed_aspect_ratio)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				if (camera_comp.is_fixed_aspect_ratio) {
					EVE_BEGIN_FIELD("Aspect Ratio");
					{
						if (ImGui::DragFloat("##AspectRatioControl",
									&camera_comp.camera.aspect_ratio, 0.05f)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();
				}
			});

	draw_component<SpriteRenderer>("Sprite Renderer", selected_entity,
			[&](SpriteRenderer& sprite_comp) {
				Ref<Texture2D> texture = sprite_comp.texture != 0
						? scene->get_asset_registry().get_asset<Texture2D>(
								  sprite_comp.texture)
						: nullptr;

				if (!texture) {
					EVE_BEGIN_FIELD("Texture");
					{
						std::string placeholder = "Drag Texture Here";
						ImGui::InputText("##TextureControl", &placeholder,
								ImGuiInputTextFlags_ReadOnly);
					}
					EVE_END_FIELD();

					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload =
										ImGui::AcceptDragDropPayload(
												"DND_PAYLOAD_TEXTURE")) {
							const AssetHandle handle =
									*(const AssetHandle*)payload->Data;
							if (scene->get_asset_registry().is_asset_loaded(
										handle)) {
								sprite_comp.texture = handle;

								g_modify_info.set_modified();
							}
						}
						ImGui::EndDragDropTarget();
					}
				} else {
					EVE_BEGIN_FIELD("Texture");
					{
						ImGui::InputText("##TextureControl", &texture->path,
								ImGuiInputTextFlags_ReadOnly);

						if (ImGui::BeginDragDropTarget()) {
							if (const ImGuiPayload* payload =
											ImGui::AcceptDragDropPayload(
													"DND_PAYLOAD_TEXTURE")) {
								const AssetHandle handle =
										*(const AssetHandle*)payload->Data;
								if (scene->get_asset_registry().is_asset_loaded(
											handle)) {
									sprite_comp.texture = handle;

									g_modify_info.set_modified();
								}
							}
							ImGui::EndDragDropTarget();
						}

						if (ImGui::Button(
									ICON_FA_MINUS, ImVec2(field_width, 0))) {
							sprite_comp.texture = 0;

							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();
				}

				EVE_BEGIN_FIELD("Color");
				{
					if (ImGui::ColorEdit4(
								"##ColorControl", &sprite_comp.color.r)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Tiling");
				{
					if (ImGui::DragFloat2(
								"##TilingControl", &sprite_comp.tex_tiling.x)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Is Atlas");
				{
					if (ImGui::Checkbox(
								"##SpriteIsAtlas", &sprite_comp.is_atlas)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				if (sprite_comp.is_atlas) {
					EVE_BEGIN_FIELD("Block Size");
					{
						if (ImGui::InputFloat2("##SpriteBlockSize",
									glm::value_ptr(sprite_comp.block_size))) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					EVE_BEGIN_FIELD("Index");
					{
						if (ImGui::InputScalar("##SpriteIndex",
									ImGuiDataType_U32, &sprite_comp.index)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();
				}
			});

	draw_component<TextRenderer>(
			"Text Renderer", selected_entity, [&](TextRenderer& text_comp) {
				EVE_BEGIN_FIELD("Text");
				{
					if (ImGui::InputTextMultiline(
								"##TextControl", &text_comp.text)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				Ref<Font> font = text_comp.font != 0
						? scene->get_asset_registry().get_asset<Font>(
								  text_comp.font)
						: nullptr;

				if (!font) {
					EVE_BEGIN_FIELD("Font");
					{
						std::string placeholder = "Default Font";
						ImGui::InputText("##FontControl", &placeholder,
								ImGuiInputTextFlags_ReadOnly);
					}
					EVE_END_FIELD();

					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload =
										ImGui::AcceptDragDropPayload(
												"DND_PAYLOAD_FONT")) {
							const AssetHandle handle =
									*(const AssetHandle*)payload->Data;
							if (scene->get_asset_registry().is_asset_loaded(
										handle)) {
								text_comp.font = handle;

								g_modify_info.set_modified();
							}
						}
						ImGui::EndDragDropTarget();
					}
				} else {
					EVE_BEGIN_FIELD("Font");
					{
						ImGui::InputText("##FontControl", &font->path,
								ImGuiInputTextFlags_ReadOnly);

						if (ImGui::BeginDragDropTarget()) {
							if (const ImGuiPayload* payload =
											ImGui::AcceptDragDropPayload(
													"DND_PAYLOAD_FONT")) {
								const AssetHandle handle =
										*(const AssetHandle*)payload->Data;
								if (scene->get_asset_registry().is_asset_loaded(
											handle)) {
									text_comp.font = handle;

									g_modify_info.set_modified();
								}
							}
							ImGui::EndDragDropTarget();
						}

						if (ImGui::Button(
									ICON_FA_MINUS, ImVec2(field_width, 0))) {
							text_comp.font = 0;

							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();
				}

				EVE_BEGIN_FIELD("Foreground Color");
				{
					if (ImGui::ColorEdit4("##ForegroundColorControl",
								&text_comp.fg_color.r)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Background Color");
				{
					if (ImGui::ColorEdit4("##BackgroundColorControl",
								&text_comp.bg_color.r)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Kerning");
				{
					if (ImGui::DragFloat(
								"##KerningControl", &text_comp.kerning)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Line Spacing");
				{
					if (ImGui::DragFloat("##LineSpacingControl",
								&text_comp.line_spacing)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Screen Space");
				{
					if (ImGui::Checkbox("##ScreenSpaceControl",
								&text_comp.is_screen_space)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();
			});

	draw_component<
			Rigidbody2D>("Rigidbody2D", selected_entity, [](Rigidbody2D& rb2d) {
		static const char* items[] = { "Static", "Dynamic", "Kinematic" };

		static const Rigidbody2D::BodyType body_types[] = {
			Rigidbody2D::BodyType::STATIC,
			Rigidbody2D::BodyType::DYNAMIC,
			Rigidbody2D::BodyType::KINEMATIC,
		};

		const char* current_item = items[static_cast<int>(rb2d.type)];

		EVE_BEGIN_FIELD("Body Type");
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
		EVE_END_FIELD();

		EVE_BEGIN_FIELD("Fixed Rotation");
		{
			if (ImGui::Checkbox("##Rb2DFixedRotation", &rb2d.fixed_rotation)) {
				g_modify_info.set_modified();
			}
		}
		EVE_END_FIELD();
	});

	draw_component<BoxCollider2D>(
			"BoxCollider2D", selected_entity, [](BoxCollider2D& box_collider) {
				EVE_BEGIN_FIELD("Offset");
				{
					if (ImGui::DragFloat2("##BoxColliderOffset",
								glm::value_ptr(box_collider.offset))) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Size");
				{
					if (ImGui::DragFloat2("##BoxColliderSize",
								glm::value_ptr(box_collider.size))) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("IsTrigger");
				{
					if (ImGui::Checkbox("##BoxColliderIsTrigger",
								&box_collider.is_trigger)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Density");
				{
					if (ImGui::DragFloat("##BoxColliderDensity",
								&box_collider.density)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Friction");
				{
					if (ImGui::DragFloat("##BoxColliderFriciton",
								&box_collider.friction)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Restitution");
				{
					if (ImGui::DragFloat("##BoxColliderRestitution",
								&box_collider.restitution)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Threshold");
				{
					if (ImGui::DragFloat("##BoxColliderRestitutionThreshold",
								&box_collider.restitution_threshold)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();
			});

	draw_component<CircleCollider2D>("CircleCollider2D", selected_entity,
			[](CircleCollider2D& circle_collider) {
				EVE_BEGIN_FIELD("Offset");
				{
					if (ImGui::DragFloat2("##CircleColliderOffset",
								glm::value_ptr(circle_collider.offset))) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Size");
				{
					if (ImGui::DragFloat("##CircleColliderSize",
								&circle_collider.radius)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("IsTrigger");
				{
					if (ImGui::Checkbox("##BoxColliderIsTrigger",
								&circle_collider.is_trigger)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Density");
				{
					if (ImGui::DragFloat("##CircleColliderDensity",
								&circle_collider.density)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Friction");
				{
					if (ImGui::DragFloat("##CircleColliderFriciton",
								&circle_collider.friction)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Restitution");
				{
					if (ImGui::DragFloat("##CircleColliderRestitution",
								&circle_collider.restitution)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();

				EVE_BEGIN_FIELD("Threshold");
				{
					if (ImGui::DragFloat("##CircleColliderRestitutionThreshold",
								&circle_collider.restitution_threshold)) {
						g_modify_info.set_modified();
					}
				}
				EVE_END_FIELD();
			});

	draw_component<PostProcessVolume>("Post Processing Volume", selected_entity,
			[](PostProcessVolume& volume) {
				ImGui::Columns();
				if (ImGui::TreeNode("Gray Scale")) {
					EVE_BEGIN_FIELD("Enabled");
					{
						if (ImGui::Checkbox("##GrayScaleEnabled",
									&volume.gray_scale.enabled)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Chromatic Aberration")) {
					EVE_BEGIN_FIELD("Enabled");
					{
						if (ImGui::Checkbox("##ChromaticAberrationEnabled",
									&volume.chromatic_aberration.enabled)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					EVE_BEGIN_FIELD("Offset");
					{
						if (ImGui::DragFloat3("##ChromaticAberrationOffset",
									glm::value_ptr(
											volume.chromatic_aberration.offset),
									0.001f, -1.0f, 1.0f)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Blur")) {
					EVE_BEGIN_FIELD("Enabled");
					{
						if (ImGui::Checkbox(
									"##BlurEnabled", &volume.blur.enabled)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					EVE_BEGIN_FIELD("Size");
					{
						if (ImGui::DragScalar("##BlurSize", ImGuiDataType_U32,
									&volume.blur.size)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					EVE_BEGIN_FIELD("Seperation");
					{
						if (ImGui::DragFloat("##BlurSeperation",
									&volume.blur.seperation, 0.1f, 1.0f,
									std::numeric_limits<float>::max())) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Sharpen")) {
					EVE_BEGIN_FIELD("Enabled");
					{
						if (ImGui::Checkbox("##SharpenEnabled",
									&volume.sharpen.enabled)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					EVE_BEGIN_FIELD("Amount");
					{
						if (ImGui::DragFloat("##SharpenAmount",
									&volume.sharpen.amount, 0.01f)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Vignette")) {
					EVE_BEGIN_FIELD("Enabled");
					{
						if (ImGui::Checkbox("##VignetteEnabled",
									&volume.vignette.enabled)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					EVE_BEGIN_FIELD("Inner");
					{
						if (ImGui::DragFloat("##VignetteInner",
									&volume.vignette.inner, 0.01f)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					EVE_BEGIN_FIELD("Outer");
					{
						if (ImGui::DragFloat("##VignetteOuter",
									&volume.vignette.outer, 0.01f)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					EVE_BEGIN_FIELD("Strengh");
					{
						if (ImGui::DragFloat("##VignetteStrengh",
									&volume.vignette.strength, 0.01f)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					EVE_BEGIN_FIELD("Curvature");
					{
						if (ImGui::DragFloat("##VignetteCurvature",
									&volume.vignette.curvature, 0.01f)) {
							g_modify_info.set_modified();
						}
					}
					EVE_END_FIELD();

					ImGui::TreePop();
				}
			});

	draw_component<ScriptComponent>("Script", selected_entity,
			[selected_entity, scene](auto& component) {
				bool script_class_exists =
						ScriptEngine::does_entity_class_exists(
								component.class_name);

				EVE_BEGIN_FIELD("Class Name");
				{
					// if script class doesn't exists make the color red
					if (!script_class_exists) {
						ImGui::PushStyleColor(
								ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));
					}

					std::string class_name = component.class_name;
					if (ImGui::InputText("##ClassName", &class_name)) {
						component.class_name = class_name;

						g_modify_info.set_modified();
					}

					if (!script_class_exists) {
						ImGui::PopStyleColor();
					}
				}
				EVE_END_FIELD();

				if (!script_class_exists) {
					return;
				}

				// Fields
				if (ImGui::TreeNodeEx(
							"Script Fields", ImGuiTreeNodeFlags_DefaultOpen)) {
					bool is_scene_running = scene->is_running();
					if (is_scene_running) {
						Ref<ScriptInstance> script_instance =
								ScriptEngine::get_entity_script_instance(
										selected_entity.get_uid());
						if (script_instance) {
							const auto& fields =
									script_instance->get_script_class()
											->get_fields();
							for (const auto& [name, field] : fields) {
								draw_script_field_runtime(
										name, field, script_instance);
							}
						}
					} else if (!is_scene_running && script_class_exists) {
						Ref<ScriptClass> entity_class =
								ScriptEngine::get_entity_class(
										component.class_name);
						const auto& fields = entity_class->get_fields();

						auto& entity_fields =
								ScriptEngine::get_script_field_map(
										selected_entity);
						for (const auto& [name, field] : fields) {
							// Field has been set in editor
							if (entity_fields.find(name) !=
									entity_fields.end()) {
								ScriptFieldInstance& script_field =
										entity_fields.at(name);
								draw_script_field(name, script_field);
							} else {
								ScriptFieldInstance& script_field =
										entity_fields[name];
								script_field.field = field;

								draw_script_field(name, script_field, true);
							}
						}
					}

					ImGui::TreePop();
				}
			});

	ImGui::PopID();
}

void draw_script_field(const std::string& name,
		ScriptFieldInstance& script_field, bool use_default) {
	switch (script_field.field.type) {
		case ScriptFieldType::FLOAT: {
			float data = !use_default ? script_field.get_value<float>() : 0.0f;
			if (ImGui::DragFloat(name.c_str(), &data)) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::DOUBLE: {
			double data = !use_default ? script_field.get_value<double>() : 0.0;
			if (ImGui::DragFloat(name.c_str(), (float*)&data)) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::BOOL: {
			bool data = !use_default ? script_field.get_value<bool>() : false;
			if (ImGui::Checkbox(name.c_str(), &data)) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::CHAR: {
			break;
		}
		case ScriptFieldType::BYTE: {
			int8_t data = !use_default ? script_field.get_value<int8_t>() : 0;
			if (ImGui::DragInt(name.c_str(), (int*)&data,
						std::numeric_limits<int8_t>::min(),
						std::numeric_limits<int8_t>::max())) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::SHORT: {
			int16_t data = !use_default ? script_field.get_value<int16_t>() : 0;
			if (ImGui::DragInt(name.c_str(), (int*)&data, 1.0f,
						std::numeric_limits<int16_t>::min(),
						std::numeric_limits<int16_t>::max())) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::INT: {
			int data = !use_default ? script_field.get_value<int>() : 0;
			if (ImGui::DragInt(name.c_str(), &data, 1.0f,
						std::numeric_limits<int>::min(),
						std::numeric_limits<int>::max())) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::LONG: {
			int64_t data = !use_default ? script_field.get_value<int64_t>() : 0;
			if (ImGui::DragInt(name.c_str(), (int*)&data, 1.0f,
						std::numeric_limits<int32_t>::min(),
						std::numeric_limits<int32_t>::max())) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::UBYTE: {
			uint8_t data = !use_default ? script_field.get_value<uint8_t>() : 0;
			if (ImGui::DragInt(name.c_str(), (int*)&data,
						std::numeric_limits<int8_t>::min(),
						std::numeric_limits<uint8_t>::max())) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::USHORT: {
			uint16_t data =
					!use_default ? script_field.get_value<uint16_t>() : 0;
			if (ImGui::DragInt(name.c_str(), (int*)&data, 1.0f,
						std::numeric_limits<uint16_t>::min(),
						std::numeric_limits<uint16_t>::max())) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::UINT: {
			uint32_t data =
					!use_default ? script_field.get_value<uint32_t>() : 0;
			if (ImGui::DragInt(name.c_str(), (int*)&data, 1.0f,
						std::numeric_limits<uint32_t>::min(),
						std::numeric_limits<uint32_t>::max())) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::ULONG: {
			uint64_t data =
					!use_default ? script_field.get_value<uint64_t>() : 0;
			if (ImGui::DragInt(name.c_str(), (int*)&data, 1.0f,
						std::numeric_limits<uint32_t>::min(),
						std::numeric_limits<uint32_t>::max())) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::VECTOR2: {
			glm::vec2 data = !use_default ? script_field.get_value<glm::vec2>()
										  : glm::vec2(0.0f);
			if (ImGui::DragFloat2(name.c_str(), &data[0])) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::VECTOR3: {
			glm::vec3 data = !use_default ? script_field.get_value<glm::vec3>()
										  : glm::vec3(0.0f);
			if (ImGui::DragFloat3(name.c_str(), &data[0])) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::VECTOR4: {
			glm::vec4 data = !use_default ? script_field.get_value<glm::vec4>()
										  : glm::vec4(0.0f);
			if (ImGui::DragFloat4(name.c_str(), &data[0])) {
				script_field.set_value(data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::COLOR: {
			Color color = !use_default ? script_field.get_value<Color>()
									   : Color(0.0f);
			if (ImGui::ColorEdit4(name.c_str(), &color.r)) {
				script_field.set_value(color);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::ENTITY: {
			UID uuid = script_field.get_value<UID>();
			Entity entity = SceneManager::get_active()->find_by_id(uuid);

			{
				std::string name_string = (!use_default && entity)
						? entity.get_name()
						: "Invalid Entity";
				ImGui::InputText(name.c_str(), &name_string,
						ImGuiInputTextFlags_ReadOnly);
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(
							"DND_PAYLOAD_ENTITY")) {
					UID recv_id = *(const UID*)payload->Data;
					script_field.set_value(recv_id);

					g_modify_info.set_modified();
				}
				ImGui::EndDragDropTarget();
			}
			break;
		}
		case ScriptFieldType::NONE:
			break;
	}
}

void draw_script_field_runtime(const std::string& name,
		const ScriptField& field, Ref<ScriptInstance>& script_instance) {
	switch (field.type) {
		case ScriptFieldType::FLOAT: {
			float data = script_instance->get_field_value<float>(name);
			if (ImGui::DragFloat(name.c_str(), &data)) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::DOUBLE: {
			double data = script_instance->get_field_value<double>(name);
			if (ImGui::DragFloat(name.c_str(), (float*)&data)) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::BOOL: {
			bool data = script_instance->get_field_value<bool>(name);
			if (ImGui::Checkbox(name.c_str(), &data)) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::CHAR: {
			break;
		}
		case ScriptFieldType::BYTE: {
			int8_t data = script_instance->get_field_value<int8_t>(name);
			if (ImGui::DragInt(name.c_str(), (int*)&data,
						std::numeric_limits<int8_t>::min(),
						std::numeric_limits<int8_t>::max())) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::SHORT: {
			int16_t data = script_instance->get_field_value<int16_t>(name);
			if (ImGui::DragInt(name.c_str(), (int*)&data, 1.0f,
						std::numeric_limits<int16_t>::min(),
						std::numeric_limits<int16_t>::max())) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::INT: {
			int data = script_instance->get_field_value<int>(name);
			if (ImGui::DragInt(name.c_str(), &data, 1.0f,
						std::numeric_limits<int32_t>::min(),
						std::numeric_limits<int32_t>::max())) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::LONG: {
			int64_t data = script_instance->get_field_value<int64_t>(name);
			if (ImGui::DragInt(name.c_str(), (int*)&data, 1.0f,
						std::numeric_limits<int32_t>::min(),
						std::numeric_limits<int32_t>::max())) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::UBYTE: {
			uint8_t data = script_instance->get_field_value<uint8_t>(name);
			if (ImGui::DragInt(name.c_str(), (int*)&data,
						std::numeric_limits<int8_t>::min(),
						std::numeric_limits<uint8_t>::max())) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::USHORT: {
			uint16_t data = script_instance->get_field_value<uint16_t>(name);
			if (ImGui::DragInt(name.c_str(), (int*)&data, 1.0f,
						std::numeric_limits<uint16_t>::min(),
						std::numeric_limits<uint16_t>::max())) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::UINT: {
			uint32_t data = script_instance->get_field_value<uint32_t>(name);
			if (ImGui::DragInt(name.c_str(), (int*)&data, 1.0f,
						std::numeric_limits<uint32_t>::min(),
						std::numeric_limits<uint32_t>::max())) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::ULONG: {
			uint64_t data = script_instance->get_field_value<uint64_t>(name);
			if (ImGui::DragInt(name.c_str(), (int*)&data, 1.0f,
						std::numeric_limits<uint32_t>::min(),
						std::numeric_limits<uint32_t>::max())) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::VECTOR2: {
			glm::vec2 data = script_instance->get_field_value<glm::vec2>(name);
			if (ImGui::DragFloat2(name.c_str(), &data[0])) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::VECTOR3: {
			glm::vec3 data = script_instance->get_field_value<glm::vec3>(name);
			if (ImGui::DragFloat3(name.c_str(), &data[0])) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::VECTOR4: {
			glm::vec4 data = script_instance->get_field_value<glm::vec4>(name);
			if (ImGui::DragFloat4(name.c_str(), &data[0])) {
				script_instance->set_field_value(name, data);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::COLOR: {
			Color color = script_instance->get_field_value<Color>(name);
			if (ImGui::ColorEdit4(name.c_str(), &color.r)) {
				script_instance->set_field_value(name, color);

				g_modify_info.set_modified();
			}
			break;
		}
		case ScriptFieldType::ENTITY: {
			UID uuid = script_instance->get_field_value<UID>(name);
			Entity entity = SceneManager::get_active()->find_by_id(uuid);

			{
				std::string name_string =
						entity ? entity.get_name() : "Invalid Entity";
				ImGui::InputText(name.c_str(), &name_string,
						ImGuiInputTextFlags_ReadOnly);
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(
							"DND_PAYLOAD_ENTITY")) {
					UID recv_id = *(const UID*)payload->Data;
					script_instance->set_field_value(name, recv_id);

					g_modify_info.set_modified();
				}
				ImGui::EndDragDropTarget();
			}
			break;
		}
		case ScriptFieldType::NONE:
			break;
	}
}
