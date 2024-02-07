#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "scene/components.h"

class Entity;
class Scene;

class b2World;
class b2Body;
enum b2BodyType;

struct PhysicsSettings {
	glm::vec2 gravity = { 0.0f, -9.81f };
};

class PhysicsSystem {
public:
	PhysicsSystem(Scene* scene, const PhysicsSettings& settings = {});
	~PhysicsSystem();

	void start();

	void stop();

	void update(float dt);

	void mark_deleted(Entity entity);

	PhysicsSettings& get_settings();

private:
	Scene* scene = nullptr;

	b2World* world2d = nullptr;

	std::vector<b2Body*> bodies_to_remove;

	PhysicsSettings settings{};
};

#endif
