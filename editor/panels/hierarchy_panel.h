#ifndef HIERARCHY_PANEL_H
#define HIERARCHY_PANEL_H

#include "panels/panel.h"

#include "scene/entity.h"
#include "scene/scene.h"
#include "scene/scene_manager.h"

class HierarchyPanel : public Panel {
	EVE_IMPL_PANEL("Hierarchy")

public:
	HierarchyPanel();

	void set_selected_entity(Entity entity);

	Entity get_selected_entity();

protected:
	void _draw() override;

private:
	void _draw_entity(Entity& entity, bool is_child = false);

	void _draw_entity_context_menu(Entity& entity);

	void _draw_entity_drag_drop_src(Entity& entity);

	void _draw_entity_drag_drop_target(const Entity& new_parent);

private:
	Entity selected_entity;
	std::vector<Entity> entities_to_remove;
};

#endif
