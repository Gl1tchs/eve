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

template <typename... Component>
struct ComponentGroup {};

using AllComponents =
		ComponentGroup<TransformComponent, CameraComponent, SpriteRendererComponent, TextRendererComponent, PostProcessVolume>;

#endif
