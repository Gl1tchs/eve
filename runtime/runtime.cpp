#include "core/entrypoint.h"

#include "project/project.h"
#include "renderer/render_command.h"
#include "renderer/shader.h"
#include "renderer/vertex_array.h"
#include "scene/scene_manager.h"
#include "scene/scene_renderer.h"
#include "scripting/script_engine.h"

class RuntimeApplication : public Application {
public:
	inline RuntimeApplication(const ApplicationCreateInfo& info, const std::string& project_path) :
			Application(info), project_path(project_path) {
		scene_renderer = create_ref<SceneRenderer>();

		EVE_ASSERT_ENGINE(Project::load(project_path), "Unable to find specified project file.");
	}

	virtual ~RuntimeApplication() = default;

protected:
	inline void _on_start() override {
		ScriptEngine::init();

		// load the first scene
		EVE_ASSERT_ENGINE(SceneManager::load_scene(Project::get_starting_scene_path()));

		// TODO get this from export settings
		window->set_title(Project::get_name());

		// screen data
		screen_vertex_array = create_ref<VertexArray>();
		screen_shader = create_ref<Shader>("shaders/screen.vert.spv", "shaders/screen.frag.spv");

		SceneManager::get_active()->start();
	}

	inline void _on_update(float dt) override {
		if (window_size != window->get_size()) {
			window_size = window->get_size();
			scene_renderer->on_viewport_resize({ window_size.x, window_size.y });
		}

		Ref<Scene> scene = SceneManager::get_active();
		//? TODO maybe not do this
		EVE_ASSERT_ENGINE(scene, "Unable to find any scene aborting!");

		scene->update(dt);
		scene_renderer->render_runtime(dt);

		RenderCommand::set_depth_testing(false);

		RenderCommand::set_clear_color(COLOR_BLACK);
		RenderCommand::clear(BUFFER_BITS_COLOR | BUFFER_BITS_DEPTH);

		screen_shader->bind();
		RenderCommand::bind_texture(scene_renderer->get_final_texture_id());

		RenderCommand::draw_arrays(screen_vertex_array, 6);

		RenderCommand::set_depth_testing(true);
	}

	inline void _on_imgui_update(float dt) override {
		//! TODO debug information
	}

	inline void _on_destroy() override {
		SceneManager::get_active()->start();
	}

private:
	std::string project_path;

	Ref<SceneRenderer> scene_renderer;

	// screen data
	Ref<VertexArray> screen_vertex_array;
	Ref<Shader> screen_shader;

	glm::ivec2 window_size{};
};

// Application entrypoint
Application* create_application(int argc, const char** argv) {
	// TODO custumize this before build
	// TODO window settings
	// TODO icon
	ApplicationCreateInfo info{};
	info.name = "runtime";
	info.argc = argc;
	info.argv = argv;
	return new RuntimeApplication(info, "sample/sample.eve");
}
