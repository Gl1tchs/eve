#include "renderer/renderer.h"

#include "renderer/renderer_api.h"

Renderer::Renderer() {
	RendererAPI::init();

	// create primitive objects
	_quad_primitive = create_scope<QuadPrimitive>();

	// create camera uniform buffer
	_camera_buffer = create_ref<UniformBuffer>(sizeof(CameraData), 0);
}

Renderer::~Renderer() {}

void Renderer::begin_pass(const CameraData& camera_data) {
	_camera_buffer->set_data(&camera_data, sizeof(CameraData));

	begin_batch();
}

void Renderer::end_pass() {
	flush();
}

void Renderer::draw_quad(const Transform& transform, int z_index,
		Ref<Texture2D> texture, const Color& color,
		const glm::vec2& tex_tiling) {
	if (_quad_primitive->needs_batch()) {
		next_batch();
	}

	_quad_primitive->add_instance(transform.transform_matrix(), z_index,
			texture, color, tex_tiling);
}

void Renderer::begin_batch() {
	_quad_primitive->reset();
}

void Renderer::flush() {
	_quad_primitive->render();
}

void Renderer::next_batch() {
	flush();
	begin_batch();
}
