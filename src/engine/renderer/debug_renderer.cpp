#include "renderer/debug_renderer.h"

#include <bgfx/bgfx.h>

namespace eve2d {
namespace debug {

void text_clear() {
    bgfx::dbgTextClear();
}

void text_printf(uint16_t x, uint16_t y, uint8_t color, const char* fmt, ...) {
    bgfx::dbgTextPrintf(x, y, color, fmt);
}

}
}
