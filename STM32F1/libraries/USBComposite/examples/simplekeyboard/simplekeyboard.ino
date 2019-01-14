#include <USBComposite.h>

USBHID HID;
HIDKeyboard Keyboard(HID);

void setup() {
  HID.begin(HID_KEYBOARD);
  Keyboard.begin(); // useful to detect host capslock state and LEDs
  delay(1000);
}

void loop() {
  Keyboard.println("Hello world");
  delay(10000);
}

