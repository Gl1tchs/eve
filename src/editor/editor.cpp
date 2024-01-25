#include "editor.h"

#include "core/color.h"
#include "core/entrypoint.h"
#include "core/input.h"
#include "core/key_code.h"
#include "core/mouse_code.h"
#include "core/transform.h"
#include "renderer/renderer.h"
#include "renderer/renderer_api.h"

EditorApplication::EditorApplication(const ApplicationCreateInfo& info) :
		Application(info) {
}

EditorApplication::~EditorApplication() {}

void EditorApplication::on_start() {
	_mouse_pos = Input::mouse_position();
}

void EditorApplication::on_update(float dt) {
	_camera.aspect_ratio = window->aspect_ratio();

	// camera control
	if (Input::is_mouse_pressed(MouseCode::RIGHT)) {
		glm::vec2 mouse_delta = Input::mouse_position() - _mouse_pos;

		_camera_transform.local_position.x += mouse_delta.x * dt;
		_camera_transform.local_position.y -= mouse_delta.y * dt;

		_camera.zoom_level += Input::scroll_offset().y * _scroll_sensitivity * dt;
	}

	_mouse_pos = Input::mouse_position();

	on_render();
}

void EditorApplication::on_destroy() {}

void EditorApplication::on_render() {
	RendererAPI::clear_color(COLOR_GRAY);
	RendererAPI::clear(BUFFER_BITS_COLOR | BUFFER_BITS_DEPTH);

	renderer->begin_pass(
			{ _camera.view_matrix(_camera_transform),
					_camera.projection_matrix(),
					_camera_transform.position() });

	Transform t{};
	for (int i = -50; i <= 50; i++) {
		for (int j = -50; j <= 50; j++) {
			t.local_position = { i, j, 0 };

			Color color;
			color.r = static_cast<float>(i + 50) / 100.0f;
			color.g = 0.0f;
			color.b = static_cast<float>(j + 50) / 100.0f;
			color.a = 1.0f;

			renderer->draw_quad(t, 0, nullptr, color);
		}
	}

	renderer->end_pass();
}

Application* create_application(int argc, const char** argv) {
	ApplicationCreateInfo info;
	info.name = "editor2d";
	info.argc = argc;
	info.argv = argv;
	return new EditorApplication(info);
}
