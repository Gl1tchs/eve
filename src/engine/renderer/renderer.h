#ifndef RENDERER_H
#define RENDERER_H

#include "core/color.h"
#include "core/transform.h"
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

	void draw_quad(const Transform& transform, int z_index = 0,
			Ref<Texture2D> texture = nullptr,
			const Color& color = { 1, 1, 1, 1 },
			const glm::vec2& tex_tiling = { 1, 1 });

private:
	void begin_batch();

	void flush();

	void next_batch();

private:
	Scope<QuadPrimitive> _quad_primitive;
	Ref<UniformBuffer> _camera_buffer;
};

#endif