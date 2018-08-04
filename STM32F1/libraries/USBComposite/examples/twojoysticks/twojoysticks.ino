#include <USBComposite.h>

const uint8_t reportDescription[] = {
   HID_MOUSE_REPORT_DESCRIPTOR(),
   HID_KEYBOARD_REPORT_DESCRIPTOR(),
   HID_JOYSTICK_REPORT_DESCRIPTOR(),
   HID_JOYSTICK_REPORT_DESCRIPTOR(HID_JOYSTICK_REPORT_ID+1),
};

HIDJoystick Joystick2(HID_JOYSTICK_REPORT_ID+1);

void setup(){
  USBHID_begin_with_serial(reportDescription, sizeof(reportDescription));
  Joystick.setManualReportMode(true);
  Joystick2.setManualReportMode(true);
}

void loop(){
  Joystick.X(0);
  Joystick.Y(0);
  Joystick.sliderRight(1023);
  Joystick.send();
  delay(400);
  Joystick.X(1023);
  Joystick.Y(1023);
  Joystick.sliderRight(0);
  Joystick.send();
  delay(400);
  Joystick2.X(0);
  Joystick2.Y(0);
  Joystick2.sliderRight(1023);
  Joystick2.send();
  delay(400);
  Joystick2.X(1023);
  Joystick2.Y(1023);
  Joystick2.sliderRight(0);
  Joystick2.send();
  delay(400);
}

