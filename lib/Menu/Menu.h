#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MenuComponent.h>

class Menu {
    SubMenu m_menuTree;
    MenuComponent* m_currentMenu;
    Adafruit_SSD1306* m_display;

   public:
    Menu(Adafruit_SSD1306* disp);
    SubMenu& getMenuTree();
    void showMainMenu();
    void moveCursor(int direction);
    void selectEntry();
    void goBack();

   private:
    void showMenu();
};