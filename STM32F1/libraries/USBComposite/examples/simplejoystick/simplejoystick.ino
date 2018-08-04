#include <USBComposite.h>

void setup() {
  USBHID_begin_with_serial(HID_JOYSTICK);
}

void loop() {
  Joystick.X(0);
  delay(500);
  Joystick.X(1023);
  delay(500);
}

