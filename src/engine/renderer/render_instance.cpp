#include "renderer/render_instance.h"

#include <GLFW/glfw3.h>
#include <X11/X.h>
#include <bgfx/bgfx.h>

#include "core/core_minimal.h"
#include "core/event_system.h"

#if EVE_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif EVE_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif EVE_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

namespace eve2d {

Renderer::Renderer(std::shared_ptr<Window> window) : m_window(window) {
    bgfx::Init init;

    GLFWwindow* native_window = (GLFWwindow*)window->native_window();
#if EVE_PLATFORM_LINUX
    init.platformData.ndt = glfwGetX11Display();
    init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(native_window);
#elif EVE_PLATFORM_OSX
    init.platformData.nwh = glfwGetCocoaWindow(native_window);
#elif EVE_PLATFORM_WINDOWS
    init.platformData.nwh = glfwGetWin32Window(native_window);
#endif

    const glm::ivec2 size = m_window->size();
    init.resolution.width = (uint32_t)size.x;
    init.resolution.height = (uint32_t)size.y;
    init.resolution.reset = BGFX_RESET_VSYNC;

    // initialize bgfx
    EVE_ASSERT(bgfx::init(init));

    event::subscribe<KeyPressEvent>([this](const KeyPressEvent& event) {
        // press F2 to toggle debug stats
        if (event.key_code == KeyCode::F2) {
            m_show_stats = !m_show_stats;
        }
    });
}

void Renderer::begin_pass() {
    constexpr bgfx::ViewId kClearView = 0;

    glm::ivec2 old_size = m_size;
    m_size = m_window->size();
    if (m_size != old_size) {
        bgfx::reset((uint32_t)m_size.x, (uint32_t)m_size.y, BGFX_RESET_VSYNC);
        bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
    }

    bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
    bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

    // This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
    bgfx::touch(kClearView);
}

void Renderer::end_pass() {
    // set debug mode
    bgfx::setDebug(m_show_stats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
    bgfx::frame();
}

}  // namespace eve2d
