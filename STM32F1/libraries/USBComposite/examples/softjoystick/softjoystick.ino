// This is a silly project: you send a feature
// report from the PC, and it becomes a joystick setting.
// I guess it's not completely useless as it lets you
// get vJoy functionality without special vJoy drivers.

#include <USBComposite.h>

#define DATA_SIZE (sizeof(JoystickReport_t)-1)

USBHID HID;

class HIDJoystickRawData : public HIDJoystick {
  private:
    uint8_t featureData[HID_BUFFER_ALLOCATE_SIZE(DATA_SIZE,1)];
    HIDBuffer_t fb { featureData, HID_BUFFER_SIZE(DATA_SIZE,1), HID_JOYSTICK_REPORT_ID }; 
    USBHID HID;
  public:
    HIDJoystickRawData(USBHID& _HID, uint8_t reportID=HID_JOYSTICK_REPORT_ID) : HIDJoystick(HID, reportID) {}
    
    void begin() {
      HID.setFeatureBuffers(&fb, 1);
    }
    
    void setRawData(JoystickReport_t* p) {
      joyReport = *p;
      send();
    }
};

HIDJoystickRawData joy(HID);
JoystickReport_t report = {HID_JOYSTICK_REPORT_ID};

const uint8_t reportDescription[] = {
   HID_JOYSTICK_REPORT_DESCRIPTOR(HID_JOYSTICK_REPORT_ID, 
        HID_FEATURE_REPORT_DESCRIPTOR(DATA_SIZE))
};

void setup() {
  HID.begin(reportDescription, sizeof(reportDescription));
  joy.begin();
}

void loop() {
  if (joy.getFeature(1+(uint8_t*)&report)) {
    joy.setRawData(&report);
  }
  
  delay(5);
}

