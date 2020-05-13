#include <USBComposite.h>

USBHID HID;

const uint8_t reportDescription[] = {
   HID_CONSUMER_REPORT_DESCRIPTOR(),
   HID_KEYBOARD_REPORT_DESCRIPTOR()
};

HIDConsumer Consumer(HID);
HIDKeyboard Keyboard(HID);

void setup(){
  HID.begin(reportDescription, sizeof(reportDescription));
}

void loop() {
  Consumer.press(HIDConsumer::BRIGHTNESS_DOWN);
  Consumer.release();
  delay(500);
  Consumer.press(HIDConsumer::BRIGHTNESS_DOWN);
  Consumer.release();
  delay(500);
  Keyboard.press(KEY_RIGHT_ARROW);
  Keyboard.release(KEY_RIGHT_ARROW);
  delay(500);  
}

