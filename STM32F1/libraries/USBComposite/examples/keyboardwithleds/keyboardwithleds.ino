#include <USBComposite.h>

USBHID HID;
HIDKeyboard Keyboard(HID);
USBCompositeSerial CompositeSerial;

void setup() {
  HID.begin(CompositeSerial, HID_KEYBOARD);
  while (!USBComposite);
  Keyboard.begin(); // needed for LED support
}

void loop() {
  CompositeSerial.println(Keyboard.getLEDs()); 
}


