#ifndef TIMER_H
#define TIMER_H

class Timer final {
public:
	Timer();

	float elapsed_milliseconds();

	float elapsed_seconds();

	float delta_time();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock>
			last_frame_time;
};

#endif