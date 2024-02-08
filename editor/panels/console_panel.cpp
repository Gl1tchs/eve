#include "panels/console_panel.h"

#include "debug/log.h"

#include <imgui.h>

void push_button_activity_color() {
	ImGui::PushStyleColor(ImGuiCol_Button,
			ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
}

ConsolePanel::ConsolePanel() {
	buffer = create_ref<LogBuffer>(1000);
	Logger::push_buffer(buffer);
}

void ConsolePanel::clear() {
	buffer->clear();
}

bool ConsolePanel::_is_level_in_filter(LogLevel level) {
	switch (level) {
		case LogLevel::TRACE:
			return _is_level_in_filter(LOG_LEVEL_FILTER_TRACE);
		case LogLevel::INFO:
			return _is_level_in_filter(LOG_LEVEL_FILTER_INFO);
		case LogLevel::WARNING:
			return _is_level_in_filter(LOG_LEVEL_FILTER_WARNING);
		case LogLevel::ERROR:
			return _is_level_in_filter(LOG_LEVEL_FILTER_ERROR);
		case LogLevel::FATAL:
			return _is_level_in_filter(LOG_LEVEL_FILTER_FATAL);
		default:
			return false;
	}
}

bool ConsolePanel::_is_level_in_filter(LogLevelFilter filter) {
	return (level_filters & filter) != 0;
}

bool ConsolePanel::_is_sender_in_filter(LogSender sender) {
	switch (sender) {
		case LogSender::ENGINE:
			return _is_sender_in_filter(LOG_SENDER_FILTER_ENGINE);
		case LogSender::CLIENT:
			return _is_sender_in_filter(LOG_SENDER_FILTER_CLIENT);
		default:
			return false;
	}
}

bool ConsolePanel::_is_sender_in_filter(LogSenderFilter filter) {
	return (sender_filters & filter) != 0;
}

void ConsolePanel::_draw_sender_filter_selectable(const std::string& name,
		LogSenderFilter filter) {
	bool use_color = _is_sender_in_filter(filter);
	if (use_color) {
		push_button_activity_color();
	}

	if (ImGui::SmallButton(name.c_str())) {
		sender_filters ^= filter;
	}

	if (use_color) {
		ImGui::PopStyleColor();
	}
}

void ConsolePanel::_draw_filter_selectable(const std::string& name,
		LogLevelFilter filter) {
	bool use_color = _is_level_in_filter(filter);
	if (use_color) {
		push_button_activity_color();
	}

	if (ImGui::SmallButton(name.c_str())) {
		level_filters ^= filter;
	}

	if (use_color) {
		ImGui::PopStyleColor();
	}
}

void ConsolePanel::_draw() {
	if (!buffer) {
		return;
	}

	const float footer_height_to_reserve =
			ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

	if (ImGui::SmallButton("Clear")) {
		clear();
	}

	ImGui::SameLine();

	// Options menu
	if (ImGui::BeginPopup("Options")) {
		ImGui::Checkbox("Auto-scroll", &auto_scroll);
		ImGui::EndPopup();
	}

	// Options, Filter
	if (ImGui::SmallButton("Options")) {
		ImGui::OpenPopup("Options");
	}

	ImGui::SameLine();
	ImGui::TextUnformatted("|");
	ImGui::SameLine();

	_draw_sender_filter_selectable("Engine", LOG_SENDER_FILTER_ENGINE);
	ImGui::SameLine();
	_draw_sender_filter_selectable("Client", LOG_SENDER_FILTER_CLIENT);

	ImGui::SameLine();
	ImGui::TextUnformatted("|");
	ImGui::SameLine();

	_draw_filter_selectable("Trace", LOG_LEVEL_FILTER_TRACE);
	ImGui::SameLine();
	_draw_filter_selectable("Info", LOG_LEVEL_FILTER_INFO);
	ImGui::SameLine();
	_draw_filter_selectable("Warning", LOG_LEVEL_FILTER_WARNING);
	ImGui::SameLine();
	_draw_filter_selectable("Error", LOG_LEVEL_FILTER_ERROR);
	ImGui::SameLine();
	_draw_filter_selectable("Fatal", LOG_LEVEL_FILTER_FATAL);

	ImGui::Separator();

	if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve),
				ImGuiChildFlags_None,
				ImGuiWindowFlags_HorizontalScrollbar)) {
		if (ImGui::BeginPopupContextWindow()) {
			if (ImGui::Selectable("Clear")) {
				clear();
			}
			ImGui::EndPopup();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

		int i = 0;
		for (const LogMessage& message : *buffer) {
			if (!_is_level_in_filter(message.level) || !_is_sender_in_filter(message.sender)) {
				continue;
			}

			ImGui::PushID(i);

			ImVec4 color;
			bool has_color = false;
			switch (message.level) {
				case LogLevel::TRACE:
					has_color = false;
					break;
				case LogLevel::INFO:
					has_color = true;
					color = ImVec4(0.5f, 1.0f, 0.88f, 1.0f);
					break;
				case LogLevel::WARNING:
					has_color = true;
					color = ImVec4(0.88f, 0.88f, 0.5f, 1.0f);
					break;
				case LogLevel::ERROR:
					has_color = true;
					color = ImVec4(0.88f, 0.5f, 0.5f, 1.0f);
					break;
				case LogLevel::FATAL:
					has_color = true;
					color = ImVec4(0.93f, 0.38f, 0.54f, 1.0f);
					break;
				default:
					break;
			}

			if (has_color) {
				ImGui::PushStyleColor(ImGuiCol_Text, color);
			}

			std::string log =
					std::format("[{}] [{}] [{}] {}", message.time_stamp,
							deserialize_log_sender(message.sender),
							deserialize_log_level(message.level), message.string);

			if (ImGui::Selectable(log.c_str(), selected_log_idx == i)) {
				selected_log_idx = (selected_log_idx == i) ? -1 : i;
			}

			if (has_color) {
				ImGui::PopStyleColor();
			}

			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::Selectable("Copy")) {
					ImGui::SetClipboardText(log.c_str());
				}
				ImGui::EndPopup();
			}

			ImGui::PopID();

			i++;
		}

		if (scroll_to_bottom ||
				(auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
			ImGui::SetScrollHereY(1.0f);
		}

		scroll_to_bottom = false;

		ImGui::PopStyleVar();
	}

	ImGui::EndChild();
}
