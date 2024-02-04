#include "core/entrypoint.h"
#include "core/log.h"

int main(int argc, const char** argv) {
	Logger::init("eve.log");

	// create application instance
	Application* _application = create_application(argc, argv);

	// start event loop
	_application->run();

	// cleanup
	delete _application;

	return 0;
}
