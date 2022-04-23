#include <AceButton.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>
#include <MidiCfg.h>
#include <Pins.h>
#include <Scales.h>

ace_button::AceButton buttonUp(PIN_BTN_UP);
ace_button::AceButton buttonDown(PIN_BTN_DOWN);
ace_button::AceButton buttonOk(PIN_BTN_OK);

Scale selectedScale = scales[0];

#define MODE_MIDI_LED 0
#define MODE_SHOW_SCALE 1
#define MODE_CHECK_SCALE 2
unsigned int mode = 0;

// Declaration for an SSD1306 display connected via I2C (SDA, SCL pins)
Adafruit_SSD1306 display(128, 64, &Wire, -1);

/* LED STRIP CFG */
#define NUM_LEDS 144
CRGB leds[NUM_LEDS];
// _______________

/* GLOBALS */
#define OFFSET 36  // Lowest note playable by piano
#define NO_OF_KEYS 61
const char* notes[12] = {"C", "C#", "D", "D#", "E", "F",
                         "F#", "G", "G#", "A", "A#", "H"};

unsigned long pressedKeys[NO_OF_KEYS] = {};
uint8_t cntPressedKeysPrevious = 0;
uint8_t cntPressedKeysCurrent = 0;

#define MENU_SIZE 4
const char* menu[MENU_SIZE] = {"LED Modus", "Tonleiter", "Akkorde erkennen", "Option 4"};
#define LINE_HEIGHT 8

int cursor = 0;

/* CALLBACK FUNCTIONS */
void handleNoteOn(byte channel, byte pitch, byte velocity) {
    byte correctedPitch = pitch - OFFSET;
    CRGB color;
    if (mode == MODE_MIDI_LED) {
        color = CRGB::BlueViolet;
    } else {
        color = CRGB::Red;
        for (uint8_t i = 0; i < selectedScale.length; i++) {
            if ((correctedPitch % 12) == selectedScale.notes[i]) {
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
        // erase previous cursor:
        display.fillRect(0, 0, 6, 64, BLACK);

        if (button->getPin() == PIN_BTN_UP) {  // up
            cursor++;
            if (cursor > (MENU_SIZE - 1)) cursor = 0;
        } else if (button->getPin() == PIN_BTN_DOWN) {  // down
            cursor--;
            if (cursor < 0) cursor = (MENU_SIZE - 1);
        }
        // show cursor at new line:
        display.setCursor(0, cursor * LINE_HEIGHT);
        display.print('>');
        display.display();
    }
    // switch (eventType) {
    //     case ace_button::AceButton::kEventPressed:
    //         if (button->getPin() == PIN_BTN_UP) {
    //             mode = (mode + 1) % 3;
    //         } else if (button->getPin() == PIN_BTN_DOWN) {
    //             mode = (mode - 1) % 3;
    //         }
    //         switchMode(mode);
    //         display.clearDisplay();
    //         display.setCursor(0, 0);
    //         display.print("Mode: ");
    //         display.print(mode);
    //         display.display();
    //         break;
    // }
}

// -----------------------------------------------------------------------------
void setScale(const char* scaleName) {
    FastLED.clear(true);
    for (uint8_t i = 0; i < sizeof(scales); i++) {
        if (scales[i].name == scaleName) {
            selectedScale = scales[i];
            break;
        }
    }

    if (mode == MODE_SHOW_SCALE) {
        for (uint8_t i = 0; i < selectedScale.length; i++) {
            uint8_t note = selectedScale.notes[i];
            while (note < NUM_LEDS) {
                uint8_t octave = ceil(note / 12);
                leds[note].setRGB(255 - 20 * octave, 0 + 20 * octave, 0 + 20 * octave);
                note += 12;
            }
        }
        FastLED.show();
    }

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(scaleName);
}

void switchMode(uint8_t tgtMode) {
    MIDI.disconnectCallbackFromType(MIDI_NAMESPACE::MidiType::NoteOn);
    MIDI.disconnectCallbackFromType(MIDI_NAMESPACE::MidiType::NoteOff);

    FastLED.clear(true);

    switch (tgtMode) {
        case MODE_MIDI_LED:
            MIDI.setHandleNoteOn(handleNoteOn);
            MIDI.setHandleNoteOff(handleNoteOff);
            break;
        case MODE_SHOW_SCALE:
            setScale("Major");
            break;
        case MODE_CHECK_SCALE:
            MIDI.setHandleNoteOn(handleNoteOn);
            MIDI.setHandleNoteOff(handleNoteOff);
            setScale("Minor");
            break;

        default:
            break;
    }
}

/**
 * Clear display and show the menu.
 */
void showMenu() {
    cursor = 0;
    display.clearDisplay();
    // show menu items:
    for (int i = 0; i < MENU_SIZE; i++) {
        display.setCursor(6, i * LINE_HEIGHT);
        display.print(menu[i]);
    }
    display.setCursor(0, 0);
    display.print('>');
    display.display();
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
    display.display();

    /* L E D S */
    FastLED.addLeds<NEOPIXEL, PIN_LED_DATA>(leds, NUM_LEDS);
    FastLED.setBrightness(15);

    /* M I D I*/
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.begin();
    MIDI.turnThruOff();

    showMenu();
}

void loop() {
    buttonUp.check();
    buttonDown.check();
    buttonOk.check();

    switch (mode) {
        case MODE_MIDI_LED:
            MIDI.read();

            // FastLED.clear();
            // for (uint8_t i = 0; i < NO_OF_KEYS; i++) {
            //     if (pressedKeys[i] != 0) {
            //         leds[constrain(i, 0, NUM_LEDS - 1)] = CRGB::Orange;
            //         uint8_t offs = floor((millis() - pressedKeys[i]) / 5);
            //         leds[constrain(i + offs, 0, NUM_LEDS - 1)] = CRGB::Red;
            //         leds[constrain(i - offs, 0, NUM_LEDS - 1)] = CRGB::Red;
            //     }
            // }
            // FastLED.show();

            // L C D

            //     if (cntPressedKeysCurrent != cntPressedKeysPrevious) {
            //         display.clearDisplay();
            //         display.setCursor(0, 0);
            //         for (uint8_t i = 0; i < NO_OF_KEYS; i++) {
            //             if (pressedKeys[i] != 0) {
            //                 display.print(notes[(i + OFFSET) % 12]);
            //             }
            //         }
            //         display.display();
            //         cntPressedKeysPrevious = cntPressedKeysCurrent;
            //     }

            break;
        case MODE_SHOW_SCALE:
            break;
        case MODE_CHECK_SCALE:
            MIDI.read();
            break;
        default:
            break;
    }
}
