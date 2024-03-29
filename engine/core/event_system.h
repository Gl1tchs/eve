#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include "core/key_code.h"
#include "core/mouse_code.h"

struct KeyPressEvent {
	KeyCode key_code;
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

struct WindowCloseEvent {};

template <typename T> using EventCallbackFunc = std::function<void(const T&)>;

namespace event {

template <typename T>
inline auto g_callbacks = std::vector<EventCallbackFunc<T>>();

template <typename T>
inline void subscribe(const EventCallbackFunc<T>& callback) {
	g_callbacks<T>.push_back(callback);
}

template <typename T> inline void unsubscribe() { g_callbacks<T>.clear(); }

template <typename T> inline void pop() { g_callbacks<T>.pop_back(); }

template <typename T> inline void notify(T& event) {
	for (auto& callback : g_callbacks<T>) {
		callback(event);
	}
}

} // namespace event

#endif
