#ifndef EDITOR_H
#define EDITOR_H

#include "core/application.h"

#include "renderer/frame_buffer.h"
#include "scene/editor_camera.h"
#include "scene/scene_renderer.h"

class EditorApplication : public Application {
public:
	EditorApplication(const ApplicationCreateInfo& info);
	~EditorApplication() = default;

protected:
	void _on_start() override;

	void _on_update(float dt) override;

	void _on_imgui_update(float dt) override;

	void _on_destroy() override;

private:
	Ref<SceneRenderer> scene_renderer;
	Ref<EditorCamera> editor_camera;

	Ref<FrameBuffer> frame_buffer;
	int viewport_width = 0;
	int viewport_height = 0;
	glm::vec2 viewport_bounds[2];
};

#endif
