#include "scene/entity.h"
#include "scene/transform.h"
#include "core/uid.h"

RelationComponent& Entity::get_relation() {
	return get_component<RelationComponent>();
}

const RelationComponent& Entity::get_relation() const {
	return get_component<RelationComponent>();
}

Entity Entity::get_parent() const {
	const auto parent_id = get_relation().parent_id;
	if (!parent_id) {
		return INVALID_ENTITY;
	}

	return scene->find_by_id(parent_id);
}

void Entity::set_parent(Entity parent) {
	// check if this is not the parent of "parent"
	if (is_parent_of(*this, parent)) {
		return;
	}

	// Remove old parent if exists.
	Entity current_parent = get_parent();
	if (current_parent) {
		current_parent.remove_child(*this);
	}

	// If an invalid entity provided then carry this into top levels
	if (!parent) {
		return;
	}

	RelationComponent& parent_relation = parent.get_relation();
	auto& parent_children_ids = parent_relation.children_ids;

	// Setup relations between components
	RelationComponent& relation = get_relation();
	relation.parent_id = parent.get_uid();
	parent_children_ids.push_back(get_uid());

	// Make transform values relative to the new parent.
	TransformComponent& transform = get_transform();
	TransformComponent& parent_transform = parent.get_transform();

	transform.local_position =
			transform.get_position() - parent_transform.get_position();
	transform.local_rotation =
			transform.get_rotation() - parent_transform.get_rotation();
	transform.local_scale = transform.get_scale() / parent_transform.get_scale();

	transform.parent = &parent_transform;
}

bool Entity::is_parent() const {
	return get_relation().children_ids.size() > 0;
}

bool Entity::is_child() const {
	return get_relation().parent_id != INVALID_UID;
}

std::vector<Entity> Entity::get_children() const {
	// TODO optimize this seems not so fast right now.
	const std::vector<UID>& children_ids = get_relation().children_ids;

	std::vector<Entity> children{};
	for (const auto& child_id : children_ids) {
		Entity entity = scene->find_by_id(child_id);
		if (!entity) {
			continue;
		}

		children.push_back(entity);
	}

	return children;
}

bool Entity::remove_child(Entity child) {
	auto& children_ids = get_relation().children_ids;

	const auto it =
			std::find(children_ids.begin(), children_ids.end(), child.get_uid());

	if (it != children_ids.end()) {
		// Set local positions as the world position
		TransformComponent& child_transform = child.get_transform();
		child_transform.local_position = child_transform.get_position();
		child_transform.local_rotation = child_transform.get_rotation();
		child_transform.local_scale = child_transform.get_scale();

		child_transform.parent = nullptr;

		// move child to top level
		child.get_relation().parent_id = INVALID_UID;
		children_ids.erase(it);

		return true;
	}

	return false;
}

bool Entity::is_parent_of(Entity parent, Entity child) {
	if (!child) {
		return false;
	}

	// check recursively if child is one of parent's
	Entity child_parent = child.get_parent();

	// if top level return false
	if (child_parent == INVALID_ENTITY) {
		return false;
	}
	// if parents are same then we found it
	else if (child_parent == parent) {
		return true;
	}
	// otherwise call the function again
	else {
		return is_parent_of(parent, child_parent);
	}
}

const UID& Entity::get_uid() const {
	return get_component<IdComponent>().id;
}

const std::string& Entity::get_name() {
	return get_component<IdComponent>().tag;
}

TransformComponent& Entity::get_transform() {
	return get_component<TransformComponent>();
}

Entity::operator bool() const {
	return handle != entt::null && scene != nullptr;
}

Entity::operator entt::entity() const {
	return handle;
}

Entity::operator uint32_t() const {
	return (uint32_t)handle;
}

bool Entity::operator==(const Entity& other) const {
	return handle == other.handle && scene == other.scene;
}

bool Entity::operator!=(const Entity& other) const {
	return !(*this == other);
}