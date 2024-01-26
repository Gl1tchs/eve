#ifndef TIMER_H
#define TIMER_H

class Timer final {
public:
	Timer();

	float get_elapsed_milliseconds();

	float get_elapsed_seconds();

	float get_delta_time();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock>
			last_frame_time;
};

#endif