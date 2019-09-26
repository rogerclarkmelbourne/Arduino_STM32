#include <USBComposite.h>

USBHID HID;
HIDKeyboard Keyboard(HID);
USBCompositeSerial CompositeSerial;

void setup() {
  HID.begin(CompositeSerial, HID_KEYBOARD);
  Keyboard.begin(); // needed for LED support
  delay(1000);
}

void loop() {
  CompositeSerial.println(Keyboard.getLEDs()); 
}


