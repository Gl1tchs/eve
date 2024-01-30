#ifndef STATS_PANEL_H
#define STATS_PANEL_H

#include "panels/panel.h"

class Renderer;

class StatsPanel : public Panel {
	EVE_IMPL_PANEL("Stats")

public:
	StatsPanel(const Ref<Renderer>& renderer);
	virtual ~StatsPanel() = default;

protected:
	void _draw() override;

private:
	Ref<Renderer> renderer;
};

#endif
