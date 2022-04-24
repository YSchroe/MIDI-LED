#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Menu.h>

#define MENU_SIZE 4
#define LINE_HEIGHT 8

const char *entries[MENU_SIZE] = 
{
    "Allgemeines",
    "Tonleiter",
    "LED Modus",
    " <-Zur"
    "\x81"
    "ck"};

Menu::Menu(Adafruit_SSD1306 &disp) {
    m_display = disp;
}

/**
 * Clear display and show the menu.
 */
void Menu::showMainMenu() {
    m_cursor = 0;
    m_display.clearDisplay();
    // show menu items:
    for (int i = 0; i < MENU_SIZE; i++) {
        m_display.setCursor(6, i * LINE_HEIGHT);
        m_display.print(entries[i]);
    }
    m_display.setCursor(0, 0);
    m_display.print('>');
    m_display.display();
}

void Menu::moveCursor(int direction) {
    // erase previous cursor:
    m_display.fillRect(0, 0, 6, 64, BLACK);

    // move cursor
    m_cursor = m_cursor + direction;
    m_cursor = (m_cursor % MENU_SIZE + MENU_SIZE) % MENU_SIZE;

    // show cursor at new line:
    m_display.setCursor(0, m_cursor * LINE_HEIGHT);
    m_display.print('>');
    m_display.display();
}
