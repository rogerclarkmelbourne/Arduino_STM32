#include "USBHID.h"

#define REPORT(name, ...) \
    static uint8_t raw_ ## name[] = { __VA_ARGS__ }; \
    static const HIDReportDescriptor desc_ ## name = { raw_ ##name, sizeof(raw_ ##name) }; \
    const HIDReportDescriptor* hidReport ## name = & desc_ ## name;

REPORT(KeyboardMouseJoystick, HID_MOUSE_REPORT_DESCRIPTOR(), HID_KEYBOARD_REPORT_DESCRIPTOR(), HID_JOYSTICK_REPORT_DESCRIPTOR());
REPORT(KeyboardMouse, HID_MOUSE_REPORT_DESCRIPTOR(), HID_KEYBOARD_REPORT_DESCRIPTOR());
REPORT(Keyboard, HID_KEYBOARD_REPORT_DESCRIPTOR());
REPORT(Mouse, HID_MOUSE_REPORT_DESCRIPTOR());
REPORT(KeyboardJoystick, HID_KEYBOARD_REPORT_DESCRIPTOR(), HID_JOYSTICK_REPORT_DESCRIPTOR());
REPORT(Joystick, HID_JOYSTICK_REPORT_DESCRIPTOR());
REPORT(BootKeyboard, HID_BOOT_KEYBOARD_REPORT_DESCRIPTOR());
