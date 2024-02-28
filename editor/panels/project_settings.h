#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include "panels/panel.h"

enum class ProjectSettingSection {
	GENERAL,
	INPUT,
	PHYSICS,
	SCRIPTING,
	SHIPPING,
};

class ProjectSettingsPanel : public Panel {
	EVE_IMPL_PANEL("Project Settings")

public:
	ProjectSettingsPanel();

protected:
	void _draw() override;

private:
	void _draw_general_settings();

	void _draw_input_settings();

	void _draw_physics_settings();

	void _draw_scripting_settings();

	void _draw_shipping_settings();

private:
	ProjectSettingSection selected_section = ProjectSettingSection::GENERAL;

	inline static uint32_t counter = 1;
};

#endif
