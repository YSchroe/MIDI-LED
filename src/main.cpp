#include <AceButton.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>
#include <MIDI.h>
#include <Menu.h>
#include <Pins.h>
#include <Scales.h>

enum LedMode : unsigned int {
    LED_MODE_MIDI_LED = 0,
    LED_MODE_SHOW_SCALE,
    LED_MODE_CHECK_SCALE
};

struct Settings {
    bool showNotesInDisplay = true;
    LedMode ledMode = LedMode::LED_MODE_MIDI_LED;
} settings;

MIDI_CREATE_DEFAULT_INSTANCE();

ace_button::AceButton buttonUp(PIN_BTN_UP);
ace_button::AceButton buttonDown(PIN_BTN_DOWN);
ace_button::AceButton buttonOk(PIN_BTN_OK);

Scale selectedScale = scales[0];

// Declaration for an SSD1306 display connected via I2C (SDA, SCL pins)
Adafruit_SSD1306 display(128, 64, &Wire, -1);
Menu menu(&display);

/* LED STRIP CFG */
#define NUM_LEDS 144
CRGB leds[NUM_LEDS];
// _______________

/* GLOBALS */
#define OFFSET 36  // Lowest note playable by piano
#define NO_OF_KEYS 61

unsigned long pressedKeys[NO_OF_KEYS] = {};
uint8_t cntPressedKeysPrevious = 0;
uint8_t cntPressedKeysCurrent = 0;

/* CALLBACK FUNCTIONS */
void handleNoteOn(byte channel, byte pitch, byte velocity) {
    byte correctedPitch = pitch - OFFSET;
    CRGB color;
    if (settings.ledMode == LedMode::LED_MODE_MIDI_LED) {
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
        if (button->getPin() == PIN_BTN_UP) {
            menu.moveCursor(1);
        } else if (button->getPin() == PIN_BTN_DOWN) {
            menu.moveCursor(-1);
        } else if (button->getPin() == PIN_BTN_OK) {
            menu.execute();
        }
    }
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

    if (settings.ledMode == LedMode::LED_MODE_SHOW_SCALE) {
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

void switchMode(LedMode tgtMode) {
    MIDI.disconnectCallbackFromType(MIDI_NAMESPACE::MidiType::NoteOn);
    MIDI.disconnectCallbackFromType(MIDI_NAMESPACE::MidiType::NoteOff);

    FastLED.clear(true);

    switch (tgtMode) {
        case LedMode::LED_MODE_MIDI_LED:
            MIDI.setHandleNoteOn(handleNoteOn);
            MIDI.setHandleNoteOff(handleNoteOff);
            break;
        case LedMode::LED_MODE_SHOW_SCALE:
            setScale("Major");
            break;
        case LedMode::LED_MODE_CHECK_SCALE:
            MIDI.setHandleNoteOn(handleNoteOn);
            MIDI.setHandleNoteOff(handleNoteOff);
            setScale("Minor");
            break;

        default:
            break;
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
}

void loop() {
    buttonUp.check();
    buttonDown.check();
    buttonOk.check();

    switch (settings.ledMode) {
        case LedMode::LED_MODE_MIDI_LED:
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
            if (settings.showNotesInDisplay) {
                if (cntPressedKeysCurrent != cntPressedKeysPrevious) {
                    display.clearDisplay();
                    display.setCursor(0, 0);
                    for (uint8_t i = 0; i < NO_OF_KEYS; i++) {
                        if (pressedKeys[i] != 0) {
                            display.print(notes[(i + OFFSET) % 12]);
                        }
                    }
                    display.display();
                    cntPressedKeysPrevious = cntPressedKeysCurrent;
                }
            }

            break;
        case LedMode::LED_MODE_SHOW_SCALE:
            break;
        case LedMode::LED_MODE_CHECK_SCALE:
            MIDI.read();
            break;
        default:
            break;
    }
}
