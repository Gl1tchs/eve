#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/window.h"
#include "renderer/renderer.h"

struct ApplicationCreateInfo {
	const char* name;
	int argc;
	const char** argv;
};

// TODO try to make this function pointer
typedef std::function<void(void)> MainThreadFunc;

class Application {
public:
	Application(const ApplicationCreateInfo& info);
	~Application();

	void run();

	static void enque_main_thread(MainThreadFunc func);

	static Application* get_instance();

private:
	void _process_main_thread_queue();

protected:
	static Application* s_instance;

	Ref<Window> window;
	Ref<Renderer> renderer;

	inline virtual void _on_start() {}

	inline virtual void _on_update(float dt) {}

	inline virtual void _on_destroy() {}

private:
	bool running = true;

	std::vector<MainThreadFunc> main_thread_queue;
	std::mutex main_thread_queue_mutex;
};

#endif
