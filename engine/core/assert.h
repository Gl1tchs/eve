#ifndef ASSERT_H
#define ASSERT_H

#include <cstdint>
#include <string>
#include <string_view>

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

#define EVE_INTERNAL_ASSERT_IMPL(check, msg, ...) \
	if (!(check)) {                               \
		printf(msg, __VA_ARGS__);                 \
		DEBUGBREAK();                             \
	}

#define EVE_INTERNAL_ASSERT_WITH_MSG(check, ...) \
	EVE_INTERNAL_ASSERT_IMPL(check, "Assertion failed: %s", __VA_ARGS__)

#define EVE_INTERNAL_ASSERT_NO_MSG(check)                                        \
	EVE_INTERNAL_ASSERT_IMPL(                                                    \
			check, "Assertion '%s' failed at %s:%s", EVE_STRINGIFY_MACRO(check), \
			__FILE__, __LINE__)

#define EVE_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro

#define EVE_INTERNAL_ASSERT_GET_MACRO(...)               \
	EVE_EXPAND_MACRO(EVE_INTERNAL_ASSERT_GET_MACRO_NAME( \
			__VA_ARGS__, EVE_INTERNAL_ASSERT_WITH_MSG,   \
			EVE_INTERNAL_ASSERT_NO_MSG))

#define EVE_ASSERT(...) \
	EVE_EXPAND_MACRO(EVE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__))

#endif