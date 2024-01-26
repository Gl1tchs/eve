#ifndef CAMERA_H
#define CAMERA_H

#include "scene/transform.h"

struct OrthographicCamera {
	float aspect_ratio = 1.77f;
	float zoom_level = 1.0f;
	float near_clip = -1.0f;
	float far_clip = 1.0f;

	virtual ~OrthographicCamera() = default;

	glm::mat4 get_projection_matrix() const;

	glm::mat4 get_view_matrix(const TransformComponent& transform);
};

#endif
