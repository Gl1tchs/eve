#include "renderer/camera.h"

glm::mat4 OrthographicCamera::get_projection_matrix() const {
	return glm::ortho(-aspect_ratio * zoom_level, aspect_ratio * zoom_level,
			-zoom_level, zoom_level, near_clip, far_clip);
}

glm::mat4 OrthographicCamera::get_view_matrix(const Transform& transform) const {
	return glm::inverse(transform.get_transform_matrix());
}
