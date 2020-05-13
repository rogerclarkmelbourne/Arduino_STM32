#include <USBComposite.h>

#define LED PC13

USBXBox360W<4> x360;

/*
 * Sketch uses 19428 bytes (79%) of program storage space. Maximum is 24576 bytes.
 * Global variables use 4144 bytes (40%) of dynamic memory, leaving 6096 bytes for local variables. Maximum is 10240 bytes.
 */

void rumble(uint8 left, uint8 right) {
  digitalWrite(LED, left+right==0);
}

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 1);
  x360.begin();
  while (!USBComposite);
  x360.controllers[0].setRumbleCallback(rumble);
  // the next two lines are not necessary as the library will
  // autoconnect when data is sent
  for(int i=0; i<2 ; i++) x360.controllers[i].connect(true);  
  for(int i=2; i<4 ; i++) x360.controllers[i].connect(false);
  x360.controllers[0].buttons(0);
  x360.controllers[1].buttons(0);
}

void loop() {
  x360.controllers[0].X(-10000);
  x360.controllers[1].Y(-10000);
  delay(500);
  x360.controllers[0].X(10000);
  x360.controllers[1].Y(10000);
  delay(500);
}

