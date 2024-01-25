#include "core/application.h"

#include "core/assert.h"
#include "core/event_system.h"
#include "core/timer.h"

Application::Application(const ApplicationCreateInfo& info) {
	WindowCreateInfo window_info{};
	window_info.title = info.name;
	window = create_ref<Window>(window_info);

	event::subscribe<WindowCloseEvent>(
			[this](const auto& _event) { _running = false; });

	renderer = create_ref<Renderer>();
}

Application::~Application() {}

void Application::run() {
	on_start();

	Timer timer;
	while (_running) {
		float dt = timer.delta_time();
		printf("%.2f\n", 1000 / dt);

		window->poll_events();

		on_update(dt);

		window->swap_buffers();
	}

	on_destroy();
}
