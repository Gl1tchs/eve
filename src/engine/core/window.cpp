#include "core/window.h"

#include "core/assert.h"
#include "core/event_system.h"
#include "core/input.h"
#include "core/key_code.h"
#include "core/mouse_code.h"

#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

Window::Window(WindowCreateInfo info) {
	EVE_ASSERT(glfwInit());

#if _DEBUG
	glfwSetErrorCallback(glfw_error_callback);
#endif

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(info.w, info.h, info.title, nullptr, nullptr);
	EVE_ASSERT(_window);

	//! TODO remove this if any other grapichs api implemented.
	glfwMakeContextCurrent(_window);

	// initialize event system
	_assign_event_delegates();

	// initialize input
	Input::init();
}

Window::~Window() {
	glfwDestroyWindow(_window);
	glfwTerminate();
}

void Window::swap_buffers() const {
	glfwSwapBuffers(_window);
}

void Window::poll_events() const {
	glfwPollEvents();
}

bool Window::is_open() const {
	return !glfwWindowShouldClose(_window);
}

glm::ivec2 Window::get_size() const {
	glm::ivec2 s{};
	glfwGetWindowSize(_window, &s.x, &s.y);
	return s;
}

float Window::get_aspect_ratio() const {
	const glm::ivec2 s = get_size();
	return static_cast<float>(s.x) / static_cast<float>(s.y);
}

void* Window::get_native_window() {
	return _window;
}

void Window::_assign_event_delegates() {
	glfwSetWindowSizeCallback(_window,
			[](GLFWwindow* window, int width, int height) {
				WindowResizeEvent resize_event;
				resize_event.size = { width, height };
				event::notify(resize_event);
			});

	glfwSetWindowCloseCallback(_window, [](GLFWwindow* window) {
		WindowCloseEvent close_event{};
		event::notify<WindowCloseEvent>(close_event);
	});

	glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
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

	glfwSetCharCallback(_window, [](GLFWwindow* window, unsigned int keycode) {
		KeyTypeEvent type_event;
		type_event.key_code = static_cast<KeyCode>(keycode);
		event::notify(type_event);
	});

	glfwSetMouseButtonCallback(
			_window, [](GLFWwindow* window, int button, int action, int mods) {
				switch (action) {
					case GLFW_PRESS: {
						MousePressEvent mouse_event;
						mouse_event.button_code = static_cast<MouseCode>(button);
						event::notify(mouse_event);
						break;
					}
					case GLFW_RELEASE: {
						MouseReleaseEvent mouse_event;
						mouse_event.button_code = static_cast<MouseCode>(button);
						event::notify(mouse_event);
						break;
					}
					default: {
						break;
					}
				}
			});

	glfwSetCursorPosCallback(
			_window,
			[](GLFWwindow* window, const double x_pos, const double y_pos) {
				MouseMoveEvent move_event;
				move_event.position = { static_cast<float>(x_pos),
					static_cast<float>(y_pos) };
				event::notify(move_event);
			});

	glfwSetScrollCallback(
			_window,
			[](GLFWwindow* window, const double x_offset, const double y_offset) {
				MouseScrollEvent scroll_event;
				scroll_event.offset = { static_cast<float>(x_offset),
					static_cast<float>(y_offset) };
				event::notify(scroll_event);
			});
}
