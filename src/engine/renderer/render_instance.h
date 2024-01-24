#pragma once

#include "core/window.h"

namespace eve2d {

class Renderer {
  public:
    Renderer(std::shared_ptr<Window> window);

    void begin_pass();

    void end_pass();

  private:
    std::shared_ptr<Window> m_window;
    glm::ivec2 m_size{};

    bool m_show_stats = false;
};

}  // namespace eve2d
