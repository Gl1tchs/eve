#include "scene/scene.h"

#include "core/transform.h"
#include "scene/components.h"
#include "scene/entity.h"

Scene::Scene(const char* name) :
		name(name) {
}

Entity Scene::create(const std::string& name, UID parent_id) {
	return create(UID(), name, parent_id);
}

Entity Scene::create(UID uid, const std::string& name, UID parent_id) {
	Entity entity{ registry.create(), this };

	entity.add_component<IdComponent>(uid, name);
	entity.add_component<Transform>();
	entity.add_component<RelationComponent>();

	if (parent_id) {
		Entity parent = find_by_id(parent_id);
		if (parent) {
			entity.set_parent(parent);
		}
	}

	entity_map[uid] = entity;

	return entity;
}

void Scene::destroy(Entity entity) {
	for (auto child : entity.get_children()) {
		destroy(child);
	}

	entity_map.erase(entity.get_uid());
	registry.destroy(entity);
}

void Scene::destroy(UID uid) {
	Entity entity = find_by_id(uid);
	if (!entity) {
		return;
	}

	destroy(entity);
}

bool Scene::exists(UID uid) const {
	return entity_map.find(uid) != entity_map.end();
}

Entity Scene::find_by_id(UID uid) {
	if (entity_map.find(uid) != entity_map.end()) {
		return { entity_map.at(uid), this };
	}
	return {};
}

Entity Scene::find_by_name(const std::string& name) {
	auto view = registry.view<IdComponent>();
	for (auto entity : view) {
		const IdComponent& id_comp = get_component<IdComponent>(entity);
		if (id_comp.tag == name) {
			return { entity, this };
		}
	}
	return {};
}

template <typename... Component>
inline static void copy_component(
		entt::registry& dst, entt::registry& src,
		const std::unordered_map<UID, entt::entity>& entt_map) {
	(
			[&]() {
				auto view = src.view<Component>();
				for (auto src_entity : view) {
					entt::entity dst_entity =
							entt_map.at(src.get<IdComponent>(src_entity).id);

					auto& src_component = src.get<Component>(src_entity);
					dst.emplace_or_replace<Component>(dst_entity, src_component);
				}
			}(),
			...);
}

template <typename... Component>
static void copy_component(
		ComponentGroup<Component...>, entt::registry& dst, entt::registry& src,
		const std::unordered_map<UID, entt::entity>& entt_map) {
	copy_component<Component...>(dst, src, entt_map);
}

template <typename... Component>
static void copy_component_if_exists(Entity dst, Entity src) {
	(
			[&]() {
				if (src.has_component<Component>())
					dst.add_or_replace_component<Component>(src.get_component<Component>());
			}(),
			...);
}

template <typename... Component>
static void copy_component_if_exists(ComponentGroup<Component...>, Entity dst,
		Entity src) {
	copy_component_if_exists<Component...>(dst, src);
}

void Scene::copy_to(Ref<Scene> dst) {
	auto& src_registry = registry;
	auto& dst_registry = dst->registry;
	std::unordered_map<UID, entt::entity> entt_map;

	// Create entities in new scene
	auto id_view = src_registry.view<IdComponent>();
	for (auto entity_id : id_view) {
		const auto& id_comp = get_component<IdComponent>(entity_id);

		const UID& uid = id_comp.id;
		const auto& name = id_comp.tag;

		Entity new_entity =
				dst->create(uid, name);

		// set parent id but do not set children vectors
		const auto& relation = get_component<RelationComponent>(entity_id);
		new_entity.get_relation().parent_id = relation.parent_id;

		entt_map[uid] = (entt::entity)new_entity;
	}

	copy_component(AllComponents{}, dst_registry, src_registry,
			entt_map);

	// set child/parent relations
	for (auto& [uid, entity] : dst->entity_map) {
		// Set parent entity
		auto& relation = entity.get_relation();
		if (relation.parent_id) {
			auto parent_entity = dst->find_by_id(relation.parent_id);
			if (parent_entity) {
				entity.set_parent(parent_entity);
			}
		}
	}
}
