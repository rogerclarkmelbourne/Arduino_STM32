/*
  MIDI note player

  This sketch shows how to use Serial1 (pins 7 and 8) to send MIDI
  note data.  If this circuit is connected to a MIDI synth, it will
  play the notes F#-0 (0x1E) to F#-5 (0x5A) in sequence.


  The circuit:
  * Pin 7 connected to MIDI jack pin 5
  * MIDI jack pin 2 connected to ground
  * MIDI jack pin 4 connected to +5V through 220-ohm resistor
  Attach a MIDI cable to the jack, then to a MIDI synth, and play music.

  created 13 Jun 2006
  modified 2 Jul 2009
  by Tom Igoe

  http://www.arduino.cc/en/Tutorial/MIDI

  Ported to the Maple 27 May 2010
  by Bryan Newbold
*/

void setup() {
    // Set MIDI baud rate:
    Serial1.begin(31250);
}

void loop() {
    // Play notes from F#-0 (0x1E) to F#-5 (0x5A):
    for (int note = 0x1E; note < 0x5A; note++) {
        // Note on channel 1 (0x90), some note value (note), middle
        // velocity (0x45):
        noteOn(0x90, note, 0x45);
        delay(100);
        // Note on channel 1 (0x90), some note value (note), silent
        // velocity (0x00):
        noteOn(0x90, note, 0x00);
        delay(100);
    }
}

// Plays a MIDI note.  Doesn't check to see that cmd is greater than
// 127, or that data values are less than 127:
void noteOn(int cmd, int pitch, int velocity) {
    Serial1.write(cmd);
    Serial1.write(pitch);
    Serial1.write(velocity);
}

