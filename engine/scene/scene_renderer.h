#ifndef SCENE_RENDERER_H
#define SCENE_RENDERER_H

#include "core/window.h"
#include "renderer/renderer.h"
#include "scene/editor_camera.h"

class FrameBuffer;
class PostProcessor;

enum class RenderBeheaviourTickFormat {
	BEFORE_RENDER,
	ON_RENDER,
	AFTER_RENDER
};

typedef std::function<void(const Ref<FrameBuffer>&)> RenderFunc;

class SceneRenderer {
public:
	SceneRenderer(Ref<Renderer> renderer);

	void render_runtime(float dt);

	void render_editor(float dt, Ref<EditorCamera>& editor_camera);

	void on_viewport_resize(glm::uvec2 size);

	void push_beheaviour(const RenderBeheaviourTickFormat format, const RenderFunc& function);

	uint32_t get_final_texture_id() const;

private:
	void _render_scene(const CameraData& data);

	void _post_process();

private:
	Ref<Renderer> renderer;
	Ref<FrameBuffer> frame_buffer;

	Ref<PostProcessor> post_processor;
	bool post_processed = false;

	glm::uvec2 viewport_size;

	std::vector<RenderFunc> before_render_functions;
	std::vector<RenderFunc> on_render_functions;
	std::vector<RenderFunc> after_render_functions;
};

#endif
