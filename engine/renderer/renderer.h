#ifndef RENDERER_H
#define RENDERER_H

#include "core/color.h"
#include "scene/transform.h"
#include "renderer/primitives/quad.h"
#include "renderer/uniform_buffer.h"

struct CameraData final {
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec2 position;
};

class Renderer final {
public:
	Renderer();
	~Renderer();

	void begin_pass(const CameraData& camera_data);

	void end_pass();

	void draw_quad(const TransformComponent& transform, int z_index = 0,
			Ref<Texture2D> texture = nullptr,
			const Color& color = { 1, 1, 1, 1 },
			const glm::vec2& tex_tiling = { 1, 1 });

private:
	void _begin_batch();

	void _flush();

	void _next_batch();

private:
	Scope<QuadPrimitive> quad_primitive;
	Ref<UniformBuffer> camera_buffer;
};

#endif