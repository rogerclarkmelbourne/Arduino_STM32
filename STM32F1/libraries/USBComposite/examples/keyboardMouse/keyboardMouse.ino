#include <USBComposite.h>
/*
 * This example types which cardinal direction the mouse will be moved
 * and then moves the mouse in that direction. If the mouse doesn't recenter
 * at the end, it is probably due to mouse acceleration 
 */

USBHID HID;
HIDKeyboard Keyboard(HID);
HIDMouse Mouse(HID); 
 
void setup() {
  HID.begin(HID_KEYBOARD_MOUSE);
  delay(2000);
}

void loop() {
  Keyboard.println("UP");
  Mouse.move(0,-127);
  delay(1500);

  Keyboard.println("DOWN");
  Mouse.move(0,127);
  Mouse.move(0,127);  //Movement cannot be greater than a byte
  delay(1500);

  Keyboard.println("LEFT");
  Mouse.move(-127,-127);
  delay(1500);

  Keyboard.println("RIGHT");
  Mouse.move(127,0);
  Mouse.move(127,0);  //Movement cannot be greater than a byte
  delay(1500);

  Keyboard.println("CENTER");
  Mouse.move(-127,0);
  delay(500);

  delay(5000);
}
