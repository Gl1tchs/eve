#include "core/application.h"

#include "core/event_system.h"
#include "renderer/debug_renderer.h"

namespace eve2d {

Application::Application(const ApplicationCreateInfo& info) {
    WindowCreateInfo window_info{};
    window_info.title = info.name;
    m_window = std::make_shared<Window>(window_info);
    m_renderer = std::make_shared<Renderer>(m_window);

    event::subscribe<WindowCloseEvent>(
        [this](const auto& _event) { m_running = false; });
}

Application::~Application() {}

void Application::run() {
    while (m_running) {
        m_window->poll_events();

        m_renderer->begin_pass();

        debug::text_clear();
        debug::text_printf(0, 0, 0x0f, "Press F2 to show stats.");

        m_renderer->end_pass();
    }
}

}  // namespace eve2d