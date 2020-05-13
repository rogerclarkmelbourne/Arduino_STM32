#include <USBComposite.h>

#define LED PC13

USBMultiXBox360<2> x360;

/*
 * Sketch uses 19476 bytes (79%) of program storage space. Maximum is 24576 bytes.
Global variables use 4128 bytes (40%) of dynamic memory, leaving 6112 bytes for local variables. Maximum is 10240 bytes.

19444/4080
 */

void rumble(uint8 left, uint8 right) {
  digitalWrite(LED, left+right==0);
}

void setup() {
  pinMode(LED, OUTPUT);
  x360.registerComponent();
  USBComposite.begin();
  while (!USBComposite);
  x360.controllers[0].setRumbleCallback(rumble);
}

void loop() {
  x360.controllers[0].X(-10000);
  x360.controllers[1].Y(-10000);
  delay(500);
  x360.controllers[0].X(10000);
  x360.controllers[1].Y(10000);
  delay(500);
}

