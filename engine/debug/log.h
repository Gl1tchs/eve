#ifndef LOG_H
#define LOG_H

#include "core/memory.h"

enum class LogLevel {
	TRACE = 0,
	INFO,
	WARNING,
	ERROR,
	FATAL,
};

std::string deserialize_log_level(LogLevel level);

struct LogMessage {
	LogLevel level;
	std::string time_stamp;
	std::string string;
};

class LogBuffer {
public:
	LogBuffer(uint32_t max_messages);

	void log(LogLevel level, const std::string& time_stamp,
			const std::string& message);

	void clear();

	std::deque<LogMessage>::iterator begin() { return messages.begin(); }
	std::deque<LogMessage>::iterator end() { return messages.end(); }
	std::deque<LogMessage>::const_iterator begin() const {
		return messages.begin();
	}
	std::deque<LogMessage>::const_iterator end() const {
		return messages.end();
	}

private:
	uint32_t max_messages = 1000;
	std::deque<LogMessage> messages;
};

class Logger {
public:
	static void init(const std::string& file_name);

	static void log(LogLevel level, const std::string& fmt, bool verbose = false);

	static void push_buffer(Ref<LogBuffer>& buffer);

private:
	static std::string _get_colored_message(
			const std::string& message, LogLevel level);

private:
	static std::unordered_map<LogLevel, std::string> s_verbosity_colors;

	static std::ofstream s_log_file;
	static std::mutex s_logger_mutex;

	static std::vector<Ref<LogBuffer>> s_log_buffers;
};

#define EVE_LOG_TRACE(...)                                              \
	Logger::log(LogLevel::TRACE, std::format(__VA_ARGS__))
#define EVE_LOG_INFO(...)                                               \
	Logger::log(LogLevel::INFO, std::format(__VA_ARGS__))
#define EVE_LOG_WARNING(...)                                            \
	Logger::log(LogLevel::WARNING, std::format(__VA_ARGS__))
#define EVE_LOG_ERROR(...)                                              \
	Logger::log(LogLevel::ERROR, std::format(__VA_ARGS__))
#define EVE_LOG_FATAL(...)                                              \
	Logger::log(LogLevel::FATAL, std::format(__VA_ARGS__))

#define EVE_LOG_VERBOSE_TRACE(...)                                             \
	Logger::log(LogLevel::TRACE, std::format(__VA_ARGS__), true)
#define EVE_LOG_VERBOSE_INFO(...)                                              \
	Logger::log(LogLevel::INFO, std::format(__VA_ARGS__), true)
#define EVE_LOG_VERBOSE_WARNING(...)                                           \
	Logger::log(LogLevel::WARNING, std::format(__VA_ARGS__), true)
#define EVE_LOG_VERBOSE_ERROR(...)                                             \
	Logger::log(LogLevel::ERROR, std::format(__VA_ARGS__), true)
#define EVE_LOG_VERBOSE_FATAL(...)                                             \
	Logger::log(LogLevel::FATAL, std::format(__VA_ARGS__), true)

#endif
