#include "core/entrypoint.h"

int main(int argc, const char** argv) {
    // create application instance
    eve2d::Application* m_application = eve2d::create_application(argc, argv);

    // start event loop
    m_application->run();

    // cleanup
    delete m_application;
    
    return 0;
}
