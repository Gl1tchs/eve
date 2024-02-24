#ifndef SCENE_H
#define SCENE_H

#include "asset/asset_registry.h"
#include "physics/physics_system.h"

#include <entt/entt.hpp>

class Entity;

class Scene {
public:
	Scene(const std::string& name = "default");

	// runtime beheaviours

	void start();

	void update(float dt);

	void stop();

	void set_paused(bool _paused);

	void step(uint32_t frames = 1);

	bool is_running();

	bool is_paused();

	// ECS

	Entity create(const std::string& name, UID parent_id = 0);
	Entity create(UID uid, const std::string& name, UID parent_id = INVALID_UID);

	void destroy(Entity entity);
	void destroy(UID uid);

	// will check if entity valid or not
	bool exists(Entity entity) const;

	// won't check entity validity but looks for the id
	bool exists(UID uid) const;

	Entity find_by_id(UID uid);
	Entity find_by_name(const std::string& name);

	template <typename... Components>
	auto view() {
		return registry.view<Components...>();
	}

	template <typename T, typename... Args>
	auto& add_component(entt::entity handle, Args&&... args) {
		EVE_ASSERT(!has_component<T>(handle), "Entity already has that component!");
		return registry.emplace<T>(handle,
				std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	T& add_or_replace_component(entt::entity handle, Args&&... args) {
		T& component = registry.emplace_or_replace<T>(
				handle, std::forward<Args>(args)...);
		return component;
	}

	template <typename T>
	T& get_component(entt::entity handle) {
		EVE_ASSERT(has_component<T>(handle), "Entity does not have component!");
		return registry.get<T>(handle);
	}

	template <typename T>
	const T& get_component(entt::entity handle) const {
		EVE_ASSERT(has_component<T>(handle), "Entity does not have component!");
		return registry.get<T>(handle);
	}

	template <typename... Components>
	bool has_component(entt::entity handle) const {
		return registry.all_of<Components...>(handle);
	}

	template <typename T>
	void remove_component(entt::entity handle) {
		EVE_ASSERT(has_component<T>(handle), "Entity does not have component!");
		registry.remove<T>(handle);
	}

	// adds | removes entity to the list based on if it already exists
	void toggle_entity_selection(Entity entity);

	// clears selected entities and adds entity to the list
	void select_entity_only(Entity entity);

	// adds entity to the list if not exists
	void select_entity(Entity entity);

	// removes entity from list if not exists
	void unselect_entity(Entity entity);

	// removes all entities from selected list.
	void clear_selected_entities();

	bool is_entity_selected(Entity entity) const;

	const std::string& get_name();

	const std::string& get_path();

	AssetRegistry& get_asset_registry();

	const std::vector<Entity>& get_selected_entities() const;

	// DISCLAIMER
	//  only the components that are defined in components.h:AllComponents<>
	//  component group will be copied.
	static Ref<Scene> copy(Ref<Scene> src);

	static void serialize(Ref<Scene> scene, std::string path);

	static bool deserialize(Ref<Scene>& scene, std::string path);

private:
	AssetHandle handle;
	std::string name;
	std::string path;

	PhysicsSystem physics_system;

	bool running = false;
	bool paused = false;
	int step_frames = 0;

	entt::registry registry;
	std::unordered_map<UID, Entity> entity_map;

	AssetRegistry asset_registry;

	std::vector<Entity> selected_entities;

	friend class EditorApplication;
	friend class HierarchyPanel;
};

#endif
