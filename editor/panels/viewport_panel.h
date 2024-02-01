#ifndef VIEWPORT_PANEL_H
#define VIEWPORT_PANEL_H

#include "panels/hierarchy_panel.h"
#include "panels/panel.h"

#include "renderer/frame_buffer.h"

class ViewportPanel : public Panel {
	EVE_IMPL_PANEL("Viewport")

public:
	ViewportPanel();

	void set_render_texture_id(uint32_t renderer_id);

	const glm::vec2& get_min_bounds() const;
	const glm::vec2& get_max_bounds() const;

protected:
	void _draw() override;

private:
	uint32_t texture_id = 0;

	glm::vec2 viewport_min_bounds{};
	glm::vec2 viewport_max_bounds{};
};

#endif
