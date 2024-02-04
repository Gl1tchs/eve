#ifndef ASSERT_H
#define ASSERT_H

#include "debug/log.h"

#if _WIN32
#define DEBUGBREAK() __debugbreak()
#elif __linux__
#include <signal.h>
#define DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif

#define EVE_EXPAND_MACRO(x) x
#define EVE_STRINGIFY_MACRO(x) #x

#define EVE_INTERNAL_ASSERT_IMPL(type, check, msg, ...) \
	if (!(check)) {                                     \
		EVE_LOG_##type##_FATAL(msg, __VA_ARGS__);       \
		DEBUGBREAK();                                   \
	}

#define EVE_INTERNAL_ASSERT_WITH_MSG(type, check, ...) \
	EVE_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {}", __VA_ARGS__)

#define EVE_INTERNAL_ASSERT_NO_MSG(type, check)                                        \
	EVE_INTERNAL_ASSERT_IMPL(                                                          \
			type, check, "Assertion '{}' failed at {}:{}", EVE_STRINGIFY_MACRO(check), \
			__FILE__, __LINE__)

#define EVE_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro

#define EVE_INTERNAL_ASSERT_GET_MACRO(...)               \
	EVE_EXPAND_MACRO(EVE_INTERNAL_ASSERT_GET_MACRO_NAME( \
			__VA_ARGS__, EVE_INTERNAL_ASSERT_WITH_MSG,   \
			EVE_INTERNAL_ASSERT_NO_MSG))

#define EVE_ASSERT_ENGINE(...) \
	EVE_EXPAND_MACRO(EVE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(ENGINE, __VA_ARGS__))

#define EVE_ASSERT_CLIENT(...) \
	EVE_EXPAND_MACRO(EVE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(CLIENT, __VA_ARGS__))

#endif
