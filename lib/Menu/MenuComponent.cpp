#include <MenuComponent.h>

// === SUBMENU ===
int SubMenu::addChild(MenuComponent* child) {
    child->setParent(this);
    m_children.add(child);

    return getChildCount();
}

int SubMenu::moveCursor(int direction) {
    // entries = entries + 1 (since "Zurueck" is added implicitly)
    int cntEntries = getChildCount();  //+ 1;
    m_selectedIndex += direction;
    m_selectedIndex = (m_selectedIndex % cntEntries + cntEntries) % cntEntries;
    return m_selectedIndex;
}