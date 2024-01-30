#include "widgets/menu_bar.h"

#include <imgui.h>

MenuItem::MenuItem(const std::string& _name, std::function<void(void)> _callback) :
		name(_name), shortcut(""), callback(_callback) {
}

MenuItem::MenuItem(const std::string& _name, const std::string& _shortcut,
		std::function<void()> _callback) :
		name(_name), shortcut(_shortcut), callback(_callback) {
}

Menu::Menu(const std::string& name, std::initializer_list<MenuItem> items) :
		name(name), items(items) {}

size_t Menu::get_size() const {
	return items.size();
}

void Menu::push_item(const MenuItem& item) {
	items.push_back(item);
}

void MenuBar::push_menu(const Menu& menu) {
	menus.push_back(menu);
}

void MenuBar::render() {
	if (ImGui::BeginMenuBar()) {
		for (auto& menu : menus) {
			if (ImGui::BeginMenu(menu.name.c_str())) {
				uint32_t index = 0;
				for (auto& item : menu.items) {
					// draw menu item
					if (ImGui::MenuItem(item.name.c_str(),
								item.shortcut.c_str())) {
						if (item.callback) {
							item.callback();
						}
					}

					index++;
				}

				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();
	}
}
