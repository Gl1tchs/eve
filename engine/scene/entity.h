#ifndef ENTITY_H
#define ENTITY_H

#include "scene/transform.h"
#include "core/uid.h"
#include "scene/scene.h"

#include <entt/entt.hpp>

struct IdComponent {
	UID id;
	std::string tag;
};

struct RelationComponent {
	UID parent_id = 0;
	std::vector<UID> children_ids{};
};

class Entity {
public:
	inline constexpr Entity() :
			handle(entt::null), scene(nullptr) {}

	inline Entity(entt::entity handle, Scene* scene) :
			handle(handle), scene(scene) {
	}

	Entity(const Entity& other) = default;

	template <typename T, typename... Args>
	inline auto& add_component(Args&&... args) {
		return scene->add_component<T>(handle, std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	inline T& add_or_replace_component(Args&&... args) {
		return scene->add_or_replace_component<T>(handle, std::forward<Args>(args)...);
	}

	template <typename T>
	inline T& get_component() {
		return scene->get_component<T>(handle);
	}

	template <typename T>
	inline const T& get_component() const {
		return scene->get_component<T>(handle);
	}

	template <typename... Components>
	inline bool has_component() const {
		return scene->has_component<Components...>(handle);
	}

	template <typename T>
	inline void remove_component() {
		scene->remove_component<T>(handle);
	}

	RelationComponent& get_relation();
	const RelationComponent& get_relation() const;

	Entity get_parent() const;
	void set_parent(Entity parent);

	bool is_parent() const;

	bool is_child() const;

	std::vector<Entity> get_children() const;

	bool remove_child(Entity child);

	static bool is_parent_of(Entity parent, Entity child);

	const UID& get_uid() const;

	const std::string& get_name();

	TransformComponent& get_transform();

	operator bool() const;

	operator entt::entity() const;

	operator uint32_t() const;

	bool operator==(const Entity& other) const;

	bool operator!=(const Entity& other) const;

private:
	entt::entity handle;
	Scene* scene = nullptr;
};

constexpr Entity INVALID_ENTITY{};

#endif