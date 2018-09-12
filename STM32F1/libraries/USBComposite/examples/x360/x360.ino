#include <USBComposite.h>

USBXBox360 XBox360;

void setup() {
  XBox360.begin();
  delay(1000);
}

void loop() {
  XBox360.X(-32767);
  XBox360.Y(-32767);
  delay(1000);  
  XBox360.X(32767);
  XBox360.Y(32767);
  delay(1000);  
}
