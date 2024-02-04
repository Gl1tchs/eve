#ifndef CORE_MINIMAL_H
#define CORE_MINIMAL_H

#include "core/buffer.h"
#include "core/memory.h"
#include "debug/assert.h"
#include "debug/instrumentor.h"

#if __linux__
#define EVE_PLATFORM_LINUX 1
#elif __FreeBSD__
#define EVE_PLATFORM_BSD
#elif _WIN32 || _WIN64 || __CYGWIN__
#define EVE_PLATFORM_WINDOWS 1
#elif __APPLE__
#define EVE_PLATFORM_APPLE
#elif __ANDROID__
#define EVE_PLATFORM_ANDROID 1
#endif

#define BIND_FUNC(fn)                                           \
	[this](auto&&... args) -> decltype(auto) {                  \
		return this->fn(std::forward<decltype(args)>(args)...); \
	}

#endif
