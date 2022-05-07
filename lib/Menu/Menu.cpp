#include <Menu.h>

#define LINE_HEIGHT 8

Menu::Menu(Adafruit_SSD1306* disp) : m_display(disp),
                                     m_menuTree("MAIN MENU"),
                                     m_currentMenu(&m_menuTree) {}

SubMenu& Menu::getMenuTree() {
    return m_menuTree;
}

void Menu::showMainMenu() {
    m_currentMenu = &m_menuTree;
    showMenu();
}

void Menu::showMenu() {
    // clear display
    m_display->clearDisplay();
    m_display->setCursor(0, 0);

    // display menu name
    m_display->print("==");
    m_display->print(m_currentMenu->getName());
    m_display->print("==");

    // iterate over all menu items
    int yPos = LINE_HEIGHT;
    for (int i = 0; i < m_currentMenu->getChildCount(); i += 1) {
        m_display->setCursor(0, yPos);
        // display cursor
        if (i == m_currentMenu->getSelectedIndex())
            m_display->print(">");
        else
            m_display->print(" ");

        // display menu item name
        MenuComponent* child = m_currentMenu->getChildren()->get(i);
        m_display->print(child->getDisplayValue());
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

    if (selectedEntry->getChildCount() > 0) {
        // if selected item is submenu
        m_currentMenu = selectedEntry;

    } else {
        // if selected item is leaf
        selectedEntry->execute();
    }

    // rerender menu afterwards in any case, since leafs can change values
    showMenu();
}

void Menu::goBack() {
    MenuComponent* parent = m_currentMenu->getParent();
    if (parent) {
        m_currentMenu = parent;
        showMenu();
    }
}