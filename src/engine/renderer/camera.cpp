#include "renderer/camera.h"

glm::mat4 OrthographicCamera::projection_matrix() const {
	return glm::ortho(-aspect_ratio * zoom_level, aspect_ratio * zoom_level,
			-zoom_level, zoom_level, near_clip, far_clip);
}

glm::mat4 OrthographicCamera::view_matrix(const Transform &transform) {
	return glm::inverse(transform.transform_matrix());
}
