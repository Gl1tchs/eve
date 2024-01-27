#ifndef IMGUI_LAYER
#define IMGUI_LAYER

class Window;

struct ImGuiSettings {
	bool enable_viewports = true;
	bool enable_docking = true;
};

class ImGuiLayer {
public:
	ImGuiLayer(Ref<Window> window);
	~ImGuiLayer();

	void begin();

	void end();

	ImGuiSettings& get_settings() { return settings; }

private:
	ImGuiSettings settings{};

	Ref<Window> window;
};

#endif
