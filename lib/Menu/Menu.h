#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LinkedList.h>

typedef void (*func_t)();

class MenuComponent {
    const char* m_name;
    MenuComponent* m_parent;

   public:
    int m_selectedIndex = 0;

   public:
    MenuComponent(const char* name) : m_name(name){};

    const char* getName() { return m_name; };
    void setParent(MenuComponent* parent) { m_parent = parent; };
    MenuComponent* getParent() { return m_parent; };
    int getSelectedIndex() { return m_selectedIndex; };

    virtual LinkedList<MenuComponent*>* getChildren() { return nullptr; };
    virtual MenuComponent* getSelectedComponent() { return nullptr; };
    virtual int addChild(MenuComponent* child) { return -1; };
    virtual int moveCursor(int direction) { return -1; };
    virtual int isLeaf() { return false; };
    virtual void execute(){};
};

class SubMenu : public MenuComponent {
   private:
    LinkedList<MenuComponent*> m_children;

   public:
    SubMenu(const char* name) : MenuComponent(name){};

    LinkedList<MenuComponent*>* getChildren() { return &m_children; };
    MenuComponent* getSelectedComponent() { return m_children.get(m_selectedIndex); }
    int addChild(MenuComponent* child);
    int moveCursor(int direction);
};

class Leaf : public MenuComponent {
    func_t m_func;

   public:
    Leaf(const char* name, func_t func) : MenuComponent(name), m_func(func){};
    int isLeaf() { return true; };
    void execute() {
        if (m_func) m_func();
    }
};

class Menu {
    MenuComponent* m_menuTree;
    MenuComponent* m_currentMenu;

    Adafruit_SSD1306* m_display;

   public:
    Menu(Adafruit_SSD1306* disp) : m_display(disp){};
    void setMenuTree(MenuComponent* menuTree);
    void showMainMenu();
    void moveCursor(int direction);
    void selectEntry();

   private:
    void showMenu();
};