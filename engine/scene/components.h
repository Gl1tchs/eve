#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "core/color.h"
#include "renderer/camera.h"
#include "renderer/font.h"
#include "renderer/post_processor.h"
#include "renderer/texture.h"
#include "scene/transform.h"

struct CameraComponent {
	OrthographicCamera camera;
	bool is_primary = true;
	bool is_fixed_aspect_ratio = false;
};

struct SpriteRenderer {
	AssetHandle texture;
	Color color = COLOR_WHITE;
	glm::vec2 tex_tiling = { 1, 1 };
};

struct TextRenderer {
	std::string text = "Add a Caption!";
	AssetHandle font = 0;
	Color fg_color = COLOR_WHITE;
	Color bg_color = COLOR_TRANSPARENT;
	float kerning = 0.0f;
	float line_spacing = 0.0f;
	bool is_screen_space = false;
};

struct Rigidbody2D {
	enum class BodyType { STATIC = 0,
		DYNAMIC,
		KINEMATIC };

	enum ForceMode {
		FORCE,
		IMPULSE,
	};

	struct ForceInfo {
		ForceMode mode;
		glm::vec2 force;
		glm::vec2 offset;
	};

	BodyType type = BodyType::STATIC;
	bool fixed_rotation = false;

	glm::vec2 velocity;
	float angular_velocity;

	std::vector<ForceInfo> forces;

	// kgmm/s
	float angular_impulse;
	// N-m
	float torque;

	// Storage for runtime
	void* runtime_body = nullptr;
};

typedef void (*CollisionTriggerFunction)(uint64_t id);

struct BoxCollider2D {
	glm::vec2 offset = { 0.0f, 0.0f };
	glm::vec2 size = { 0.5f, 0.5f };

	bool is_trigger = false;

	// function which wills be triggered uppon trigger event
	// will be setted from script
	CollisionTriggerFunction trigger_function = nullptr;

	// TODO create a physics material
	float density = 1.0f;
	float friction = 0.5f;
	float restitution = 0.0f;
	float restitution_threshold = 0.5f;

	// Storage for runtime
	void* runtime_fixture = nullptr;
};

struct CircleCollider2D {
	glm::vec2 offset = { 0.0f, 0.0f };
	float radius = 0.5f;

	bool is_trigger = false;

	// function which will be triggered uppon trigger event
	// will be setted from script
	CollisionTriggerFunction trigger_function = nullptr;

	// TODO create a physics material
	float density = 1.0f;
	float friction = 0.5f;
	float restitution = 0.0f;
	float restitution_threshold = 0.5f;

	// Storage for runtime
	void* runtime_fixture = nullptr;
};

struct ScriptComponent {
	std::string class_name;
};

template <typename... Component>
struct ComponentGroup {};

using AllComponents =
		ComponentGroup<Transform, CameraComponent,
				SpriteRenderer, TextRenderer,
				Rigidbody2D, BoxCollider2D,
				CircleCollider2D, PostProcessVolume,
				ScriptComponent>;

#endif
