#include "scene/editor_camera.h"

#include "core/input.h"
#include <algorithm>

EditorCamera::EditorCamera() :
		OrthographicCamera(), last_mouse_pos(0, 0) {
	reset_transform();

	last_mouse_pos = Input::get_mouse_position();
}

glm::mat4 EditorCamera::get_view_matrix() const {
	return OrthographicCamera::get_view_matrix(transform);
}

void EditorCamera::update(float dt) {
	if (!Input::is_mouse_pressed(MouseCode::RIGHT)) {
		last_mouse_pos = Input::get_mouse_position();
		return;
	}

	glm::vec2 mouse_delta = Input::get_mouse_position() - last_mouse_pos;

	transform.local_position.x += mouse_delta.x * sensitivity * zoom_level * dt;
	transform.local_position.y -= mouse_delta.y * sensitivity * zoom_level * dt;

	zoom_level -= Input::get_scroll_offset().y * scroll_speed;
	zoom_level = std::max(zoom_level, 0.5f);

	// store last mouse pos to prevent instant rotations
	last_mouse_pos = Input::get_mouse_position();
}

void EditorCamera::reset_transform() {
	transform.local_position = VEC3_ZERO;
	transform.local_rotation = VEC3_ZERO;
	transform.local_scale = VEC3_ONE;
}

Transform& EditorCamera::get_transform() { return transform; }

const Transform& EditorCamera::get_transform() const { return transform; }

void EditorCamera::set_speed(float value) { sensitivity = value; }

float EditorCamera::get_speed() const { return sensitivity; }

void EditorCamera::set_sensitivity(float value) { scroll_speed = value; }

float EditorCamera::get_sensitivity() const { return scroll_speed; }
