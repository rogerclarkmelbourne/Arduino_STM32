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

/**
 * @brief USB HID Keyboard device 
 */

#include <string.h>
#include <stdint.h>
#include <libmaple/nvic.h>
#include <libmaple/usb.h>
#include "USBXBox360.h" 
#include "usb_x360.h"

void USBXBox360::sendReport(void){
	x360_tx(xbox360_Report, sizeof(xbox360_Report));
	
	while (x360_is_transmitting() != 0) {
    }
	/* flush out to avoid having the pc wait for more data */
	x360_tx(NULL, 0);
}

void USBXBox360::setRumbleCallback(void (*callback)(uint8 left, uint8 right)) {
    x360_set_rumble_callback(callback);
}

void USBXBox360::setLEDCallback(void (*callback)(uint8 pattern)) {
    x360_set_led_callback(callback);
}


bool USBXBox360::init(void* ignore) {
	(void)ignore;
	usb_generic_set_info(0x045e, 0x028e, NULL, NULL, NULL);
	return true;
}

bool USBXBox360::registerComponent() {
	return USBComposite.add(&usbX360Part, this, init);
}

void USBXBox360::begin(void){
	if(!enabled){
		USBComposite.clear();
		registerComponent();
		USBComposite.begin();

		enabled = true;
	}
}

void USBXBox360::end() {
	if (enabled) {
		enabled = false;
        USBComposite.end();
	}
}

void USBXBox360::stop(void){
	setRumbleCallback(NULL);
	setLEDCallback(NULL);
}

void USBXBox360::setManualReportMode(bool mode) {
    manualReport = mode;
}

bool USBXBox360::getManualReportMode() {
    return manualReport;
}

void USBXBox360::safeSendReport() {	
    if (!manualReport) {
        while (x360_is_transmitting() != 0) {
        }
        sendReport();
    }
}

void USBXBox360::send() {
    while (x360_is_transmitting() != 0) {
    }
    sendReport();
}
    
void USBXBox360::button(uint8_t button, bool val){
	button--;
	uint8_t mask = (1 << (button & 7));
	if (val) {
		if (button < 8) xbox360_Report[2] |= mask;
		else if (button < 16) xbox360_Report[3] |= mask;
	} else {
		mask = ~mask;
		if (button < 8) xbox360_Report[2] &= mask;
		else if (button < 16) xbox360_Report[3] &= mask;
	}
	
    safeSendReport();
}

void USBXBox360::X(int16_t val){
	xbox360_Report[6] = val;
    xbox360_Report[7] = (uint16)val >> 8;
		
    safeSendReport();
}

void USBXBox360::Y(int16_t val){
	xbox360_Report[8] = val;
    xbox360_Report[9] = (uint16)val >> 8;
		
    safeSendReport();
}

void USBXBox360::XRight(int16_t val){
	xbox360_Report[0xA] = val;
    xbox360_Report[0xB] = (uint16)val >> 8;
		
    safeSendReport();
}

void USBXBox360::YRight(int16_t val){
	xbox360_Report[0xC] = val;
    xbox360_Report[0xD] = (uint16)val >> 8;
		
    safeSendReport();
}

void USBXBox360::position(int16_t x, int16_t y){
	xbox360_Report[6] = x;
    xbox360_Report[7] = (uint16)x >> 8;
	xbox360_Report[8] = y;
    xbox360_Report[9] = (uint16)y >> 8;
		
    safeSendReport();
}

void USBXBox360::positionRight(int16_t x, int16_t y){
	xbox360_Report[0xA] = x;
    xbox360_Report[0xB] = (uint16)x >> 8;
	xbox360_Report[0xC] = y;
    xbox360_Report[0xD] = (uint16)y >> 8;
		
    safeSendReport();
}

void USBXBox360::sliderLeft(uint8_t val){
	xbox360_Report[4] = val;
	
    safeSendReport();
}

void USBXBox360::sliderRight(uint8_t val){
	xbox360_Report[5] = val;
	
    safeSendReport();
}

USBXBox360 XBox360;
