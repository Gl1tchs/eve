#include "physics/physics_system.h"

#include "core/application.h"
#include "scene/components.h"
#include "scene/entity.h"

#include <box2d/b2_body.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>

struct FixtureUserData {
	Entity entity = INVALID_ENTITY;
	bool is_trigger = false;
	CollisionTriggerFunction trigger_function = nullptr;
};

class Physics2DContactListener : public b2ContactListener {
	inline void BeginContact(b2Contact* contact) override {
		b2Fixture* fixture_a = contact->GetFixtureA();
		b2Fixture* fixture_b = contact->GetFixtureB();

		FixtureUserData* user_data_a = reinterpret_cast<FixtureUserData*>(fixture_a->GetUserData().pointer);
		FixtureUserData* user_data_b = reinterpret_cast<FixtureUserData*>(fixture_b->GetUserData().pointer);

		if (!user_data_a || !user_data_b) {
			return;
		}

		if (user_data_a->is_trigger && user_data_a->trigger_function && user_data_b->entity) {
			user_data_a->trigger_function(/* user_data_b->entity.get_uid() */);
		}

		if (user_data_b->is_trigger && user_data_b->trigger_function && user_data_a->entity) {
			user_data_b->trigger_function(/* user_data_a->entity.get_uid() */);
		}
	}
};

inline static b2BodyType rigidbody2d_type_to_box2d_body(Rigidbody2D::BodyType bodyType) {
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

inline static Rigidbody2D::BodyType rigidbody2d_type_from_box2d_body(b2BodyType bodyType) {
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

static std::vector<Scope<FixtureUserData>> s_fixture_user_datas{};

PhysicsSystem::PhysicsSystem(Scene* scene, const PhysicsSettings& settings) :
		scene(scene), settings(settings) {
	world2d = new b2World({ settings.gravity.x, settings.gravity.y });

	static Physics2DContactListener s_contact_listener2d{};
	world2d->SetContactListener(&s_contact_listener2d);
}

PhysicsSystem::~PhysicsSystem() {
	delete world2d;
}

inline static b2Body* create_body(Entity entity, b2World* world) {
	auto& transform = entity.get_transform();
	auto& rb2d = entity.get_component<Rigidbody2D>();

	b2BodyDef body_def;
	body_def.type = rigidbody2d_type_to_box2d_body(rb2d.type);
	body_def.position.Set(transform.get_position().x, transform.get_position().y);
	body_def.angle = glm::radians(transform.get_rotation().z);

	b2Body* body = world->CreateBody(&body_def);
	body->SetFixedRotation(rb2d.fixed_rotation);

	rb2d.runtime_body = body;

	return body;
}

inline static b2Fixture* create_box_fixture(Entity entity, b2Body* body) {
	auto& transform = entity.get_transform();
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

	// set user data
	auto user_data = create_scope<FixtureUserData>();
	user_data->entity = entity;
	user_data->is_trigger = bc2d.is_trigger;
	user_data->trigger_function = bc2d.trigger_function;

	fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(user_data.get());

	s_fixture_user_datas.emplace_back(std::move(user_data));
	user_data = nullptr;

	b2Fixture* fixture = body->CreateFixture(&fixture_def);

	bc2d.runtime_fixture = fixture;

	return fixture;
}

inline static b2Fixture* create_circle_fixture(Entity entity, b2Body* body) {
	auto& transform = entity.get_transform();
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

	// set user data
	auto user_data = create_scope<FixtureUserData>();
	user_data->entity = entity;
	user_data->is_trigger = cc2d.is_trigger;
	user_data->trigger_function = cc2d.trigger_function;

	fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(user_data.get());

	s_fixture_user_datas.emplace_back(std::move(user_data));
	user_data = nullptr;

	cc2d.runtime_fixture = body->CreateFixture(&fixture_def);

	b2Fixture* fixture = body->CreateFixture(&fixture_def);
	cc2d.runtime_fixture = fixture;

	return fixture;
}

void PhysicsSystem::start() {
	EVE_PROFILE_FUNCTION();

	world2d->SetGravity({ settings.gravity.x, settings.gravity.y });

	for (auto entity_id : scene->view<Rigidbody2D>()) {
		Entity entity{ entity_id, scene };

		b2Body* body = create_body(entity, world2d);

		if (entity.has_component<BoxCollider2D>()) {
			create_box_fixture(entity, body);
		}

		if (entity.has_component<CircleCollider2D>()) {
			create_circle_fixture(entity, body);
		}
	}
}

void PhysicsSystem::stop() {
	EVE_PROFILE_FUNCTION();

	delete world2d;

	world2d = nullptr;
	scene = nullptr;

	s_fixture_user_datas.clear();
}

void PhysicsSystem::update(float dt) {
	if (!scene) {
		EVE_LOG_ENGINE_ERROR("Could not update Physics2D, no scene context found!");
		return;
	}

	EVE_PROFILE_FUNCTION();

	{
		for (auto body : bodies_to_remove) {
			world2d->DestroyBody(body);
		}

		bodies_to_remove.clear();
	}

	constexpr int velocity_iters = 6;
	constexpr int position_iters = 2;

	world2d->Step(dt, velocity_iters, position_iters);

	// Retrieve transform from Box2D
	for (auto e : scene->view<Rigidbody2D>()) {
		Entity entity = { e, scene };

		auto& transform = entity.get_transform();
		auto& rb2d = entity.get_component<Rigidbody2D>();

		b2Body* body = (b2Body*)rb2d.runtime_body;
		if (!body) {
			body = create_body(entity, world2d);
		}

		body->SetType(rigidbody2d_type_to_box2d_body(rb2d.type));
		body->SetFixedRotation(rb2d.fixed_rotation);

		if (entity.has_component<BoxCollider2D>()) {
			auto& bc2d = entity.get_component<BoxCollider2D>();

			b2Fixture* fixture = (b2Fixture*)bc2d.runtime_fixture;
			if (!fixture) {
				fixture = create_box_fixture(entity, body);
			}

			if (fixture->GetShape()->GetType() == b2Shape::Type::e_polygon) {
				//? TODO maybe cache it
				b2PolygonShape* shape = (b2PolygonShape*)fixture->GetShape();
				shape->SetAsBox(bc2d.size.x * transform.get_scale().x,
						bc2d.size.y * transform.get_scale().y,
						b2Vec2(bc2d.offset.x, bc2d.offset.y),
						0.0f);
			}

			fixture->SetDensity(bc2d.density);
			fixture->SetFriction(bc2d.friction);
			fixture->SetRestitution(bc2d.restitution);
			fixture->SetRestitutionThreshold(bc2d.restitution_threshold);
		}

		if (entity.has_component<CircleCollider2D>()) {
			auto& cc2d = entity.get_component<CircleCollider2D>();

			b2Fixture* fixture = (b2Fixture*)cc2d.runtime_fixture;
			if (!fixture) {
				fixture = create_circle_fixture(entity, body);
			}

			if (fixture->GetShape()->GetType() == b2Shape::Type::e_circle) {
				b2CircleShape* shape = (b2CircleShape*)fixture->GetShape();
				shape->m_p.Set(cc2d.offset.x, cc2d.offset.y);
				shape->m_radius = transform.get_scale().x * cc2d.radius;
			}

			fixture->SetDensity(cc2d.density);
			fixture->SetFriction(cc2d.friction);
			fixture->SetRestitution(cc2d.restitution);
			fixture->SetRestitutionThreshold(cc2d.restitution_threshold);
		}

		const auto& position = body->GetPosition();
		transform.local_position.x = position.x;
		transform.local_position.y = position.y;
		transform.local_rotation.z = glm::degrees(body->GetAngle());
	}
}

void PhysicsSystem::mark_deleted(Entity entity) {
	if (!entity.has_component<Rigidbody2D>()) {
		return;
	}

	const auto rb2d = entity.get_component<Rigidbody2D>();
	if (!rb2d.runtime_body) {
		return;
	}

	b2Body* body = (b2Body*)rb2d.runtime_body;

	bodies_to_remove.push_back(body);
}

PhysicsSettings& PhysicsSystem::get_settings() {
	return settings;
}
