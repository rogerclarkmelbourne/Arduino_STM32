#include <USBHID.h>

#define LED PB12

void setup(){
  pinMode(LED,OUTPUT);
  digitalWrite(LED,1);
  USBHID_begin_with_serial(HID_MOUSE);
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
