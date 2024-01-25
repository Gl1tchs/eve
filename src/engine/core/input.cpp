#include "core/input.h"

#include "core/event_system.h"

std::unordered_map<KeyCode, bool> Input::_key_press_states = {};
std::unordered_map<KeyCode, bool> Input::_key_release_states = {};
std::unordered_map<MouseCode, bool> Input::_mouse_press_states = {};
std::unordered_map<MouseCode, bool> Input::_mouse_release_states = {};
glm::vec2 Input::_mouse_position = glm::vec2(0.0f);
glm::vec2 Input::_scroll_offset = glm::vec2(0.0f);

void Input::init() {
	event::subscribe<KeyPressEvent>([&](const KeyPressEvent& event) {
		_key_press_states[event.key_code] = true;
		_key_release_states[event.key_code] = false;
	});

	event::subscribe<KeyReleaseEvent>([&](const KeyReleaseEvent& event) {
		_key_press_states[event.key_code] = false;
		_key_release_states[event.key_code] = true;
	});

	event::subscribe<MousePressEvent>(
			[&](const MousePressEvent& event) {
				_mouse_press_states[event.button_code] = true;
				_mouse_release_states[event.button_code] = false;
			});

	event::subscribe<MouseReleaseEvent>(
			[&](const MouseReleaseEvent& event) {
				_mouse_press_states[event.button_code] = false;
				_mouse_release_states[event.button_code] = true;
			});

	event::subscribe<MouseMoveEvent>([&](const MouseMoveEvent& event) {
		_mouse_position = event.position;
	});

	event::subscribe<MouseScrollEvent>([&](const MouseScrollEvent& event) {
		_scroll_offset = event.offset;
	});
}

bool Input::is_key_pressed(KeyCode key) {
	const auto it = _key_press_states.find(key);
	if (it != _key_press_states.end()) {
		return it->second;
	}
	return false;
}

bool Input::is_key_released(KeyCode key) {
	const auto it = _key_release_states.find(key);
	if (it != _key_release_states.end()) {
		return it->second;
	}
	return false;
}

bool Input::is_mouse_pressed(MouseCode button) {
	const auto it = _mouse_press_states.find(button);
	if (it != _mouse_press_states.end()) {
		return it->second;
	}
	return false;
}

bool Input::is_mouse_released(MouseCode button) {
	const auto it = _mouse_release_states.find(button);
	if (it != _mouse_release_states.end()) {
		return it->second;
	}
	return false;
}

glm::vec2 Input::mouse_position() {
	return _mouse_position;
}

glm::vec2 Input::scroll_offset() {
	const glm::vec2 scroll_offset = _scroll_offset;
	_scroll_offset = { 0, 0 };
	return scroll_offset;
}
