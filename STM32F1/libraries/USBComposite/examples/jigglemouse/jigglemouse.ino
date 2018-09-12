#include <USBComposite.h>

#define LED PB12

USBHID HID;
HIDMouse Mouse(HID);

void setup(){
  pinMode(LED,OUTPUT);
  digitalWrite(LED,1);
  HID.begin(HID_MOUSE);
  delay(1000);
}

void loop(){
  digitalWrite(LED,0);
  Mouse.move(4,0);
  delay(500);
  digitalWrite(LED,1);    
  delay(30000);
  digitalWrite(LED,0);
  Mouse.move(-4,0);
  delay(500);
  digitalWrite(LED,1);
  delay(30000);
}
