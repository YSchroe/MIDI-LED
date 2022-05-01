# MIDI LEDs

Midi LEDS

## Opto Coupler 6N137

-   Wiring based on [this thread](https://electronics.stackexchange.com/questions/317622/how-to-get-3-3v-output-from-vishay-6n137-high-speed-opto-coupler).
-   The 5V input on PIN7 can be dropped, since the opto coupler behaves the regardless if 5V is connected or not. Only behaves differently if ground is connected to that pin (see vishay datasheet truth table).

https://wokwi.com/projects/330435484301918804
