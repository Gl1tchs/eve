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

struct SpriteRendererComponent {
	AssetHandle texture;
	Color color = COLOR_WHITE;
	glm::vec2 tex_tiling = { 1, 1 };
};

struct TextRendererComponent {
	std::string text = "Add a Caption!";
	AssetHandle font = 0;
	Color fg_color = COLOR_WHITE;
	Color bg_color = COLOR_TRANSPARENT;
	float kerning = 0.0f;
	float line_spacing = 0.0f;
};

struct Rigidbody2DComponent {
	enum class BodyType { STATIC = 0,
		DYNAMIC,
		KINEMATIC };

	BodyType type = BodyType::STATIC;
	bool fixed_rotation = false;

	// Storage for runtime
	void* runtime_body = nullptr;
};

struct BoxCollider2DComponent {
	glm::vec2 offset = { 0.0f, 0.0f };
	glm::vec2 size = { 0.5f, 0.5f };

	// TODO create a physics material
	float density = 1.0f;
	float friction = 0.5f;
	float restitution = 0.0f;
	float restitution_threshold = 0.5f;

	// Storage for runtime
	void* runtime_fixture = nullptr;
};

struct CircleCollider2DComponent {
	glm::vec2 offset = { 0.0f, 0.0f };
	float radius = 0.5f;

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
		ComponentGroup<TransformComponent, CameraComponent,
				SpriteRendererComponent, TextRendererComponent,
				Rigidbody2DComponent, BoxCollider2DComponent,
				CircleCollider2DComponent, PostProcessVolume,
				ScriptComponent>;

#endif
