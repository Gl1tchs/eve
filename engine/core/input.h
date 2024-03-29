#ifndef INPUT_H
#define INPUT_H

#include "core/key_code.h"
#include "core/mouse_code.h"

struct Input {
	static void init();

	static bool is_key_pressed_once(KeyCode key);

	static bool is_key_pressed(KeyCode key);

	static bool is_key_released(KeyCode key);

	static bool is_mouse_pressed(MouseCode button);

	static bool is_mouse_released(MouseCode button);

	static glm::vec2 get_mouse_position();

	static glm::vec2 get_scroll_offset();
};

#endif
