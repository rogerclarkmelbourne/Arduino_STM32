#include "USBHID.h"

void HIDConsumer::begin(void) {}
void HIDConsumer::end(void) {}
void HIDConsumer::press(uint16_t button) {
    report.button = button;
    sendReport();
}

void HIDConsumer::release() {
    report.button = 0;
    sendReport();
}
