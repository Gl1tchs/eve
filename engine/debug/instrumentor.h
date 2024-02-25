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

	void begin_session(const std::string& name,
			const std::string& file_path = "profile-result.json");

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

#define EVE_CONCAT_IMPL(x, y) x##y
#define EVE_CONCAT(x, y) EVE_CONCAT_IMPL(x, y)

#if EVE_DEBUG
#define EVE_ENABLE_PROFILING 1
#else
#define EVE_ENABLE_PROFILING 0
#endif

#if EVE_ENABLE_PROFILING

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) ||    \
		(defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define EVE_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define EVE_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define EVE_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) ||              \
		(defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define EVE_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define EVE_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define EVE_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define EVE_FUNC_SIG __func__
#else
#define EVE_FUNC_SIG "EVE_FUNC_SIG unknown!"
#endif

#define EVE_PROFILE_BEGIN_SESSION(name, file_path)                             \
	Instrumentor::get_instance().begin_session(name, file_path)
#define EVE_PROFILE_END_SESSION() Instrumentor::get_instance().end_session()

#define EVE_PROFILE_SCOPE(name)                                                \
	InstrumentationTimer EVE_CONCAT(timer_, __LINE__)(name)
#define EVE_PROFILE_FUNCTION() EVE_PROFILE_SCOPE(EVE_FUNC_SIG)

#else

#define EVE_PROFILE_BEGIN_SESSION(name, file_path)
#define EVE_PROFILE_END_SESSION()

#define EVE_PROFILE_SCOPE(name)
#define EVE_PROFILE_FUNCTION()

#endif

#endif
