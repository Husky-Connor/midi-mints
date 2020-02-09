#include <Wire.h>
#include "Adafruit_MPR121.h"
#include "MIDIUSB.h"

// -------------- Cap-Touch Stuff ---------
#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif
Adafruit_MPR121 cap = Adafruit_MPR121();

// Pin state buffer
uint16_t lasttouched = 0;
uint16_t currtouched = 0;
// ----------------------------------------

// --------------- MIDI Stuff -------------
byte channel = 0;
int octave = 0;

void noteOn(byte pitch) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, 64};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush(); // Sends immediately
}

void noteOff(byte pitch) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, 64};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush(); // Sends immediately
}
// ----------------------------------------

void setup() {

  Serial.begin(115200);

  // ---------- Cap-Touch Setup ----------
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  // -------------------------------------  
}

void loop() {
  
  // Get the currently touched pads
  currtouched = cap.touched();
  // Check all pins
  for (uint8_t i=0; i<12; i++) {
    // Detect press
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      noteOn(i+48+12*octave); // NOTE ON
    }
    // Detect release
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      noteOff(i+48+12*octave); // NOTE OFF
    }
  }
  // update buffer
  lasttouched = currtouched;
  
  return;
}
