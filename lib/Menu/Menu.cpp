#include "Menu.h"

#define LINE_HEIGHT 8

// === MENU COMPONENT ===
MenuComponent::MenuComponent() {}
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

// === SUBMENU ===
SubMenu::SubMenu(const char* name) : MenuComponent(name) {}

LinkedList<MenuComponent>& SubMenu::getChildren() {
    return m_children;
}

int SubMenu::addChild(MenuComponent& item) {
    item.setParent(this);
    m_children.add(item);

    return m_children.size();
}

int SubMenu::moveCursor(int direction) {
    // entries = entries + 1 (since "Zurueck" is added implicitly)
    int cntEntries = m_children.size();  //+ 1;
    m_selectedEntry += direction;
    m_selectedEntry = (m_selectedEntry % cntEntries + cntEntries) % cntEntries;
    return m_selectedEntry;
}

int SubMenu::getSelectedEntry() {
    return m_selectedEntry;
}

// === LEAF ===
Leaf::Leaf(const char* name) : MenuComponent(name) {}

// ==MENU==
Menu::Menu(Adafruit_SSD1306* disp) {
    m_display = disp;
}

void Menu::setMenuTree(SubMenu* menuTree) {
    m_menuTree = menuTree;
    m_currentMenu = m_menuTree;
}

void Menu::showMainMenu() {
    m_currentMenu = m_menuTree;
    showMenu();
}

void Menu::showMenu() {
    m_currentMenu->getChildren().size();
    m_display->clearDisplay();
    m_display->setCursor(0, 0);
    m_display->print("==");
    m_display->print(m_currentMenu->getName());
    m_display->print("==");

    int yPos = LINE_HEIGHT;

    for (int i = 0; i < m_currentMenu->getChildren().size(); i += 1) {
        m_display->setCursor(0, yPos);
        if (i == m_currentMenu->getSelectedEntry())
            m_display->print(">");
        else
            m_display->print(" ");
        m_display->print(m_currentMenu->getChildren().get(i).getName());
        yPos += LINE_HEIGHT;
    }

    m_display->display();
}

void Menu::moveCursor(int direction) {
    // erase previous cursor:
    m_display->fillRect(0, LINE_HEIGHT, 6, 64, BLACK);

    // move cursor
    int newPos = m_currentMenu->moveCursor(direction);
    m_display->clearDisplay();
    m_display->setCursor(0, 0);
    m_display->print(newPos);
    m_display->print(direction);

    // show cursor at new line:
    m_display->setCursor(0, LINE_HEIGHT + newPos * LINE_HEIGHT);
    m_display->print('>');
    m_display->display();
}

void Menu::selectEntry() {
}

//

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
