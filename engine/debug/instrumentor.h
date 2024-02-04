#ifndef INSTRUMENTOR_H
#define INSTRUMENTOR_H

struct ProfileResult {
	std::string name;
	uint64_t start;
	uint64_t end;
	std::thread::id thread_id;
};

struct InstrumentationSession {
	std::string name;
};

class Instrumentor {
public:
	Instrumentor();

	void begin_session(const std::string& name, const std::string& file_path = "profile-result.json");

	void end_session();

	void write_profile(const ProfileResult& result);

	static Instrumentor& get_instance();

private:
	void _write_header();

	void _write_footer();

private:
	InstrumentationSession* current_session;
	std::ofstream output_stream;
	int profile_count;
	std::mutex file_mutex;
};

class InstrumentationTimer {
public:
	InstrumentationTimer(const std::string& name);

	~InstrumentationTimer();

	void stop();

private:
	std::string name;
	std::chrono::time_point<std::chrono::high_resolution_clock> start_point;
	bool stopped;
};

#if EVE_DEBUG
#define EVE_ENABLE_PROFILING 1
#else
#define EVE_ENABLE_PROFILING 0
#endif

#if EVE_ENABLE_PROFILING

#define EVE_PROFILE_BEGIN_SESSION(name, file_path) Instrumentor::get_instance().begin_session(name, file_path)
#define EVE_PROFILE_END_SESSION() Instrumentor::get_instance().end_session()

#define EVE_PROFILE_SCOPE(name) InstrumentationTimer timer_##__LINE__(name)
#define EVE_PROFILE_FUNCTION() EVE_PROFILE_SCOPE(__FUNCSIG__)

#else

#define EVE_PROFILE_BEGIN_SESSION(name, file_path)
#define EVE_PROFILE_END_SESSION()

#define EVE_PROFILE_SCOPE(name)
#define EVE_PROFILE_FUNCTION()

#endif

#endif
