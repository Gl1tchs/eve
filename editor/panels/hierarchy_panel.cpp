// Copyright (c) 2023 Berke Umut Biricik All Rights Reserved

#include "panels/hierarchy_panel.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "data/fonts/font_awesome.h"
#include "scene/entity.h"

HierarchyPanel::HierarchyPanel() {
	set_flags(ImGuiWindowFlags_HorizontalScrollbar);
}

void HierarchyPanel::set_selected_entity(Entity entity) {
	selected_entity = entity;
}

Entity HierarchyPanel::get_selected_entity() {
	if (selected_entity &&
			!SceneManager::get_active()->exists(selected_entity)) {
		return INVALID_ENTITY;
	}

	return selected_entity;
}

void HierarchyPanel::_draw() {
	auto scene = SceneManager::get_active();
	if (!scene) {
		selected_entity = INVALID_ENTITY;
		return;
	}

	// If scene changed during runtime
	if (selected_entity.scene != scene.get()) {
		selected_entity = Entity{ selected_entity.handle, scene.get() };
	}

	ImGui::Text("scene: %s", scene->name.c_str());

	ImGui::SameLine(ImGui::GetContentRegionMax().x -
			(ImGui::CalcTextSize(ICON_FA_PLUS).x +
					2 * ImGui::GetStyle().FramePadding.x));

	if (ImGui::Button(ICON_FA_PLUS)) {
		// create entity and set selected to new created entity
		selected_entity = scene->create("Entity");
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

	ImGui::Dummy(
			ImVec2(ImGui::GetWindowWidth(), std::max(ImGui::GetContentRegionAvail().y, 100.0f)));

	// If an item dragged here it will set as top object.
	_draw_entity_drag_drop_target(INVALID_ENTITY);

	if (ImGui::IsItemClicked()) {
		selected_entity = INVALID_ENTITY;
	}

	for (auto entity : entities_to_remove) {
		scene->destroy(entity);
	}
}

void HierarchyPanel::_draw_entity(Entity& entity, bool is_child) {
	if (!entity) {
		return;
	}

	ImGui::PushID((uint64_t)entity.get_uid());

	const std::string hierarchy_name = entity.get_name();

	const bool is_selected = selected_entity == entity;

	static const float entity_indent = 0.75f;

	if (!entity.is_parent()) {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 1, 2 });

		ImGui::Bullet();

		if (ImGui::Selectable(hierarchy_name.c_str(), is_selected)) {
			set_selected_entity(entity);
		}

		ImGui::PopStyleVar();

		_draw_entity_context_menu(entity);

		// Drag and drop source and target to change relations.
		_draw_entity_drag_drop_src(entity);
		_draw_entity_drag_drop_target(entity);
	} else {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 1, 2 });

		bool node_open = ImGui::TreeNodeEx(
				hierarchy_name.c_str(),
				ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
						(is_selected ? ImGuiTreeNodeFlags_Selected : 0));

		ImGui::PopStyleVar();

		_draw_entity_context_menu(entity);

		// Drag and drop source and target to change relations.
		_draw_entity_drag_drop_src(entity);
		_draw_entity_drag_drop_target(entity);

		if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left) &&
				ImGui::IsItemClicked()) {
			set_selected_entity(entity);
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

void HierarchyPanel::_draw_entity_context_menu(Entity& entity) {
	auto scene = SceneManager::get_active();
	if (!scene || !entity) {
		return;
	}

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Add")) {
			scene->create("Entity");
		}

		if (ImGui::MenuItem("Add Child")) {
			scene->create("Entity", entity.get_uid());
		}

		if (ImGui::MenuItem("Delete")) {
			// Handle logic to remove the entity
			entities_to_remove.push_back(entity);
		}

		ImGui::EndPopup();
	}
}

void HierarchyPanel::_draw_entity_drag_drop_src(Entity& entity) {
	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("DND_PAYLOAD_ENTITY", &entity.get_uid(),
				sizeof(UID));

		ImGui::SetTooltip("%s", entity.get_name().c_str());

		ImGui::EndDragDropSource();
	}
}

void HierarchyPanel::_draw_entity_drag_drop_target(const Entity& new_parent) {
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload =
						ImGui::AcceptDragDropPayload("DND_PAYLOAD_ENTITY")) {
			UID recv_id = *static_cast<UID*>(payload->Data);
			if (Entity recv_entity =
							SceneManager::get_active()->find_by_id(recv_id);
					recv_entity) {
				recv_entity.set_parent(new_parent);
			}
		}

		ImGui::EndDragDropTarget();
	}
}
