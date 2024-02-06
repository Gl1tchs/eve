#ifndef STATS_PANEL_H
#define STATS_PANEL_H

#include "panels/panel.h"

class StatsPanel : public Panel {
	EVE_IMPL_PANEL("Stats")

public:
	StatsPanel() = default;
	virtual ~StatsPanel() = default;

protected:
	void _draw() override;
};

#endif
