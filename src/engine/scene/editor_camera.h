#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include "core/transform.h"
#include "renderer/camera.h"

class EditorCamera : public OrthographicCamera {
public:
	EditorCamera();

	glm::mat4 get_view_matrix();

	void update(float ds);

	void reset_transform();

	Transform& get_transform();
	const Transform& get_transform() const;

	void set_speed(float value);
	float get_speed() const;

	void set_sensitivity(float value);
	float get_sensitivity() const;

private:
	Transform transform;

	float speed{ 1.0f };
	float sensitivity{ 10.0f };

	glm::vec2 last_mouse_pos;
};

#endif