#ifndef PANEL_H
#define PANEL_H

#define EVE_IMPL_PANEL(name)                                                   \
	std::string _get_name() override { return name; }

class Panel {
public:
	Panel(bool default_active = true, glm::vec2 size = {}, glm::vec2 pos = {});
	virtual ~Panel() = default;

	void render();

	void set_flags(int flags);

	void set_static(bool value);

	bool is_active() const;
	void set_active(bool active);

	bool is_focused() const;

	bool is_hovered() const;

	void set_pos(const glm::vec2& pos);
	const glm::vec2& get_pos() const;

	void set_size(const glm::vec2& size);
	const glm::vec2& get_size() const;

	const glm::vec2& get_min_region() const;

	const glm::vec2& get_max_region() const;

protected:
	virtual void _draw() = 0;

private:
	virtual std::string _get_name() = 0;

private:
	int window_flags = 0;

	bool active;
	bool hovered = false;
	bool focused = false;
	bool statik = false;

	glm::vec2 panel_size;
	glm::vec2 panel_pos;

	glm::vec2 min_region{};
	glm::vec2 max_region{};
};

#define EVE_BEGIN_FIELD(name)                                                  \
	{                                                                          \
		ImGui::Columns(2, nullptr, false);                                     \
		ImGui::SetColumnWidth(0, 75);                                          \
		const float field_width = ImGui::GetColumnWidth(1) - 10.0f;            \
		ImGui::TextUnformatted(name);                                          \
		ImGui::NextColumn();                                                   \
		ImGui::PushItemWidth(field_width);

#define EVE_END_FIELD()                                                        \
	ImGui::PopItemWidth();                                                     \
	ImGui::Columns();                                                          \
	}

#endif
