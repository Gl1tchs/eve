#ifndef CONSOLE_PANEL_H
#define CONSOLE_PANEL_H

#include "panels/panel.h"

enum LogLevelFilter : uint16_t {
	LOG_LEVEL_FILTER_TRACE = 1 << 0,
	LOG_LEVEL_FILTER_INFO = 1 << 1,
	LOG_LEVEL_FILTER_WARNING = 1 << 2,
	LOG_LEVEL_FILTER_ERROR = 1 << 3,
	LOG_LEVEL_FILTER_FATAL = 1 << 4
};

class ConsolePanel : public Panel {
	EVE_IMPL_PANEL("Console")

public:
	ConsolePanel();

	void clear();

private:
	bool _is_level_in_filter(LogLevel level);

	bool _is_level_in_filter(LogLevelFilter filter);

	void _draw_filter_selectable(const std::string& name,
			LogLevelFilter filter);

protected:
	void _draw() override;

private:
	Ref<LogBuffer> buffer;

	bool auto_scroll = true;
	bool scroll_to_bottom = false;
	int selected_log_idx = -1;

	uint16_t level_filters = LOG_LEVEL_FILTER_TRACE | LOG_LEVEL_FILTER_INFO |
			LOG_LEVEL_FILTER_WARNING | LOG_LEVEL_FILTER_ERROR |
			LOG_LEVEL_FILTER_FATAL;
};

#endif
