#ifndef SCENE_H
#define SCENE_H

#include "asset/asset.h"

#include <entt/entt.hpp>

class Entity;

class Scene : public Asset {
public:
	EVE_IMPL_ASSET(AssetType::SCENE)

	Scene(const char* name = "default");

	void start();

	void update(float dt);

	void stop();

	void set_paused(bool paused);

	void step(uint32_t frames);

	bool is_running();

	bool is_paused();

	Entity create(const std::string& name, UID parent_id = 0);
	Entity create(UID uid, const std::string& name, UID parent_id = INVALID_UID);

	// FIXME
	//	if script running remove from script engine
	void destroy(Entity entity);
	void destroy(UID uid);

	bool exists(UID uid) const;

	Entity find_by_id(UID uid);
	Entity find_by_name(const std::string& name);

	template <typename... Components>
	auto view() {
		return registry.view<Components...>();
	}

	template <typename T, typename... Args>
	auto& add_component(entt::entity handle, Args&&... args) {
		EVE_ASSERT_ENGINE(!has_component<T>(handle), "Entity already has that component!");
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
		EVE_ASSERT_ENGINE(has_component<T>(handle), "Entity does not have component!");
		return registry.get<T>(handle);
	}

	template <typename T>
	const T& get_component(entt::entity handle) const {
		EVE_ASSERT_ENGINE(has_component<T>(handle), "Entity does not have component!");
		return registry.get<T>(handle);
	}

	template <typename... Components>
	bool has_component(entt::entity handle) const {
		return registry.all_of<Components...>(handle);
	}

	template <typename T>
	void remove_component(entt::entity handle) {
		EVE_ASSERT_ENGINE(has_component<T>(handle), "Entity does not have component!");
		registry.remove<T>(handle);
	}

	// DISCLAIMER
	//  only the components that are defined in components.h:AllComponents<>
	//  component group will be copied.
	void copy_to(Ref<Scene> dst);

	static void serialize(Ref<Scene> scene, const fs::path& path);

	static bool deserialize(Ref<Scene>& scene, const fs::path& path);

private:
	std::string name;

	bool running = false;
	bool paused = false;
	int step_frames = 0;

	entt::registry registry;
	std::unordered_map<UID, Entity> entity_map;
};

#endif
