#ifndef INPUT_H
#define INPUT_H

#include "core/key_code.h"
#include "core/mouse_code.h"

struct Input {
	static void init();

	static bool is_key_pressed(KeyCode key);

	static bool is_key_released(KeyCode key);

	static bool is_mouse_pressed(MouseCode button);

	static bool is_mouse_released(MouseCode button);

	static glm::vec2 mouse_position();

	static glm::vec2 scroll_offset();

private:
	static std::unordered_map<KeyCode, bool> _key_press_states;
	static std::unordered_map<KeyCode, bool> _key_release_states;

	static std::unordered_map<MouseCode, bool> _mouse_press_states;
	static std::unordered_map<MouseCode, bool> _mouse_release_states;

	static glm::vec2 _mouse_position;
	static glm::vec2 _scroll_offset;
};

#endif
