#ifndef SCENE_RENDERER_H
#define SCENE_RENDERER_H

#include "core/window.h"
#include "renderer/renderer.h"
#include "scene/editor_camera.h"

class SceneRenderer {
public:
	SceneRenderer(Ref<Renderer> renderer);

	void render_runtime(float dt);

	void render_editor(float dt, Ref<EditorCamera>& editor_camera);

	void on_viewport_resize(glm::uvec2 size);

private:
	void _render_scene(const CameraData& data);

private:
	Ref<Renderer> renderer;
	glm::uvec2 viewport_size;
};

#endif