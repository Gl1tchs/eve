#include "renderer/renderer.h"

#include "renderer/renderer_api.h"

Renderer::Renderer() {
	RendererAPI::init();

	// create primitive objects
	quad_primitive = create_scope<QuadPrimitive>();

	// create camera uniform buffer
	camera_buffer = create_ref<UniformBuffer>(sizeof(CameraData), 0);
}

Renderer::~Renderer() {}

void Renderer::begin_pass(const CameraData& camera_data) {
	camera_buffer->set_data(&camera_data, sizeof(CameraData));

	_begin_batch();
}

void Renderer::end_pass() {
	_flush();
}

void Renderer::draw_quad(const TransformComponent& transform, int z_index,
		Ref<Texture2D> texture, const Color& color,
		const glm::vec2& tex_tiling) {
	if (quad_primitive->needs_batch()) {
		_next_batch();
	}

	quad_primitive->add_instance(transform.get_transform_matrix(), z_index,
			texture, color, tex_tiling);
}

void Renderer::_begin_batch() {
	quad_primitive->reset();
}

void Renderer::_flush() {
	quad_primitive->render();
}

void Renderer::_next_batch() {
	_flush();
	_begin_batch();
}
