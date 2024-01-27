#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include "scene/transform.h"
#include "renderer/camera.h"

class EditorCamera : public OrthographicCamera {
public:
	EditorCamera();

	glm::mat4 get_view_matrix();

	void update(float ds);

	void reset_transform();

	TransformComponent& get_transform();
	const TransformComponent& get_transform() const;

	void set_speed(float value);
	float get_speed() const;

	void set_sensitivity(float value);
	float get_sensitivity() const;

private:
	TransformComponent transform;

	float sensitivity{ 0.25 };
	float scroll_speed{ 0.5f };

	glm::vec2 last_mouse_pos;
};

#endif
