#include <USBComposite.h>


void setup() {
  USBHID_begin_with_serial(HID_KEYBOARD);
  Keyboard.begin(); // needed for LED support
  delay(1000);
}

void loop() {
  CompositeSerial.println(Keyboard.getLEDs()); 
}


