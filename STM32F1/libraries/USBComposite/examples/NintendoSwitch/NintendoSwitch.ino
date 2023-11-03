#include <USBComposite.h>

USBHID HID;
HIDSwitchController controller(HID);

void setup() {
  controller.begin();  
  while (!USBComposite);
  delay(1000);
}

void loop() {
  controller.X(16);
  controller.send();
  delay(300);
  controller.X(255-16);
  controller.send();
  delay(300);
  controller.X(128);
  controller.send();
  delay(300);
  /*
   for (int i=0; i<=0; i++) {
    controller.button(i,true);
    controller.sendReport();
    delay(100);
    controller.button(i,false);
    controller.sendReport();
    delay(100);
  } */
}
