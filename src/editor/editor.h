#ifndef EDITOR_H
#define EDITOR_H

#include "core/application.h"

#include "renderer/camera.h"

class EditorApplication : public Application {
public:
	EditorApplication(const ApplicationCreateInfo& info);
	~EditorApplication();

protected:
	void on_start() override;

	void on_update(float dt) override;

	void on_destroy() override;

private:
	void on_render();

private:
	OrthographicCamera _camera;
	Transform _camera_transform;

	glm::vec2 _mouse_pos;

	glm::vec2 _scroll_offset;
	const float _scroll_sensitivity = 40.0f;
};

#endif