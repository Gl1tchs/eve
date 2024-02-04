#include "core/entrypoint.h"

#include "debug/log.h"

int main(int argc, const char** argv) {
	Logger::init("eve.log");

	// create application instance
	EVE_PROFILE_BEGIN_SESSION("startup", "profiler-startup.json");
	Application* _application = create_application(argc, argv);
	EVE_PROFILE_END_SESSION();

	// start event loop
	EVE_PROFILE_BEGIN_SESSION("runtime", "profiler-runtime.json");
	_application->run();
	EVE_PROFILE_END_SESSION();

	// cleanup
	EVE_PROFILE_BEGIN_SESSION("shutdown", "profiler-shutdown.json");
	delete _application;
	EVE_PROFILE_END_SESSION();

	return 0;
}
