#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LinkedList.h>

// class Menu {
//     Adafruit_SSD1306* m_display;
//     int m_cursor = 0;

//    public:
//     Menu(Adafruit_SSD1306* disp);
//     void showMainMenu();
//     void moveCursor(int direction);
//     void execute();

//    private:
//     void showMenu();
// };

class MenuComponent {
    const char* m_name;
    MenuComponent* m_parent;

   public:
    MenuComponent();
    MenuComponent(const char* name);

    virtual const char* getName();
    virtual MenuComponent& getParent();
    virtual void setParent(MenuComponent* parent);
    // virtual void execute();
};

class SubMenu : public MenuComponent {
   private:
    LinkedList<MenuComponent> m_children;
    int m_selectedEntry = 0;

   public:
    SubMenu(const char* name);
    int addChild(MenuComponent& a);
    int moveCursor(int direction);
    int getSelectedEntry();
    LinkedList<MenuComponent>& getChildren();
};

class Leaf : public MenuComponent {
   public:
    Leaf(const char* name);
};

class Menu {
    SubMenu* m_menuTree;
    SubMenu* m_currentMenu;

    Adafruit_SSD1306* m_display;

   public:
    Menu(Adafruit_SSD1306* disp);
    void setMenuTree(SubMenu* menuTree);
    void showMainMenu();
    void moveCursor(int direction);
    void selectEntry();

   private:
    void showMenu();
};