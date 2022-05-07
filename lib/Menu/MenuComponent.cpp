#include <MenuComponent.h>

// === SUBMENU ===
uint8_t SubMenu::addChild(MenuComponent* child) {
    child->setParent(this);
    m_children.add(child);

    return getChildCount();
}

uint8_t SubMenu::moveCursor(int direction) {
    uint8_t cntEntries = getChildCount();  //+ 1;
    m_selectedIndex += direction;
    m_selectedIndex = (m_selectedIndex % cntEntries + cntEntries) % cntEntries;
    return m_selectedIndex;
}