#include <USBComposite.h>

USBHID HID;
HIDDigitizer digitizer(HID);

void setup(){
  HID.begin();
  while (!USBComposite);
  digitizer.move(0,0);
  delay(1000);
}

void loop(){
  digitizer.move(2000,3000);
  digitizer.press(DIGITIZER_TOUCH_IN_RANGE);
  delay(1000);
  digitizer.move(4000,4000);
  digitizer.release(DIGITIZER_TOUCH_IN_RANGE);
  delay(1000);
}
