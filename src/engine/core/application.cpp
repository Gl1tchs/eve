#include "core/application.h"

#include "core/assert.h"
#include "core/event_system.h"
#include "core/timer.h"

Application::Application(const ApplicationCreateInfo& info) {
	WindowCreateInfo window_info{};
	window_info.title = info.name;
	window = create_ref<Window>(window_info);

	event::subscribe<WindowCloseEvent>(
			[this](const auto& _event) { running = false; });

	renderer = create_ref<Renderer>();
}

Application::~Application() {}

void Application::run() {
	_on_start();

	Timer timer;
	while (running) {
		float dt = timer.get_delta_time();
		// printf("FPS: %.2f\n", 1000.0f / dt);

		window->poll_events();

		_on_update(dt);

		window->swap_buffers();
	}

	_on_destroy();
}
