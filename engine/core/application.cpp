#include "core/application.h"

#include "core/assert.h"
#include "core/event_system.h"
#include "core/timer.h"
#include "renderer/font.h"

Application::Application(const ApplicationCreateInfo& info) {
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
	_on_start();

	Timer timer;
	while (running) {
		float dt = timer.get_delta_time();
		// EVE_LOG_ENGINE_TRACE("FPS: {}", 1000.0f / dt);

		window->poll_events();

		_on_update(dt);

		window->swap_buffers();
	}

	_on_destroy();
}
