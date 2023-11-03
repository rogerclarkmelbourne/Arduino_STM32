/* Copyright (c) 2011, Peter Barrett
**
** Permission to use, copy, modify, and/or distribute this software for
** any purpose with or without fee is hereby granted, provided that the
** above copyright notice and this permission notice appear in all copies.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
** SOFTWARE.
*/

#include <Arduino.h>
#include "USBComposite.h" 

void USBXBox360WController::send(void){
    if (!connected)
        connect(true);
    report.header[0] = 0x00;
    report.header[1] = 0x01;
    report.header[2] = 0x00;
    report.header[3] = 0xf0;
    sendData(&report, sizeof(report));
}

uint16_t USBXBox360WController::getReportSize(void){
    return sizeof(report);
}

uint8_t* USBXBox360WController::getReport(void){
    return (uint8_t*)&report;
}


const uint8 startup[] = {0x00,0x0F,0x00,0xF0,0xF0,0xCC,0x42,0xAF,0x3C,0x60,0xAC,0x24,0xFB,0x50,0x00,0x05,0x13,0xE7,0x20,0x1D,0x30,0x03,0x40,0x01,0x50,0x01,0xFF,0xFF,0xFF };

bool USBXBox360WController::connect(bool state) {
//    delay(250);
    report.header[0] = 0x08;
    report.header[1] = state ? 0x80 : 0x00;
    if (!sendData(&report.header, 2))
        return false;
//    delay(250);
    if (state && !sendData(startup, 29))
        return false;
//    delay(250);
    connected = state;
    return true;
}

void USBXBox360WController::setRumbleCallback(void (*callback)(uint8 left, uint8 right)) {
    x360_set_rumble_callback(controller,callback);
}

void USBXBox360WController::setLEDCallback(void (*callback)(uint8 pattern)) {
    x360_set_led_callback(controller, callback);
}

void USBXBox360WController::stop(void){
	setRumbleCallback(NULL);
	setLEDCallback(NULL);
}

void USBXBox360WController::safeSendReport() {	
    if (!manualReport) 
        send();
}

void USBXBox360WController::button(uint8_t button, bool val){
	uint16_t mask = (1 << (button-1));
	if (val) {
        report.buttons |= mask;
	} else {
		report.buttons &= ~mask;
	}
	
    safeSendReport();
}

void USBXBox360WController::buttons(uint16_t buttons){
    report.buttons = buttons;
	
    safeSendReport();
}

void USBXBox360WController::X(int16_t val){
    report.x = val;
		
    safeSendReport();
}

void USBXBox360WController::Y(int16_t val){
    report.y = val;
		
    safeSendReport();
}

void USBXBox360WController::XRight(int16_t val){
    report.rx = val;
		
    safeSendReport();
}

void USBXBox360WController::YRight(int16_t val){
    report.ry = val;
		
    safeSendReport();
}

void USBXBox360WController::position(int16_t x, int16_t y){
    report.x = x;
    report.y = y;
		
    safeSendReport();
}

void USBXBox360WController::positionRight(int16_t x, int16_t y){
    report.rx = x;
    report.ry = y;
		
    safeSendReport();
}

void USBXBox360WController::sliderLeft(uint8_t val){
    report.sliderLeft = val;
	
    safeSendReport();
}

void USBXBox360WController::sliderRight(uint8_t val){
    report.sliderRight = val;
	
    safeSendReport();
}

