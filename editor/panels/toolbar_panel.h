#ifndef TOOLBAR_PANEL_H
#define TOOLBAR_PANEL_H

#include "panels/panel.h"

enum class SceneState { EDIT,
	PLAY,
	PAUSED };

class ToolbarPanel : public Panel {
	EVE_IMPL_PANEL("Toolbar")

public:
	ToolbarPanel();

	std::function<void()> on_play;
	std::function<void()> on_stop;
	std::function<void()> on_pause;
	std::function<void()> on_resume;
	std::function<void()> on_step;

	void set_state(SceneState _state);

protected:
	void _draw() override;

private:
	SceneState state = SceneState::EDIT;
};

#endif
