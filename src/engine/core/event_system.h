#pragma once

#include "core/key_code.h"
#include "core/mouse_code.h"

namespace eve2d {

enum class EventType {
    NONE = 0,
    KEY_PRESS,
    KEY_RELEASE,
    KEY_TYPE,
    MOUSE_PRESS,
    MOUSE_RELEASE,
    MOUSE_MOVE,
    MOUSE_SCROLL,
    WINDOW_RESIZE,
    WINDOW_CLOSE
};

struct KeyPressEvent {
    const EventType type = EventType::KEY_PRESS;
    KeyCode key_code;
    bool is_repeat = false;
};

struct KeyReleaseEvent {
    const EventType type = EventType::KEY_RELEASE;
    KeyCode key_code;
};

struct KeyTypeEvent {
    const EventType type = EventType::KEY_TYPE;
    KeyCode key_code;
};

struct MouseMoveEvent {
    const EventType type = EventType::MOUSE_MOVE;
    glm::vec2 position;
};

struct MouseScrollEvent {
    const EventType type = EventType::MOUSE_SCROLL;
    glm::vec2 offset;
};

struct MousePressEvent {
    const EventType type = EventType::MOUSE_PRESS;
    MouseCode code;
};

struct MouseReleaseEvent {
    const EventType type = EventType::MOUSE_RELEASE;
    MouseCode code;
};

struct WindowResizeEvent {
    const EventType type = EventType::WINDOW_RESIZE;
    glm::ivec2 size;
};

struct WindowCloseEvent {
    const EventType type = EventType::WINDOW_CLOSE;
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

}  // namespace event

}  // namespace eve2d
