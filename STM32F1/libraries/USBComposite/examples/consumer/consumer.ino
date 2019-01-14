#include <USBComposite.h>

USBHID HID;

const uint8_t reportDescription[] = {
   HID_CONSUMER_REPORT_DESCRIPTOR()
};

HIDConsumer Consumer(HID);

void setup(){
  HID.begin(reportDescription, sizeof(reportDescription));
}

void loop() {
  Consumer.press(HIDConsumer::BRIGHTNESS_DOWN);
  Consumer.release();
  delay(500);
}

