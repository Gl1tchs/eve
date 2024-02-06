#include "core/application.h"

#include "core/event_system.h"
#include "core/timer.h"
#include "debug/assert.h"
#include "imgui/imgui_layer.h"
#include "renderer/font.h"
#include "renderer/renderer.h"
#include "scripting/script_engine.h"

Application* Application::s_instance = nullptr;

Application::Application(const ApplicationCreateInfo& info) {
	EVE_PROFILE_FUNCTION();

	EVE_ASSERT_ENGINE(!s_instance, "Only on instance can exists at a time!");
	s_instance = this;

	WindowCreateInfo window_info{};
	window_info.title = info.name;
	window = create_ref<Window>(window_info);

	event::subscribe<WindowCloseEvent>(
			[this](const auto& _event) { running = false; });

	renderer::init();
}

Application::~Application() {
	EVE_PROFILE_FUNCTION();

	// FIXME
	// 	perhaps there is a better way to
	//	delete static opengl resources.
	if (Font::s_default_font) {
		Font::s_default_font.reset();
	}

	ScriptEngine::shutdown();
	renderer::shutdown();
}

void Application::run() {
	EVE_PROFILE_FUNCTION();

	imgui_layer = new ImGuiLayer(window);

	{
		EVE_PROFILE_SCOPE("Application::_on_start");

		_on_start();
	}

	Timer timer;
	while (running) {
		_event_loop(timer.get_delta_time());
	}

	{
		EVE_PROFILE_SCOPE("Application::_on_destroy");

		_on_destroy();
	}

	delete imgui_layer;
}

void Application::enque_main_thread(MainThreadFunc func) {
	Application* app = get_instance();
	if (!app) {
		return;
	}

	std::scoped_lock<std::mutex> lock(app->main_thread_queue_mutex);
	app->main_thread_queue.push_back(func);
}

Application* Application::get_instance() {
	return s_instance;
}

void Application::_event_loop(float dt) {
	EVE_PROFILE_FUNCTION();

	window->poll_events();

	_process_main_thread_queue();

	{
		EVE_PROFILE_SCOPE("Application::_on_update");

		_on_update(dt);
	}

	imgui_layer->begin();
	{
		EVE_PROFILE_SCOPE("Application::_on_imgui_update");

		_on_imgui_update(dt);
	}
	imgui_layer->end();

	window->swap_buffers();
}

void Application::_process_main_thread_queue() {
	std::scoped_lock<std::mutex> lock(main_thread_queue_mutex);
	for (auto& func : main_thread_queue) {
		func();
	}

	main_thread_queue.clear();
}

void Application::quit() {
	running = false;
}

Ref<Window> Application::get_window() {
	return window;
}
