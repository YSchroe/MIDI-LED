#include <Menu.h>

#define LINE_HEIGHT 8

// === SUBMENU ===
int SubMenu::addChild(MenuComponent* child) {
    child->setParent(this);
    m_children.add(child);

    return m_children.size();
}

int SubMenu::moveCursor(int direction) {
    // entries = entries + 1 (since "Zurueck" is added implicitly)
    int cntEntries = m_children.size();  //+ 1;
    m_selectedIndex += direction;
    m_selectedIndex = (m_selectedIndex % cntEntries + cntEntries) % cntEntries;
    return m_selectedIndex;
}

// === LEAF ===

// ==MENU==
void Menu::setMenuTree(MenuComponent* menuTree) {
    m_menuTree = menuTree;
    m_currentMenu = m_menuTree;
}

void Menu::showMainMenu() {
    m_currentMenu = m_menuTree;
    showMenu();
}

void Menu::showMenu() {
    m_currentMenu->getChildren()->size();
    m_display->clearDisplay();
    m_display->setCursor(0, 0);
    m_display->print("==");
    m_display->print(m_currentMenu->getName());
    m_display->print("==");

    int yPos = LINE_HEIGHT;

    for (int i = 0; i < m_currentMenu->getChildren()->size(); i += 1) {
        m_display->setCursor(0, yPos);
        if (i == m_currentMenu->getSelectedIndex())
            m_display->print(">");
        else
            m_display->print(" ");

        MenuComponent* child = m_currentMenu->getChildren()->get(i);
        m_display->print(child->getName());
        yPos += LINE_HEIGHT;
    }

    m_display->display();
}

void Menu::moveCursor(int direction) {
    // erase previous cursor:
    m_display->fillRect(0, LINE_HEIGHT, 6, 64, BLACK);

    // move cursor
    int newPos = m_currentMenu->moveCursor(direction);

    // show cursor at new line:
    m_display->setCursor(0, LINE_HEIGHT + newPos * LINE_HEIGHT);
    m_display->print('>');
    m_display->display();
}

void Menu::selectEntry() {
    MenuComponent* selectedEntry = m_currentMenu->getSelectedComponent();
    if (!selectedEntry->isLeaf()) {
        m_currentMenu = selectedEntry;
        showMenu();
    } else {
        selectedEntry->execute();
        MenuComponent* parent = m_currentMenu->getParent();
        if (parent) {
            m_currentMenu = parent;
            showMenu();
        }
    }
}