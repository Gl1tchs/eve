#include "core/timer.h"

Timer::Timer() :
		_last_frame_time(std::chrono::high_resolution_clock::now()) {}

float Timer::elapsed_milliseconds() {
	auto current_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::chrono::milliseconds::period>
			elapsed_time = current_time - _last_frame_time;
	return elapsed_time.count();
}

float Timer::elapsed_seconds() {
	auto current_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsed_time =
			current_time - _last_frame_time;
	return elapsed_time.count();
}

float Timer::delta_time() {
	auto current_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> delta_time = current_time - _last_frame_time;
	_last_frame_time = current_time;
	return delta_time.count();
}
