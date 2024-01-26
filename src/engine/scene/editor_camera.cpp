#include "scene/editor_camera.h"

#include "core/input.h"

EditorCamera::EditorCamera() :
		OrthographicCamera() {
	reset_transform();

	last_mouse_pos = Input::get_mouse_position();
}

glm::mat4 EditorCamera::get_view_matrix() {
	return OrthographicCamera::get_view_matrix(transform);
}

void EditorCamera::update(float dt) {
	if (!Input::is_mouse_pressed(MouseCode::RIGHT)) {
		last_mouse_pos = Input::get_mouse_position();
		return;
	}

	glm::vec2 mouse_delta = Input::get_mouse_position() - last_mouse_pos;

	transform.local_position.x += mouse_delta.x * speed * dt;
	transform.local_position.y -= mouse_delta.y * speed * dt;

	zoom_level -= Input::get_scroll_offset().y * sensitivity * dt;
	if (zoom_level <= 0.1f) {
		zoom_level = 0.1f;
	}

	// store last mouse pos to prevent instant rotations
	last_mouse_pos = Input::get_mouse_position();
}

void EditorCamera::reset_transform() {
	transform.local_position = VEC3_ZERO;
	transform.local_rotation = VEC3_ZERO;
	transform.local_scale = VEC3_ONE;
}

TransformComponent& EditorCamera::get_transform() { return transform; }

const TransformComponent& EditorCamera::get_transform() const { return transform; }

void EditorCamera::set_speed(float value) { speed = value; }

float EditorCamera::get_speed() const { return speed; }

void EditorCamera::set_sensitivity(float value) { sensitivity = value; }

float EditorCamera::get_sensitivity() const { return sensitivity; }