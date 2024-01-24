#pragma once

struct GLFWwindow;

namespace eve2d {

struct WindowCreateInfo {
    const char* title = "eve2d game";
    uint32_t w = 1280;
    uint32_t h = 768;
};

class Window {
  public:
    Window(WindowCreateInfo info);
    ~Window();

    void poll_events() const;

    bool is_open() const;

    glm::ivec2 size() const;

    void* native_window();

  private:
    void assign_event_delegates();

  private:
    GLFWwindow* m_window;
};

}  // namespace eve2d
