#include "debug/log.h"

#include "core/core_minimal.h"

std::string deserialize_log_level(LogLevel level) {
	switch (level) {
		case LogLevel::TRACE:
			return "TRACE";
		case LogLevel::INFO:
			return "INFO";
		case LogLevel::WARNING:
			return "WARNING";
		case LogLevel::ERROR:
			return "ERROR";
		case LogLevel::FATAL:
			return "FATAL";
		default:
			return "";
	}
}

std::string deserialize_log_sender(LogSender sender) {
	switch (sender) {
		case LogSender::ENGINE:
			return "ENGINE";
		case LogSender::CLIENT:
			return "CLIENT";
		default:
			return "";
	}
}

inline static std::string get_timestamp() {
	auto now =
			std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::tm tm_now{};
	std::stringstream ss;
#if EVE_PLATFORM_WINDOWS
	localtime_s(&tm_now, &now);
#else
	localtime_r(&now, &tm_now);
#endif

	ss << std::put_time(&tm_now, "%H:%M:%S");

	return ss.str();
}

std::unordered_map<LogLevel, std::string> Logger::s_verbosity_colors = {
	{ LogLevel::TRACE, "\x1B[1m" }, // None
	{ LogLevel::INFO, "\x1B[32m" }, // Green
	{ LogLevel::WARNING, "\x1B[93m" }, // Yellow
	{ LogLevel::ERROR, "\x1B[91m" }, // Light Red
	{ LogLevel::FATAL, "\x1B[31m" }, // Red
};

std::ofstream Logger::s_log_file;
std::mutex Logger::s_logger_mutex;

std::vector<Ref<LogBuffer>> Logger::s_log_buffers = {};

LogBuffer::LogBuffer(uint32_t max_messages) :
		max_messages(max_messages) {}

void LogBuffer::log(LogSender sender, LogLevel level,
		const std::string& time_stamp, const std::string& message) {
	if (messages.size() + 1 >= max_messages) {
		messages.pop_front();
	}

	messages.push_back({ sender, level, time_stamp, message });
}

void LogBuffer::clear() {
	messages.clear();
}

void Logger::init(const std::string& file_name) {
	std::lock_guard<std::mutex> lock(s_logger_mutex);

	s_log_file.open(file_name);
	if (!s_log_file.is_open()) {
		throw std::runtime_error(
				"Error: Unable to initialize logger file does not exists!\n");
	}
}

void Logger::log(LogSender sender, LogLevel level, const std::string& fmt) {
	std::lock_guard<std::mutex> lock(s_logger_mutex);

	const std::string time_stamp = get_timestamp();

	const std::string message =
			std::format("[{}] [{}] [{}]: \"{}\"", time_stamp,
					deserialize_log_sender(sender), deserialize_log_level(level), fmt);

	const std::string colored_messages = _get_colored_message(message, level);

	// Output to buffers
	for (auto& buffer : s_log_buffers) {
		buffer->log(sender, level, time_stamp, fmt);
	}

	// Output to stdout
	std::cout << colored_messages << "\x1B[0m\n";

	// Output to file
	if (s_log_file.is_open()) {
		s_log_file << message << "\n";
		s_log_file.flush();
	}
}

void Logger::push_buffer(Ref<LogBuffer>& buffer) {
	s_log_buffers.push_back(buffer);
}

std::string Logger::_get_colored_message(const std::string& message,
		LogLevel level) {
	const auto color_it = s_verbosity_colors.find(level);
	if (color_it != s_verbosity_colors.end()) {
		return color_it->second + message;
	}

	return message; // No color for the default case
}
