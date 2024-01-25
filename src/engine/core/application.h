#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/window.h"
#include "renderer/renderer.h"

struct ApplicationCreateInfo {
	const char* name;
	int argc;
	const char** argv;
};

class Application {
public:
	Application(const ApplicationCreateInfo& info);
	~Application();

	void run();

protected:
	Ref<Window> window;
	Ref<Renderer> renderer;

	inline virtual void on_start() {}

	inline virtual void on_update(float dt) {}

	inline virtual void on_destroy() {}

private:
	bool _running = true;
};

#endif