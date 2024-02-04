#ifndef LOG_H
#define LOG_H

#include "core/memory.h"

enum class LogSender {
	ENGINE,
	CLIENT
};

enum class LogLevel {
	TRACE = 0,
	INFO,
	WARNING,
	ERROR,
	FATAL,
};

std::string deserialize_log_level(LogLevel level);

struct LogMessage {
	LogSender sender;
	LogLevel level;
	std::string time_stamp;
	std::string string;
};

std::string deserialize_log_sender(LogSender sender);

class LogBuffer {
public:
	LogBuffer(uint32_t max_messages);

	void log(LogSender sender, LogLevel level, const std::string& time_stamp,
			const std::string& m1essage);

	void clear();

	std::deque<LogMessage>::iterator begin() { return messages.begin(); }
	std::deque<LogMessage>::iterator end() { return messages.end(); }
	std::deque<LogMessage>::const_iterator begin() const {
		return messages.begin();
	}
	std::deque<LogMessage>::const_iterator end() const { return messages.end(); }

private:
	uint32_t max_messages = 1000;
	std::deque<LogMessage> messages;
};

class Logger {
public:
	static void init(const std::string& file_name);

	static void log(LogSender sender, LogLevel level, const std::string& fmt);

	static void push_buffer(Ref<LogBuffer>& buffer);

private:
	static std::string _get_colored_message(const std::string& message,
			LogLevel level);

private:
	static std::unordered_map<LogLevel, std::string> s_verbosity_colors;

	static std::ofstream s_log_file;
	static std::mutex s_logger_mutex;

	static std::vector<Ref<LogBuffer>> s_log_buffers;
};

#define EVE_LOG_ENGINE_TRACE(...)                   \
	Logger::log(LogSender::ENGINE, LogLevel::TRACE, \
			std::format(__VA_ARGS__))
#define EVE_LOG_ENGINE_INFO(...)                   \
	Logger::log(LogSender::ENGINE, LogLevel::INFO, \
			std::format(__VA_ARGS__))
#define EVE_LOG_ENGINE_WARNING(...)                   \
	Logger::log(LogSender::ENGINE, LogLevel::WARNING, \
			std::format(__VA_ARGS__))
#define EVE_LOG_ENGINE_ERROR(...)                   \
	Logger::log(LogSender::ENGINE, LogLevel::ERROR, \
			std::format(__VA_ARGS__))
#define EVE_LOG_ENGINE_FATAL(...)                   \
	Logger::log(LogSender::ENGINE, LogLevel::FATAL, \
			std::format(__VA_ARGS__))

#define EVE_LOG_CLIENT_TRACE(...)                    \
	Logger::log(LogSender::CLIENT, LogLevel::TRACE, \
			std::format(__VA_ARGS__))
#define EVE_LOG_CLIENT_INFO(...)                    \
	Logger::log(LogSender::CLIENT, LogLevel::INFO, \
			std::format(__VA_ARGS__))
#define EVE_LOG_CLIENT_WARNING(...)                    \
	Logger::log(LogSender::CLIENT, LogLevel::WARNING, \
			std::format(__VA_ARGS__))
#define EVE_LOG_CLIENT_ERROR(...)                    \
	Logger::log(LogSender::CLIENT, LogLevel::ERROR, \
			std::format(__VA_ARGS__))
#define EVE_LOG_CLIENT_FATAL(...)                    \
	Logger::log(LogSender::CLIENT, LogLevel::FATAL, \
			std::format(__VA_ARGS__))

#endif
