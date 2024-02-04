#include "physics/physics_system.h"

#include "scene/components.h"
#include "scene/entity.h"

#include <box2d/b2_body.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>

b2BodyType rigidbody2d_type_to_box2d_body(Rigidbody2D::BodyType bodyType) {
	switch (bodyType) {
		case Rigidbody2D::BodyType::STATIC:
			return b2_staticBody;
		case Rigidbody2D::BodyType::DYNAMIC:
			return b2_dynamicBody;
		case Rigidbody2D::BodyType::KINEMATIC:
			return b2_kinematicBody;
		default: {
			EVE_ASSERT_ENGINE(false, "Unknown body type");
			return b2_staticBody;
		}
	}
}

Rigidbody2D::BodyType rigidbody2d_type_from_box2d_body(b2BodyType bodyType) {
	switch (bodyType) {
		case b2_staticBody:
			return Rigidbody2D::BodyType::STATIC;
		case b2_dynamicBody:
			return Rigidbody2D::BodyType::DYNAMIC;
		case b2_kinematicBody:
			return Rigidbody2D::BodyType::KINEMATIC;
		default: {
			EVE_ASSERT_ENGINE(false, "Unknown body type");
			return Rigidbody2D::BodyType::STATIC;
		}
	}
}

PhysicsSystem::PhysicsSystem(Scene* scene, const PhysicsSettings& settings) :
		scene(scene), settings(settings) {
	physics2d_world = new b2World({ settings.gravity.x, settings.gravity.y });
}

PhysicsSystem::~PhysicsSystem() {
	delete physics2d_world;
}

void PhysicsSystem::on_physics2d_start() {
	EVE_PROFILE_FUNCTION();

	physics2d_world->SetGravity({ settings.gravity.x, settings.gravity.y });

	for (auto entity_id : scene->view<Rigidbody2D>()) {
		Entity entity{ entity_id, scene };

		auto& transform = entity.get_transform();
		auto& rb2d = entity.get_component<Rigidbody2D>();

		b2BodyDef body_def;
		body_def.type = rigidbody2d_type_to_box2d_body(rb2d.type);
		body_def.position.Set(transform.get_position().x, transform.get_position().y);
		body_def.angle = transform.get_rotation().z;

		b2Body* body = physics2d_world->CreateBody(&body_def);
		body->SetFixedRotation(rb2d.fixed_rotation);
		rb2d.runtime_body = body;

		if (entity.has_component<BoxCollider2D>()) {
			auto& bc2d = entity.get_component<BoxCollider2D>();

			b2PolygonShape box_shape;
			box_shape.SetAsBox(bc2d.size.x * transform.get_scale().x,
					bc2d.size.y * transform.get_scale().y,
					b2Vec2(bc2d.offset.x, bc2d.offset.y),
					0.0f);

			b2FixtureDef fixture_def;
			fixture_def.shape = &box_shape;
			fixture_def.density = bc2d.density;
			fixture_def.friction = bc2d.friction;
			fixture_def.restitution = bc2d.restitution;
			fixture_def.restitutionThreshold = bc2d.restitution_threshold;
			body->CreateFixture(&fixture_def);
		}

		if (entity.has_component<CircleCollider2D>()) {
			auto& cc2d = entity.get_component<CircleCollider2D>();

			b2CircleShape circle_shape;
			circle_shape.m_p.Set(cc2d.offset.x, cc2d.offset.y);
			circle_shape.m_radius = transform.get_scale().x * cc2d.radius;

			b2FixtureDef fixture_def;
			fixture_def.shape = &circle_shape;
			fixture_def.density = cc2d.density;
			fixture_def.friction = cc2d.friction;
			fixture_def.restitution = cc2d.restitution;
			fixture_def.restitutionThreshold = cc2d.restitution_threshold;
			body->CreateFixture(&fixture_def);
		}
	}
}

void PhysicsSystem::on_physics2d_stop() {
	EVE_PROFILE_FUNCTION();

	delete physics2d_world;

	physics2d_world = nullptr;
	scene = nullptr;
}

void PhysicsSystem::on_physics2d_update(float dt) {
	if (!scene) {
		EVE_LOG_ENGINE_ERROR("Could not update Physics2D, no scene context found!");
		return;
	}

	EVE_PROFILE_FUNCTION();

	constexpr int velocity_iters = 6;
	constexpr int position_iters = 2;

	physics2d_world->Step(dt, velocity_iters, position_iters);

	// Retrieve transform from Box2D
	for (auto e : scene->view<Rigidbody2D>()) {
		Entity entity = { e, scene };

		auto& transform = entity.get_transform();
		auto& rb2d = entity.get_component<Rigidbody2D>();

		b2Body* body = (b2Body*)rb2d.runtime_body;

		const auto& position = body->GetPosition();
		transform.local_position.x = position.x;
		transform.local_position.y = position.y;
		transform.local_rotation.z = body->GetAngle();
	}
}

PhysicsSettings& PhysicsSystem::get_settings() {
	return settings;
}
