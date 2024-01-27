#include "core/application.h"

#include "core/assert.h"
#include "core/event_system.h"
#include "core/timer.h"
#include "imgui/imgui_layer.h"
#include "renderer/font.h"

Application* Application::s_instance = nullptr;

Application::Application(const ApplicationCreateInfo& info) {
	EVE_ASSERT_ENGINE(!s_instance, "Only on instance can exists at a time!");
	s_instance = this;

	WindowCreateInfo window_info{};
	window_info.title = info.name;
	window = create_ref<Window>(window_info);

	event::subscribe<WindowCloseEvent>(
			[this](const auto& _event) { running = false; });

	renderer = create_ref<Renderer>();
}

Application::~Application() {
	// FIXME
	// 	perhaps there is a better way to
	//	delete static opengl resources.
	if (Font::s_default_font) {
		Font::s_default_font.reset();
	}
}

void Application::run() {
	ImGuiLayer imgui_layer(window);

	_on_start();

	Timer timer;
	while (running) {
		float dt = timer.get_delta_time();

		window->poll_events();

		_process_main_thread_queue();

		_on_update(dt);

		imgui_layer.begin();
		{
			_on_imgui_update(dt);
		}
		imgui_layer.end();

		window->swap_buffers();
	}

	_on_destroy();
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

void Application::_process_main_thread_queue() {
	std::scoped_lock<std::mutex> lock(main_thread_queue_mutex);
	for (auto& func : main_thread_queue) {
		func();
	}

	main_thread_queue.clear();
}
