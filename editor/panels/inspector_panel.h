#ifndef INSPECTOR_PANEL_H
#define INSPECTOR_PANEL_H

#include "panels/panel.h"

class InspectorPanel : public Panel {
	EVE_IMPL_PANEL("Inspector")
public:
	InspectorPanel();

protected:
	void _draw() override;
};

#endif
