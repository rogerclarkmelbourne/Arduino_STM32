#include <USBComposite.h>

USBMultiSerial<2> ms;

void setup() {
  ms.begin();
  while (!USBComposite);
}

void loop() {
  while(ms.ports[1].available()) {
    ms.ports[0].write(ms.ports[1].read());
  }
  while(ms.ports[0].available()) {
    ms.ports[1].write(ms.ports[0].read());
  } 
}
