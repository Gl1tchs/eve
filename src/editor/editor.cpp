#include "core/entrypoint.h"

namespace eve2d {

Application* create_application(int argc, const char** argv) {
    ApplicationCreateInfo info;
    info.name = "editor2d";
    info.argc = argc;
    info.argv = argv;
    return new Application(info);
}

}  // namespace eve2d
