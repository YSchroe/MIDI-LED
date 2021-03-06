#include <AceButton.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>
#include <MIDI.h>
#include <Menu.h>
#include <Pins.h>
#include <Setting.h>

struct Scale {
    uint8_t length;
    uint8_t notes[10];
};

const char* notes[] = {"C", "C#", "D", "D#", "E", "F",
                       "F#", "G", "G#", "A", "A#", "H"};
const char* modeVals[] = {"MIDI LED", "Show Scale", "Check Scale"};

NamedValue<Scale> scaleValues[] = {
    NamedValue<Scale>{"Major", Scale{7, {0, 2, 4, 5, 7, 9, 11}}},
    NamedValue<Scale>{"Minor", Scale{7, {0, 2, 3, 5, 7, 8, 10}}}};

SettingGroup<uint8_t, 3> modes(modeVals);
SettingGroup<uint8_t, 12> baseNotes(notes);
SettingGroup<Scale, 2> scales(scaleValues);

enum LedMode : uint8_t {
    LED_MODE_MIDI_LED = 0,
    LED_MODE_SHOW_SCALE,
    LED_MODE_CHECK_SCALE
};

struct Settings {
    bool showNotesInDisplay = false;
} settings;

MIDI_CREATE_DEFAULT_INSTANCE();

ace_button::AceButton buttonUp(PIN_BTN_UP);
ace_button::AceButton buttonDown(PIN_BTN_DOWN);
ace_button::AceButton buttonOk(PIN_BTN_OK);

// Declaration for an SSD1306 display connected via I2C (SDA, SCL pins)
Adafruit_SSD1306 display(128, 64, &Wire);
Menu menu(&display);

/* LED STRIP CFG */
#define NUM_LEDS 144
CRGB leds[NUM_LEDS];
// _______________

/* GLOBALS */
#define OFFSET 36  // Lowest note playable by piano
#define NO_OF_KEYS 61

long pressedKeys[NO_OF_KEYS] = {};
uint8_t cntPressedKeysPrevious = 0;
uint8_t cntPressedKeysCurrent = 0;

/* CALLBACK FUNCTIONS */
void handleNoteOn(byte channel, byte pitch, byte velocity) {
    byte correctedPitch = pitch - OFFSET;
    CRGB color;
    if (modes.getSelectedValue() == LedMode::LED_MODE_MIDI_LED) {
        color = CRGB::BlueViolet;
    } else {
        color = CRGB::Red;
        for (int i = 0; i < scales.getSelectedValue().length; i++) {
            uint8_t note = scales.getSelectedValue().notes[i] + baseNotes.getSelectedValue();
            if ((correctedPitch - note) % 12 == 0) {
                color = CRGB::Green;
                break;
            }
        }
    }

    leds[correctedPitch] = color;
    FastLED.show();

    pressedKeys[correctedPitch] = millis();
    cntPressedKeysCurrent += 1;
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
    byte correctedPitch = pitch - OFFSET;

    leds[correctedPitch] = CRGB::Black;
    FastLED.show();

    pressedKeys[correctedPitch] = 0;
    cntPressedKeysCurrent -= 1;
}

void handleButtonPress(ace_button::AceButton* button, uint8_t eventType, uint8_t buttonState) {
    if (eventType == ace_button::AceButton::kEventPressed) {
        if (button->getPin() == PIN_BTN_UP) {
            menu.moveCursor(-1);
        } else if (button->getPin() == PIN_BTN_DOWN) {
            menu.moveCursor(1);
        } else if (button->getPin() == PIN_BTN_OK) {
            menu.selectEntry();
        }
    }
}

// -----------------------------------------------------------------------------
void setScale() {
    FastLED.clear(true);
    Scale selectedScale = scales.getSelectedValue();

    if (modes.getSelectedValue() == LedMode::LED_MODE_SHOW_SCALE) {
        for (int i = 0; i < selectedScale.length; i++) {
            int note = selectedScale.notes[i] + baseNotes.getSelectedValue();
            while (note < NUM_LEDS) {
                int octave = ceil(note / 12);
                leds[note].setRGB(255 - 20 * octave, 0 + 20 * octave, 0 + 20 * octave);
                note += 12;
            }
        }
        FastLED.show();
    }
}

void switchToNewMode() {
    FastLED.clear(true);
    int mode = modes.getSelectedValue();
    if (mode == LedMode::LED_MODE_SHOW_SCALE || LedMode::LED_MODE_CHECK_SCALE) {
        setScale();
    }
}

void setup() {
    /* B U T T O N S */
    pinMode(PIN_BTN_UP, INPUT);
    pinMode(PIN_BTN_DOWN, INPUT);
    pinMode(PIN_BTN_OK, INPUT);
    ace_button::ButtonConfig* buttonConfig = ace_button::ButtonConfig::getSystemButtonConfig();
    buttonConfig->setEventHandler(handleButtonPress);

    /* D I S P L A Y */
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        // endless loop in case display init fails
        for (;;) {
        }
    }
    delay(1000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.cp437(true);
    display.display();

    /* L E D S */
    FastLED.addLeds<NEOPIXEL, PIN_LED_DATA>(leds, NUM_LEDS);
    FastLED.setBrightness(15);

    /* M I D I*/
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.begin();
    MIDI.turnThruOff();

    /* M E N U */
    SubMenu& mainMenu = menu.getMenuTree();
    Leaf* backButton = new Leaf("BACK", []() {
        menu.goBack();
    });

    SubMenu* modeMenu = modes.createSettingsMenu("MODE", switchToNewMode);
    modeMenu->addChild(backButton);

    SubMenu* scaleMenu = scales.createSettingsMenu("SCALE", setScale);
    scaleMenu->addChild(backButton);

    SubMenu* baseNoteMenu = baseNotes.createSettingsMenu("BASE NOTE", setScale);
    baseNoteMenu->addChild(backButton);

    mainMenu.addChild(modeMenu);
    mainMenu.addChild(scaleMenu);
    mainMenu.addChild(baseNoteMenu);
    mainMenu.addChild(backButton);

    menu.showMainMenu();
}

void loop() {
    // read-in buttons
    buttonUp.check();
    buttonDown.check();
    buttonOk.check();

    // handle midi
    int selectedMode = modes.getSelectedValue();
    if (selectedMode == LedMode::LED_MODE_MIDI_LED || selectedMode == LedMode::LED_MODE_CHECK_SCALE) {
        MIDI.read();

        // FastLED.clear();
        // for (int i = 0; i < NO_OF_KEYS; i++) {
        //     if (pressedKeys[i] != 0) {
        //         leds[constrain(i, 0, NUM_LEDS - 1)] = CRGB::Orange;
        //         int offs = floor((millis() - pressedKeys[i]) / 5);
        //         leds[constrain(i + offs, 0, NUM_LEDS - 1)] = CRGB::Red;
        //         leds[constrain(i - offs, 0, NUM_LEDS - 1)] = CRGB::Red;
        //     }
        // }
        // FastLED.show();

        // L C D
        if (settings.showNotesInDisplay) {
            if (cntPressedKeysCurrent != cntPressedKeysPrevious) {
                display.clearDisplay();
                display.setCursor(0, 0);
                for (int i = 0; i < NO_OF_KEYS; i++) {
                    if (pressedKeys[i] != 0) {
                        display.print(notes[(i + OFFSET) % 12]);
                    }
                }
                display.display();
                cntPressedKeysPrevious = cntPressedKeysCurrent;
            }
        }
    }
}
