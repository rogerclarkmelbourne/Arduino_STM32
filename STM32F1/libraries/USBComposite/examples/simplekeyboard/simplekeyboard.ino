#include <USBComposite.h>

void setup() {
  USBHID_begin_with_serial(HID_KEYBOARD);
  Keyboard.begin(); // useful to detect host capslock state and LEDs
  delay(1000);
}

void loop() {
  Keyboard.println("Hello world");
  delay(10000);
}

