#include <MIDI.h>

// // MIDI_CREATE_DEFAULT_INSTANCE();
// // Override default baud rate to be compatible with hairless midi
// struct CustomBaudRateSettings : public MIDI_NAMESPACE::DefaultSerialSettings {
//     static const long BaudRate = 115200;
// };

// // Set SysExMaxSize, else Note Offs are not processed correctly sometimes
// struct MidiSettings : public midi::DefaultSettings {
//     // static const unsigned SysExMaxSize = 1024;
//     static const bool Use1ByteParsing = false;
//     // static const bool UseRunningStatus = true;
// };

// // create midi instance
// MIDI_NAMESPACE::SerialMIDI<HardwareSerial, CustomBaudRateSettings> serialMIDI(Serial);
// MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial, CustomBaudRateSettings>, MidiSettings> MIDI((MIDI_NAMESPACE::SerialMIDI<HardwareSerial, CustomBaudRateSettings>&)serialMIDI);
MIDI_CREATE_DEFAULT_INSTANCE();