#include <USBComposite.h>

const uint8_t reportDescription[] = {
  HID_ABS_MOUSE_REPORT_DESCRIPTOR(HID_MOUSE_REPORT_ID)
};

USBHID HID;
HIDAbsMouse mouse(HID);

void setup(){
  HID.begin(reportDescription, sizeof(reportDescription));
  delay(1000);
  mouse.move(0,0);
  delay(1000);
  mouse.press(MOUSE_LEFT);
  mouse.move(500,500);
  mouse.release(MOUSE_ALL);
  mouse.click(MOUSE_RIGHT);
}

void loop(){
  mouse.move(0,0);
  delay(1000);
  mouse.move(16384,16384);
  delay(1000);
}
