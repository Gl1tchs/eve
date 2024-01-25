#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "core/color.h"
#include "core/transform.h"
#include "renderer/camera.h"
#include "renderer/texture.h"

struct CameraComponent {
	OrthographicCamera camera;
	bool is_primary = true;
	bool is_fixed_aspect_ratio = false;
};

struct SpriteRendererComponent {
	Ref<Texture2D> texture;
	int z_index = 0;
	Color color = COLOR_WHITE;
	glm::vec2 tex_tiling = { 1, 1 };
};

template <typename... Component>
struct ComponentGroup {};

using AllComponents =
		ComponentGroup<Transform, CameraComponent, SpriteRendererComponent>;

#endif