#include "debug/instrumentor.h"

Instrumentor::Instrumentor() :
		current_session(nullptr), profile_count(0) {
}

void Instrumentor::begin_session(const std::string& name, const std::string& file_path) {
	std::lock_guard<std::mutex> lock(file_mutex);

	output_stream.open(file_path);
	_write_header();
	current_session = new InstrumentationSession{ name };
}

void Instrumentor::end_session() {
	std::lock_guard<std::mutex> lock(file_mutex);

	_write_footer();
	output_stream.close();
	delete current_session;
	current_session = nullptr;
	profile_count = 0;
}

void Instrumentor::write_profile(const ProfileResult& result) {
	std::lock_guard<std::mutex> lock(file_mutex);

	if (profile_count++ > 0) {
		output_stream << ",";
	}

	std::string name = result.name;
	std::replace(name.begin(), name.end(), '"', '\'');

	output_stream << "{";
	output_stream << "\"cat\":\"function\",";
	output_stream << "\"dur\":" << (result.end - result.start) << ',';
	output_stream << "\"name\":\"" << name << "\",";
	output_stream << "\"ph\":\"X\",";
	output_stream << "\"pid\":0,";
	output_stream << "\"tid\":" << result.thread_id << ",";
	output_stream << "\"ts\":" << result.start;
	output_stream << "}";

	output_stream.flush();
}

Instrumentor& Instrumentor::get_instance() {
	static Instrumentor instance;
	return instance;
}

void Instrumentor::_write_header() {
	output_stream << "{\"otherData\": {},\"traceEvents\":[";
	output_stream.flush();
}

void Instrumentor::_write_footer() {
	output_stream << "]}";
	output_stream.flush();
}

InstrumentationTimer::InstrumentationTimer(const std::string& name) :
		name(name), stopped(false) {
	start_point = std::chrono::steady_clock::now();
}

InstrumentationTimer::~InstrumentationTimer() {
	if (!stopped) {
		stop();
	}
}

void InstrumentationTimer::stop() {
	auto end_point = std::chrono::high_resolution_clock::now();

	uint64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(start_point).time_since_epoch().count();
	uint64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(end_point).time_since_epoch().count();

	Instrumentor::get_instance().write_profile({ name, start, end, std::this_thread::get_id() });

	stopped = true;
}
