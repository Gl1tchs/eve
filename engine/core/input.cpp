#include "core/input.h"

#include "core/event_system.h"

inline static std::unordered_map<KeyCode, bool> key_press_states = {};
inline static std::unordered_map<KeyCode, bool> key_release_states = {};
inline static std::unordered_map<MouseCode, bool> mouse_press_states = {};
inline static std::unordered_map<MouseCode, bool> mouse_release_states = {};
inline static glm::vec2 mouse_position = glm::vec2(0.0f);
inline static glm::vec2 scroll_offset = glm::vec2(0.0f);

void Input::init() {
	event::subscribe<KeyPressEvent>([&](const KeyPressEvent& event) {
		key_press_states[event.key_code] = true;
		key_release_states[event.key_code] = false;
	});

	event::subscribe<KeyReleaseEvent>([&](const KeyReleaseEvent& event) {
		key_press_states[event.key_code] = false;
		key_release_states[event.key_code] = true;
	});

	event::subscribe<MousePressEvent>(
			[&](const MousePressEvent& event) {
				mouse_press_states[event.button_code] = true;
				mouse_release_states[event.button_code] = false;
			});

	event::subscribe<MouseReleaseEvent>(
			[&](const MouseReleaseEvent& event) {
				mouse_press_states[event.button_code] = false;
				mouse_release_states[event.button_code] = true;
			});

	event::subscribe<MouseMoveEvent>([&](const MouseMoveEvent& event) {
		mouse_position = event.position;
	});

	event::subscribe<MouseScrollEvent>([&](const MouseScrollEvent& event) {
		scroll_offset = event.offset;
	});
}

bool Input::is_key_pressed(KeyCode key) {
	const auto it = key_press_states.find(key);
	if (it != key_press_states.end()) {
		return it->second;
	}
	return false;
}

bool Input::is_key_released(KeyCode key) {
	const auto it = key_release_states.find(key);
	if (it != key_release_states.end()) {
		return it->second;
	}
	return false;
}

bool Input::is_mouse_pressed(MouseCode button) {
	const auto it = mouse_press_states.find(button);
	if (it != mouse_press_states.end()) {
		return it->second;
	}
	return false;
}

bool Input::is_mouse_released(MouseCode button) {
	const auto it = mouse_release_states.find(button);
	if (it != mouse_release_states.end()) {
		return it->second;
	}
	return false;
}

glm::vec2 Input::get_mouse_position() {
	return mouse_position;
}

glm::vec2 Input::get_scroll_offset() {
	const glm::vec2 old_scroll_offset = scroll_offset;
	scroll_offset = { 0, 0 };
	return old_scroll_offset;
}
