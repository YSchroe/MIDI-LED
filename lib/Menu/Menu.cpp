#include <Menu.h>

MenuComponent::MenuComponent() {
}

MenuComponent::MenuComponent(const char* name) {
    m_name = name;
}

void MenuComponent::setParent(MenuComponent* parent) {
    m_parent = parent;
}

MenuComponent& MenuComponent::getParent() {
    return *m_parent;
}

const char* MenuComponent::getName() {
    return m_name;
}

Menu::Menu(const char* name) : MenuComponent(name) {
}

LinkedList<MenuComponent>& Menu::getChildren() {
    return m_children;
}

Leaf::Leaf(const char* name) : MenuComponent(name) {
}

int Menu::addChild(MenuComponent& item) {
    item.setParent(this);
    m_children.add(item);

    return m_children.size();
}

// #define LINE_HEIGHT 8

// struct SubMenu {
//     const char* name;
//     unsigned int cntEntries;
//     const char* entries[10];
// };

// SubMenu menus[] = {
//     {"Main", 3, {"Allgemeines", "Tonleiter", "LED-Modus"}},
//     {"Allgemeines", 1, {"Noten anzeigen"}}};

// SubMenu* selectedMenu = &menus[0];

// Menu::Menu(Adafruit_SSD1306* disp) {
//     m_display = disp;
// }

// /**
//  * Clear display and show the menu.
//  */
// void Menu::showMainMenu() {
//     selectedMenu = &menus[0];
//     Menu::showMenu();
// }

// void Menu::showMenu() {
//     m_cursor = 0;
//     m_display->clearDisplay();
//     // show menu items:
//     for (int i = 0; i < selectedMenu->cntEntries; i++) {
//         m_display->setCursor(6, i * LINE_HEIGHT);
//         m_display->print(selectedMenu->entries[i]);
//     }

//     // add "Zurück" entry
//     m_display->setCursor(6, selectedMenu->cntEntries * LINE_HEIGHT);
//     m_display->print("--Zurueck");

//     m_display->setCursor(0, 0);
//     m_display->print('>');
//     m_display->display();
// }

// void Menu::moveCursor(int direction) {
//     // entries = entries + 1 (since "Zurueck" is added implicitly)
//     int cntEntries = selectedMenu->cntEntries + 1;

//     // erase previous cursor:
//     m_display->fillRect(0, 0, 6, 64, BLACK);

//     // move cursor
//     m_cursor = m_cursor + direction;
//     m_cursor = (m_cursor % cntEntries + cntEntries) % cntEntries;

//     // show cursor at new line:
//     m_display->setCursor(0, m_cursor * LINE_HEIGHT);
//     m_display->print('>');
//     m_display->display();
// }

// void Menu::execute() {
//     if (m_cursor == selectedMenu->cntEntries)
//         Menu::showMainMenu();
//     else {
//         const char* selectedEntry = selectedMenu->entries[m_cursor];
//         for (int i = 0; i < 2; i += 1) {
//             if (strcmp(selectedEntry, menus[i].name) == 0) selectedMenu = &menus[i];
//         }
//         Menu::showMenu();
//     }
// }
