#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include "core/key_code.h"
#include "core/mouse_code.h"

struct KeyPressEvent {
	KeyCode key_code;
	bool is_repeat = false;
};

struct KeyReleaseEvent {
	KeyCode key_code;
};

struct KeyTypeEvent {
	KeyCode key_code;
};

struct MouseMoveEvent {
	glm::vec2 position;
};

struct MouseScrollEvent {
	glm::vec2 offset;
};

struct MousePressEvent {
	MouseCode button_code;
};

struct MouseReleaseEvent {
	MouseCode button_code;
};

struct WindowResizeEvent {
	glm::ivec2 size;
};

struct WindowCloseEvent {
};

template <typename T>
using event_callback = std::function<void(const T&)>;

namespace event {

template <typename T>
auto g_callbacks = std::vector<event_callback<T>>();

template <typename T>
inline void subscribe(const event_callback<T>& callback) {
	g_callbacks<T>.push_back(callback);
}

template <typename T>
inline void unsubscribe() {
	g_callbacks<T>.clear();
}

template <typename T>
inline void pop() {
	g_callbacks<T>.pop_back();
}

template <typename T>
inline void notify(T& event) {
	for (auto& callback : g_callbacks<T>) {
		callback(event);
	}
}

} // namespace event

#endif