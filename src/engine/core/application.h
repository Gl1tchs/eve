#pragma once

#include "core/window.h"
#include "renderer/render_instance.h"

namespace eve2d {

struct ApplicationCreateInfo {
    const char* name;
    int argc;
    const char** argv;
};

class Application {
public:
    Application(const ApplicationCreateInfo& info);
    ~Application();

    void run();

private:
    std::shared_ptr<Window> m_window;
    std::shared_ptr<Renderer> m_renderer;

    bool m_running = true;
};  

}
