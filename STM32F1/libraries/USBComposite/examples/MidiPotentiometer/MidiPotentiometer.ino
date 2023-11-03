#include <USBComposite.h>

USBMIDI midi;
const uint8 pot_pin = 6;
const uint8 threshold = 1;

const unsigned int midi_channel =
    1; // this might show up as channel 1 depending on start index
const unsigned int cc_command = 0; // bank select command

unsigned int old_value = 0;
unsigned int new_value = 0;

void setup() {

  // product id taken from library example
  USBComposite.setProductId(0x0031);
  pinMode(pot_pin, INPUT);
  midi.begin();
  delay(1000);
}

void loop() {

  int temp = analogRead(pot_pin); // a value between 0-4095
  new_value = temp / 32;          // convert to a value between 0-127

  // If difference between new_value and old_value is grater than threshold
  if ((new_value > old_value && new_value - old_value > threshold) ||
      (new_value < old_value && old_value - new_value > threshold)) {

    midi.sendControlChange(midi_channel, cc_command, new_value);

    // Update old_value
    old_value = new_value;
  }
  // Wait 50ms before reading the pin again
  delay(50);
}
