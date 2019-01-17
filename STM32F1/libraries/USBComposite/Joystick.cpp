#include "USBComposite.h" 

// This code requires gcc on low-endian devices.

//================================================================================
//================================================================================
//	Joystick

void HIDJoystick::begin(void){
}

void HIDJoystick::end(void){
}

void HIDJoystick::setManualReportMode(bool mode) {
    manualReport = mode;
}

bool HIDJoystick::getManualReportMode() {
    return manualReport;
}

void HIDJoystick::safeSendReport() {	
    if (!manualReport) {
        sendReport();
    }
}

void HIDJoystick::button(uint8_t button, bool val){
	uint32_t mask = ((uint32_t)1 << (button-1));

	if (val) {
        joyReport.buttons |= mask;
	} else {
        joyReport.buttons &= ~mask;
	}
	
    safeSendReport();
}

void HIDJoystick::X(uint16_t val){
	if (val > 1023) val = 1023;
    joyReport.x = val;
		
    safeSendReport();
}

void HIDJoystick::Y(uint16_t val){
	if (val > 1023) val = 1023;
    joyReport.y = val;
			
    safeSendReport();
}

void HIDJoystick::position(uint16_t x, uint16_t y){
	if (x > 1023) x = 1023;
	if (y > 1023) y = 1023;
    joyReport.x = x;
    joyReport.y = y;
	
    safeSendReport();
}

void HIDJoystick::Xrotate(uint16_t val){
	if (val > 1023) val = 1023;
    joyReport.rx = val;
	
    safeSendReport();
}

void HIDJoystick::Yrotate(uint16_t val){
	if (val > 1023) val = 1023;
    joyReport.ry = val;
	
    safeSendReport();
}

void HIDJoystick::sliderLeft(uint16_t val){
	if (val > 1023) val = 1023;
    joyReport.sliderLeft = val;	
	
    safeSendReport();
}

void HIDJoystick::sliderRight(uint16_t val){
	if (val > 1023) val = 1023;
    joyReport.sliderRight = val;	
	
    safeSendReport();
}

void HIDJoystick::slider(uint16_t val){
	if (val > 1023) val = 1023;
    joyReport.sliderLeft = val;	
    joyReport.sliderRight = val;	
	
    safeSendReport();
}

void HIDJoystick::hat(int16_t dir){
	uint8_t val;
	if (dir < 0) val = 15;
	else if (dir < 23) val = 0;
	else if (dir < 68) val = 1;
	else if (dir < 113) val = 2;
	else if (dir < 158) val = 3;
	else if (dir < 203) val = 4;
	else if (dir < 245) val = 5;
	else if (dir < 293) val = 6;
	else if (dir < 338) val = 7;
    else val = 15;
    
    joyReport.hat = val;
	
    safeSendReport();
}

