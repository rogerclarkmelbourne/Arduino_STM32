#include <USBComposite.h>

USBHID HID;
HIDKeyboard Keyboard(HID);

void setup() {
  HID.begin(HID_KEYBOARD);
  while (!USBComposite);
  Keyboard.begin(); // useful to detect host capslock state and LEDs
}

void loop() {
  Keyboard.println("Hello world");
  delay(10000);
}

