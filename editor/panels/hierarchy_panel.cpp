// Copyright (c) 2023 Berke Umut Biricik All Rights Reserved

#include "panels/hierarchy_panel.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "data/fonts/font_awesome.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "scene/scene_manager.h"

HierarchyPanel::HierarchyPanel() {
	set_flags(ImGuiWindowFlags_HorizontalScrollbar);
}

const std::vector<Entity> HierarchyPanel::get_selected_entities() {
	const auto scene = SceneManager::get_active();
	if (!scene) {
		return {};
	}

	return scene->get_selected_entities();
}

void HierarchyPanel::_draw() {
	auto scene = SceneManager::get_active();
	if (!scene) {
		return;
	}

	ImGui::TextUnformatted(scene->name.c_str());

	ImGui::SameLine(ImGui::GetContentRegionMax().x -
			(ImGui::CalcTextSize(ICON_FA_PLUS).x +
					2 * ImGui::GetStyle().FramePadding.x));

	if (ImGui::BeginPopup("HierarchyEntityPopup")) {
		if (ImGui::MenuItem("Empty")) {
			scene->select_entity(scene->create("Entity"));

			g_modify_info.set_modified();

			ImGui::CloseCurrentPopup();
		}

		if (ImGui::MenuItem("Sprite")) {
			Entity entity = scene->create("Entity");

			entity.add_component<SpriteRenderer>();

			scene->select_entity(entity);

			g_modify_info.set_modified();

			ImGui::CloseCurrentPopup();
		}

		if (ImGui::MenuItem("Text")) {
			Entity entity = scene->create("Entity");

			auto& tc = entity.add_component<TextRenderer>();
			tc.text = "Hello, World!";

			scene->select_entity(entity);

			g_modify_info.set_modified();

			ImGui::CloseCurrentPopup();
		}

		if (ImGui::MenuItem("Camera")) {
			Entity entity = scene->create("Entity");

			entity.add_component<CameraComponent>();

			scene->select_entity(entity);

			g_modify_info.set_modified();

			ImGui::CloseCurrentPopup();
		}

		if (ImGui::MenuItem("Physics Object")) {
			Entity entity = scene->create("Entity");
			entity.add_component<Rigidbody2D>();

			scene->select_entity(entity);

			g_modify_info.set_modified();

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::Button(ICON_FA_PLUS)) {
		ImGui::OpenPopup("HierarchyEntityPopup");
	}

	ImGui::Separator();

	entities_to_remove.clear();

	for (auto [UID, entity] : scene->entity_map) {
		ImGui::PushID((uint64_t)UID);

		// If entity has not any parent (top level).
		if (!entity.get_parent()) {
			_draw_entity(entity);
		}

		ImGui::PopID();
	}

	ImGui::Dummy(ImVec2(ImGui::GetWindowWidth(),
			std::max(ImGui::GetContentRegionAvail().y, 100.0f)));

	// If an item dragged here it will set as top object.
	_draw_entity_drag_drop_target(INVALID_ENTITY);

	// remove all selected entities if clicked outside the bounds
	if (ImGui::IsItemClicked()) {
		scene->clear_selected_entities();
	}

	for (auto entity : entities_to_remove) {
		scene->destroy(entity);
	}
}

void HierarchyPanel::_draw_entity(Entity entity, bool is_child) {
	if (!entity) {
		return;
	}

	auto scene = SceneManager::get_active();

	ImGui::PushID((uint64_t)entity.get_uid());

	const std::string hierarchy_name = entity.get_name();

	const bool is_selected = scene->is_entity_selected(entity);

	static const float entity_indent = 0.75f;

	if (!entity.is_parent()) {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 1, 2 });

		ImGui::Bullet();

		if (ImGui::Selectable(hierarchy_name.c_str(), is_selected)) {
			if (!ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
				scene->clear_selected_entities();
			}

			scene->toggle_entity_selection(entity);
		}

		ImGui::PopStyleVar();

		_draw_entity_context_menu(entity);

		// Drag and drop source and target to change relations.
		_draw_entity_drag_drop_src(entity);
		_draw_entity_drag_drop_target(entity);
	} else {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 1, 2 });

		bool node_open = ImGui::TreeNodeEx(hierarchy_name.c_str(),
				ImGuiTreeNodeFlags_FramePadding |
						ImGuiTreeNodeFlags_OpenOnArrow |
						ImGuiTreeNodeFlags_OpenOnDoubleClick |
						(is_selected ? ImGuiTreeNodeFlags_Selected : 0));

		ImGui::PopStyleVar();

		_draw_entity_context_menu(entity);

		// Drag and drop source and target to change relations.
		_draw_entity_drag_drop_src(entity);
		_draw_entity_drag_drop_target(entity);

		if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left) &&
				ImGui::IsItemClicked()) {
			scene->select_entity_only(entity);
		}

		if (node_open) {
			ImGui::Indent(entity_indent);

			for (Entity& child : entity.get_children()) {
				_draw_entity(child, true);
			}

			ImGui::Unindent(entity_indent);

			ImGui::TreePop();
		}
	}

	ImGui::PopID();
}

void HierarchyPanel::_draw_entity_context_menu(Entity entity) {
	auto scene = SceneManager::get_active();
	if (!scene || !entity) {
		return;
	}

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Add")) {
			scene->create("Entity");

			g_modify_info.set_modified();
		}

		if (entity) {
			if (ImGui::MenuItem("Add Child")) {
				scene->create("Entity", entity.get_uid());

				g_modify_info.set_modified();
			}

			if (ImGui::MenuItem("Delete")) {
				// Handle logic to remove the entity
				const auto selected_entities = scene->get_selected_entities();

				entities_to_remove.insert(entities_to_remove.end(),
						selected_entities.begin(), selected_entities.end());

				g_modify_info.set_modified();
			}
		}

		ImGui::EndPopup();
	}
}

void HierarchyPanel::_draw_entity_drag_drop_src(Entity entity) {
	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload(
				"DND_PAYLOAD_ENTITY", &entity.get_uid(), sizeof(UID));

		ImGui::SetTooltip("%s", entity.get_name().c_str());

		ImGui::EndDragDropSource();
	}
}

void HierarchyPanel::_draw_entity_drag_drop_target(const Entity& new_parent) {
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload =
						ImGui::AcceptDragDropPayload("DND_PAYLOAD_ENTITY")) {
			const UID recv_id = *static_cast<UID*>(payload->Data);
			if (Entity recv_entity =
							SceneManager::get_active()->find_by_id(recv_id);
					recv_entity) {
				recv_entity.set_parent(new_parent);

				g_modify_info.set_modified();
			}
		}

		ImGui::EndDragDropTarget();
	}
}
