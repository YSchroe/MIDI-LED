#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Menu {
    Adafruit_SSD1306 *m_display;
    int m_cursor = 0;

   public:
    Menu(Adafruit_SSD1306 *disp);
    void showMenu();
    void moveCursor(int direction);
};