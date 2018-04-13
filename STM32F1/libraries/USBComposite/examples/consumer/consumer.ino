#include <USBComposite.h>

const uint8_t reportDescription[] = {
   HID_CONSUMER_REPORT_DESCRIPTOR()
};

HIDConsumer Consumer;

void setup(){
  USBHID_begin_with_serial(reportDescription, sizeof(reportDescription));
}

void loop() {
  Consumer.press(HIDConsumer::BRIGHTNESS_DOWN);
  Consumer.release();
  delay(500);
}

