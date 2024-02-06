#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "scene/components.h"

class Scene;

class b2World;
enum b2BodyType;

struct PhysicsSettings {
	glm::vec2 gravity = { 0.0f, -9.81f };
};

class PhysicsSystem {
public:
	PhysicsSystem(Scene* scene, const PhysicsSettings& settings = {});
	~PhysicsSystem();

	void on_physics2d_start();

	void on_physics2d_stop();

	void on_physics2d_update(float dt);

	PhysicsSettings& get_settings();

private:
	Scene* scene = nullptr;
	b2World* physics2d_world = nullptr;

	PhysicsSettings settings{};
};

b2BodyType rigidbody2d_type_to_box2d_body(Rigidbody2D::BodyType body_type);

Rigidbody2D::BodyType rigidbody2d_type_from_box2d_body(b2BodyType body_type);

#endif
