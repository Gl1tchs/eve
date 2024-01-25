#ifndef TIMER_H
#define TIMER_H

class Timer final {
public:
	Timer();

	[[nodiscard]] float elapsed_milliseconds();

	[[nodiscard]] float elapsed_seconds();

	[[nodiscard]] float delta_time();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock>
			_last_frame_time;
};

#endif