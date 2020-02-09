#include <Wire.h>
#include "Adafruit_MPR121.h"
#include "MIDIUSB.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "note_frames.h"

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

void noteOn(uint8_t pitch) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, 64};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush(); // Sends immediately
}

void noteOff(uint8_t pitch) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, 64};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush(); // Sends immediately
}
// ----------------------------------------

// ----------- LED Matrix Stuff -----------
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
void dispNote(uint8_t pitch) {
  matrix.clear();
  switch(pitch % 12) {
    case 0:
      matrix.drawBitmap(0, 0, c_bmp, 8, 8, LED_RED);
      break;
    case 1:
      matrix.drawBitmap(0, 0, df_bmp, 8, 8, LED_RED);
      break;
    case 2:
      matrix.drawBitmap(0, 0, d_bmp, 8, 8, LED_RED);
      break;
    case 3:
      matrix.drawBitmap(0, 0, ef_bmp, 8, 8, LED_RED);
      break;
    case 4:
      matrix.drawBitmap(0, 0, e_bmp, 8, 8, LED_RED);
      break;
    case 5:
      matrix.drawBitmap(0, 0, f_bmp, 8, 8, LED_RED);
      break;
    case 6:
      matrix.drawBitmap(0, 0, gf_bmp, 8, 8, LED_RED);
      break;
    case 7:
      matrix.drawBitmap(0, 0, g_bmp, 8, 8, LED_RED);
      break;
    case 8:
      matrix.drawBitmap(0, 0, af_bmp, 8, 8, LED_RED);
      break;
    case 9:
      matrix.drawBitmap(0, 0, a_bmp, 8, 8, LED_RED);
      break;
    case 10:
      matrix.drawBitmap(0, 0, bf_bmp, 8, 8, LED_RED);
      break;
    case 11:
      matrix.drawBitmap(0, 0, b_bmp, 8, 8, LED_RED);
      break;
    default:
      matrix.clear();
    matrix.setRotation(3);
    matrix.writeDisplay();
  }
}
void clearScreen() {
  matrix.clear();
  matrix.writeDisplay();
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

  // -------- 8x8 LED Matrix Setup -------
  matrix.begin(0x70);
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
      dispNote(i+48+12*octave);
    }
    // Detect release
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      noteOff(i+48+12*octave); // NOTE OFF
      clearScreen();
    }
  }
  // update buffer
  lasttouched = currtouched;
  
  return;
}
