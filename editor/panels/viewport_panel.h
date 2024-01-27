#ifndef VIEWPORT_PANEL_H
#define VIEWPORT_PANEL_H

#include "panels/hierarchy_panel.h"
#include "panels/panel.h"

#include "renderer/frame_buffer.h"

class ViewportPanel : public Panel {
	EVE_IMPL_PANEL("Viewport")

public:
	ViewportPanel(Ref<FrameBuffer> frame_buffer);

	glm::vec2 get_min_bounds();
	glm::vec2 get_max_bounds();

protected:
	void _draw() override;

private:
	Ref<FrameBuffer> frame_buffer;

	glm::vec2 viewport_min_bounds{};
	glm::vec2 viewport_max_bounds{};
};

#endif
