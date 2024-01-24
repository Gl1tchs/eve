#include "core/window.h"

#include <GLFW/glfw3.h>
#include <bgfx/platform.h>

#include "core/event_system.h"
#include "core/key_code.h"
#include "core/log.h"
#include "core/mouse_code.h"

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

namespace eve2d {

Window::Window(WindowCreateInfo info) {
    glfwSetErrorCallback(glfw_error_callback);
    EVE_ASSERT(glfwInit());

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(info.w, info.h, info.title, nullptr, nullptr);
    EVE_ASSERT(m_window);

    // initialize event system
    assign_event_delegates();
}

Window::~Window() {
    glfwTerminate();
}

void Window::poll_events() const {
    glfwPollEvents();
}

bool Window::is_open() const {
    return !glfwWindowShouldClose(m_window);
}

glm::ivec2 Window::size() const {
    glm::ivec2 s{};
    glfwGetWindowSize(m_window, &s.x, &s.y);
    return s;
}

void* Window::native_window() {
    return m_window;
}

void Window::assign_event_delegates() {
    glfwSetWindowSizeCallback(m_window,
                              [](GLFWwindow* window, int width, int height) {
                                  WindowResizeEvent resize_event;
                                  resize_event.size = {width, height};
                                  event::notify(resize_event);
                              });

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
        WindowCloseEvent close_event{};
        event::notify<WindowCloseEvent>(close_event);
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode,
                                    int action, int mods) {
        switch (action) {
            case GLFW_PRESS: {
                KeyPressEvent key_event;
                key_event.key_code = static_cast<KeyCode>(key);
                event::notify(key_event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleaseEvent key_event;
                key_event.key_code = static_cast<KeyCode>(key);
                event::notify(key_event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressEvent key_event;
                key_event.key_code = static_cast<KeyCode>(key);
                key_event.is_repeat = true;
                event::notify(key_event);
                break;
            }
            default: {
                break;
            }
        }
    });

    glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode) {
        KeyTypeEvent type_event;
        type_event.key_code = static_cast<KeyCode>(keycode);
        event::notify(type_event);
    });

    glfwSetMouseButtonCallback(
        m_window, [](GLFWwindow* window, int button, int action, int mods) {
            switch (action) {
                case GLFW_PRESS: {
                    MousePressEvent mouse_event;
                    mouse_event.code = static_cast<MouseCode>(button);
                    event::notify(mouse_event);
                    break;
                }
                case GLFW_RELEASE: {
                    MouseReleaseEvent mouse_event;
                    mouse_event.code = static_cast<MouseCode>(button);
                    event::notify(mouse_event);
                    break;
                }
                default: {
                    break;
                }
            }
        });

    glfwSetCursorPosCallback(
        m_window,
        [](GLFWwindow* window, const double x_pos, const double y_pos) {
            MouseMoveEvent move_event;
            move_event.position = {static_cast<float>(x_pos),
                                   static_cast<float>(y_pos)};
            event::notify(move_event);
        });

    glfwSetScrollCallback(
        m_window,
        [](GLFWwindow* window, const double x_offset, const double y_offset) {
            MouseScrollEvent scroll_event;
            scroll_event.offset = {static_cast<float>(x_offset),
                                   static_cast<float>(y_offset)};
            event::notify(scroll_event);
        });
}

}  // namespace eve2d