#ifndef EDITOR_H
#define EDITOR_H

#include "core/application.h"

#include "renderer/camera.h"
#include "scene/editor_camera.h"
#include "scene/scene_renderer.h"

class EditorApplication : public Application {
public:
	EditorApplication(const ApplicationCreateInfo& info);
	~EditorApplication() = default;

protected:
	void _on_start() override;

	void _on_update(float dt) override;

	void _on_destroy() override;

private:
	Ref<SceneRenderer> scene_renderer;
	Ref<EditorCamera> editor_camera;
};

#endif
