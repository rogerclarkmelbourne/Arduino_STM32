#include <USBComposite.h>

#define HID_SIGNED_JOYSTICK_REPORT_DESCRIPTOR(...) \
  0x05, 0x01,           /*  Usage Page (Generic Desktop) */ \
  0x09, 0x04,           /*  Usage (Joystick) */ \
  0xA1, 0x01,           /*  Collection (Application) */ \
  0x85, MACRO_GET_ARGUMENT_1_WITH_DEFAULT(HID_JOYSTICK_REPORT_ID, ## __VA_ARGS__),  /*    REPORT_ID */ \
  0x15, 0x00,           /*   Logical Minimum (0) */ \
  0x25, 0x01,           /*    Logical Maximum (1) */ \
  0x75, 0x01,           /*    Report Size (1) */ \
  0x95, 0x20,           /*    Report Count (32) */ \
  0x05, 0x09,           /*    Usage Page (Button) */ \
  0x19, 0x01,           /*    Usage Minimum (Button #1) */ \
  0x29, 0x20,           /*    Usage Maximum (Button #32) */ \
  0x81, 0x02,           /*    Input (variable,absolute) */ \
  0x15, 0x00,           /*    Logical Minimum (0) */ \
  0x25, 0x07,           /*    Logical Maximum (7) */ \
  0x35, 0x00,           /*    Physical Minimum (0) */ \
  0x46, 0x3B, 0x01,       /*    Physical Maximum (315) */ \
  0x75, 0x04,           /*    Report Size (4) */ \
  0x95, 0x01,           /*    Report Count (1) */ \
  0x65, 0x14,           /*    Unit (20) */ \
    0x05, 0x01,                     /*    Usage Page (Generic Desktop) */ \
  0x09, 0x39,           /*    Usage (Hat switch) */ \
  0x81, 0x42,           /*    Input (variable,absolute,null_state) */ \
    0x05, 0x01,                     /* Usage Page (Generic Desktop) */ \
  0x09, 0x01,           /* Usage (Pointer) */ \
    0xA1, 0x00,                     /* Collection () */ \
  0x16, 0x00, 0xFC,           /*    Logical Minimum (-1024) */ \
  0x26, 0xFF, 0x03,       /*    Logical Maximum (1023) */ \
  0x75, 0x0B,           /*    Report Size (11) */ \
  0x95, 0x04,           /*    Report Count (4) */ \
  0x09, 0x30,           /*    Usage (X) */ \
  0x09, 0x31,           /*    Usage (Y) */ \
  0x09, 0x33,           /*    Usage (Rx) */ \
  0x09, 0x34,           /*    Usage (Ry) */ \
  0x81, 0x02,           /*    Input (variable,absolute) */ \
    0xC0,                           /*  End Collection */ \
  0x15, 0x00,           /*  Logical Minimum (0) */ \
  0x26, 0xFF, 0x03,       /*  Logical Maximum (1023) */ \
  0x75, 0x0A,           /*  Report Size (10) */ \
  0x95, 0x02,           /*  Report Count (2) */ \
  0x09, 0x36,           /*  Usage (Slider) */ \
  0x09, 0x36,           /*  Usage (Slider) */ \
  0x81, 0x02,           /*  Input (variable,absolute) */ \
  0x75, 0x04,                    /*   REPORT_SIZE (4) */ \
  0x95, 0x01,                    /*   REPORT_COUNT (1) */ \
  0x81, 0x03,                    /*   OUTPUT (Cnst,Var,Abs) */ \  
  MACRO_ARGUMENT_2_TO_END(__VA_ARGS__)  \
  0xC0

typedef struct {
    uint8_t reportID;
    uint32_t buttons;
    unsigned hat:4;
    int x:11;
    int y:11;
    int rx:11;
    int ry:11;
    unsigned sliderLeft:10;
    unsigned sliderRight:10;
    unsigned unused:4;
} __packed SignedJoystickReport_t;

class HIDSignedJoystick : public HIDReporter {
public:
  SignedJoystickReport_t joyReport; 
  void begin(void) {};
  void end(void) {};
  HIDSignedJoystick(USBHID& HID, uint8_t reportID=HID_JOYSTICK_REPORT_ID) 
            : HIDReporter(HID, NULL, (uint8_t*)&joyReport, sizeof(joyReport), reportID) {
        joyReport.buttons = 0;
        joyReport.hat = 15;
        joyReport.x = 0;
        joyReport.y = 0;
        joyReport.rx = 0;
        joyReport.ry = 0;
        joyReport.sliderLeft = 0;
        joyReport.sliderRight = 0;
    }
};

USBHID HID;
HIDSignedJoystick joy(HID);

uint8 signedJoyReportDescriptor[] = {
  HID_SIGNED_JOYSTICK_REPORT_DESCRIPTOR()
};

void setup() {
  HID.setReportDescriptor(signedJoyReportDescriptor, sizeof(signedJoyReportDescriptor));
  HID.registerComponent();
  USBComposite.begin();  
  while (!USBComposite);
}

void loop() {
  joy.joyReport.buttons = 1 | 2;
  joy.joyReport.x = -1024;
  joy.joyReport.y = -1024;
  joy.joyReport.rx = -1024;
  joy.joyReport.ry = -1024; 
  joy.sendReport(); 
  delay(500);
  joy.joyReport.buttons = 0;
  joy.joyReport.x = 1023;
  joy.joyReport.y = 1023;
  joy.joyReport.rx = 1023;
  joy.joyReport.ry = 1023;  
  joy.sendReport(); 
  delay(500);
}

