#include <USBHID.h>

void setup() 
{
    USBHID_begin_with_serial(HID_BOOT_KEYBOARD);
    BootKeyboard.begin(); // needed just in case you need LED support
}

void loop() 
{
  BootKeyboard.press(KEY_F12);
  delay(100);
  BootKeyboard.release(KEY_F12);
  delay(1000);
}
