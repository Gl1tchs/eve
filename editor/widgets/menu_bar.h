#ifndef MENU_BAR_H
#define MENU_BAR_H

struct MenuItem {
	std::string name;
	std::string shortcut;
	std::function<void()> callback;

	MenuItem(const std::string& name, std::function<void()> callback);

	MenuItem(const std::string& name, const std::string& shortcut,
			std::function<void(void)> callback);
};

struct Menu {
	std::string name;
	std::vector<MenuItem> items;

	Menu(const std::string& name, std::initializer_list<MenuItem> items);

	size_t get_size() const;

	void push_item(const MenuItem& item);
};

class MenuBar final {
public:
	MenuBar() = default;

	void push_menu(const Menu& menu);

	void render();

private:
	std::vector<Menu> menus;
};

#endif
