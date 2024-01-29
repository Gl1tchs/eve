#ifndef CAMERA_H
#define CAMERA_H

#include "scene/transform.h"

struct CameraData final {
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec2 position;
};

struct OrthographicCamera {
	float aspect_ratio = 1.77f;
	float zoom_level = 1.0f;
	float near_clip = -1.0f;
	float far_clip = 1.0f;

	virtual ~OrthographicCamera() = default;

	glm::mat4 get_projection_matrix() const;

	glm::mat4 get_view_matrix(const TransformComponent& transform) const;
};

#endif
