#ifndef CAMERA_H
#define CAMERA_H

#include "core/transform.h"

struct OrthographicCamera {
	float aspect_ratio;
	float zoom_level = 1.0f;
	float near_clip = -1.0f;
	float far_clip = 1.0f;

	virtual ~OrthographicCamera() = default;

	[[nodiscard]] glm::mat4 projection_matrix() const;

	[[nodiscard]] glm::mat4 view_matrix(const Transform &transform);
};

#endif