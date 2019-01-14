#include <USBComposite.h>

#define TXSIZE 256
#define RXSIZE 300

USBHID HID;
HIDRaw<TXSIZE,RXSIZE> raw(HID);
uint8 buf[RXSIZE];

const uint8_t reportDescription[] = {
   HID_RAW_REPORT_DESCRIPTOR(TXSIZE,RXSIZE)
};

void setup(){
  HID.begin(reportDescription, sizeof(reportDescription));  
  raw.begin();
}

void loop() {
  if (raw.getOutput(buf)) {
    for (int i=0;i<RXSIZE;i++) buf[i]++;
    raw.send(buf+RXSIZE-min(RXSIZE,TXSIZE),min(RXSIZE,TXSIZE));
  }
}

