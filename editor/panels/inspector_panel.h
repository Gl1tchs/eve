#ifndef INSPECTOR_PANEL_H
#define INSPECTOR_PANEL_H

#include "panels/panel.h"

#include "panels/hierarchy_panel.h"

class InspectorPanel : public Panel {
	EVE_IMPL_PANEL("Inspector")
public:
	InspectorPanel(Ref<HierarchyPanel> hierarchy);

protected:
	void _draw() override;

private:
	Ref<HierarchyPanel> hierarchy;
};

#endif
