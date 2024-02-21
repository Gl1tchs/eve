#include "utils/platform_utils.h"

#ifdef EVE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

bool open_folder_in_explorer(const fs::path& path) {
#if defined(EVE_PLATFORM_LINUX) || defined(EVE_PLATFORM_BSD)
	std::string command = "xdg-open \"" + path.string() + "\"";
	std::system(command.c_str());
	return true;
#elif defined(EVE_PLATFORM_WINDOWS)
	ShellExecuteA(NULL, "open", path.string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
	return true;
#elif defined(EVE_PLATFORM_APPLE)
	std::string command = "open \"" + path.string() + "\"";
	std::system(command.c_str());
	return true;
#elif defined(EVE_PLATFORM_ANDROID)
	return false;
#else
	return false;
#endif
}
