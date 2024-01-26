#ifndef WINDOW_H
#define WINDOW_H

struct GLFWwindow;

struct WindowCreateInfo {
	const char* title = "eve2d game";
	uint32_t w = 1280;
	uint32_t h = 768;
};

class Window {
public:
	Window(WindowCreateInfo info);
	~Window();

	void swap_buffers() const;

	void poll_events() const;

	bool is_open() const;

	glm::ivec2 get_size() const;

	float get_aspect_ratio() const;

	void* get_native_window();

private:
	void _assign_event_delegates();

private:
	GLFWwindow* _window;
};

#endif