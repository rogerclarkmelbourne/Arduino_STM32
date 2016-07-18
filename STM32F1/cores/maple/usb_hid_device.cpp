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
 
#if defined(USB_HID_KMJ) || defined(USB_HID_KM) || defined(USB_HID_J)

#include <usb_hid_device.h>

#include <string.h>
#include <stdint.h>
#include <libmaple/nvic.h>
#include <libmaple/usb_hid.h>
#include <libmaple/usb.h>

#include <wirish.h>

/*
 * USB HID interface
 */

#define USB_TIMEOUT 50

HIDDevice::HIDDevice(void){
	
}

void HIDDevice::begin(void){
	if(!enabled){
		usb_hid_enable(BOARD_USB_DISC_DEV, BOARD_USB_DISC_BIT);
		enabled = true;
	}
}

void HIDDevice::end(void){
	if(enabled){
	    usb_hid_disable(BOARD_USB_DISC_DEV, BOARD_USB_DISC_BIT);
		enabled = false;
	}
}


//================================================================================
//================================================================================
//	Mouse

HIDMouse::HIDMouse(void) : _buttons(0){
}

void HIDMouse::begin(void){
	HID.begin();
}

void HIDMouse::end(void){
	HID.end();
}

void HIDMouse::click(uint8_t b)
{
	_buttons = b;
	while (usb_hid_is_transmitting() != 0) {
    }
	move(0,0,0);
	_buttons = 0;
	while (usb_hid_is_transmitting() != 0) {
    }
	move(0,0,0);
}

void HIDMouse::move(signed char x, signed char y, signed char wheel)
{
	uint8_t m[4];
	m[0] = _buttons;
	m[1] = x;
	m[2] = y;
	m[3] = wheel;
	
	uint8_t buf[1+sizeof(m)];
	buf[0] = 1;//report id
	uint8_t i;
	for(i=0;i<sizeof(m);i++){
		buf[i+1] = m[i];
	}
	
	usb_hid_tx(buf, sizeof(buf));
	
	while (usb_hid_is_transmitting() != 0) {
    }
	/* flush out to avoid having the pc wait for more data */
	usb_hid_tx(NULL, 0);
}

void HIDMouse::buttons(uint8_t b)
{
	if (b != _buttons)
	{
		_buttons = b;
		while (usb_hid_is_transmitting() != 0) {
		}
		move(0,0,0);
	}
}

void HIDMouse::press(uint8_t b)
{
	while (usb_hid_is_transmitting() != 0) {
    }
	buttons(_buttons | b);
}

void HIDMouse::release(uint8_t b)
{
	while (usb_hid_is_transmitting() != 0) {
    }
	buttons(_buttons & ~b);
}

bool HIDMouse::isPressed(uint8_t b)
{
	if ((b & _buttons) > 0)
		return true;
	return false;
}



//================================================================================
//================================================================================
//	Keyboard

HIDKeyboard::HIDKeyboard(void){
	
}

void HIDKeyboard::sendReport(KeyReport* keys)
{
	//HID_SendReport(2,keys,sizeof(KeyReport));
	uint8_t buf[9];//sizeof(_keyReport)+1;
	buf[0] = 2; //report id;
	buf[1] = _keyReport.modifiers;
	buf[2] = _keyReport.reserved;
	
	uint8_t i;
	for(i=0;i<sizeof(_keyReport.keys);i++){
		buf[i+3] = _keyReport.keys[i];
	}
	usb_hid_tx(buf, sizeof(buf));
	
	while (usb_hid_is_transmitting() != 0) {
    }
	/* flush out to avoid having the pc wait for more data */
	usb_hid_tx(NULL, 0);
}

void HIDKeyboard::begin(void){
	HID.begin();
}

void HIDKeyboard::end(void) {
	HID.end();
}

size_t HIDKeyboard::press(uint8_t k)
{
	uint8_t i;
	if (k >= 136) {			// it's a non-printing key (not a modifier)
		k = k - 136;
	} else if (k >= 128) {	// it's a modifier key
		_keyReport.modifiers |= (1<<(k-128));
		k = 0;
	} else {				// it's a printing key
		k = _asciimap[k];
		if (!k) {
			//setWriteError();
			return 0;
		}
		if (k & SHIFT) {						// it's a capital letter or other character reached with shift
			_keyReport.modifiers |= 0x02;	// the left shift modifier
			k &= 0x7F;
		}
	}

	// Add k to the key report only if it's not already present
	// and if there is an empty slot.
	if (_keyReport.keys[0] != k && _keyReport.keys[1] != k &&
		_keyReport.keys[2] != k && _keyReport.keys[3] != k &&
		_keyReport.keys[4] != k && _keyReport.keys[5] != k) {

		for (i=0; i<6; i++) {
			if (_keyReport.keys[i] == 0x00) {
				_keyReport.keys[i] = k;
				break;
			}
		}
		if (i == 6) {
			//setWriteError();
			return 0;
		}
	}
	
	while (usb_hid_is_transmitting() != 0) {
    }
	
	sendReport(&_keyReport);
	return 1;
}

// release() takes the specified key out of the persistent key report and
// sends the report.  This tells the OS the key is no longer pressed and that
// it shouldn't be repeated any more.
size_t HIDKeyboard::release(uint8_t k)
{
	uint8_t i;
	if (k >= 136) {			// it's a non-printing key (not a modifier)
		k = k - 136;
	} else if (k >= 128) {	// it's a modifier key
		_keyReport.modifiers &= ~(1<<(k-128));
		k = 0;
	} else {				// it's a printing key
		k = _asciimap[k];
		if (!k) {
			return 0;
		}
		if (k & 0x80) {							// it's a capital letter or other character reached with shift
			_keyReport.modifiers &= ~(0x02);	// the left shift modifier
			k &= 0x7F;
		}
	}

	// Test the key report to see if k is present.  Clear it if it exists.
	// Check all positions in case the key is present more than once (which it shouldn't be)
	for (i=0; i<6; i++) {
		if (0 != k && _keyReport.keys[i] == k) {
			_keyReport.keys[i] = 0x00;
		}
	}
	
	while (usb_hid_is_transmitting() != 0) {
    }

	sendReport(&_keyReport);
	return 1;
}

void HIDKeyboard::releaseAll(void)
{
	_keyReport.keys[0] = 0;
	_keyReport.keys[1] = 0;
	_keyReport.keys[2] = 0;
	_keyReport.keys[3] = 0;
	_keyReport.keys[4] = 0;
	_keyReport.keys[5] = 0;
	_keyReport.modifiers = 0;
	
	while (usb_hid_is_transmitting() != 0) {
    }
	
	sendReport(&_keyReport);
}

size_t HIDKeyboard::write(uint8_t c)
{
	uint8_t p = 0;

	p = press(c);	// Keydown
	release(c);		// Keyup
	
	return (p);		// Just return the result of press() since release() almost always returns 1
}

//================================================================================
//================================================================================
//	Joystick

void HIDJoystick::sendReport(void){
	usb_hid_tx(joystick_Report, sizeof(joystick_Report));
	
	while (usb_hid_is_transmitting() != 0) {
    }
	/* flush out to avoid having the pc wait for more data */
	usb_hid_tx(NULL, 0);
}

HIDJoystick::HIDJoystick(void){
	
}

void HIDJoystick::begin(void){
	HID.begin();
}

void HIDJoystick::end(void){
	HID.end();
}

void HIDJoystick::button(uint8_t button, bool val){
	button--;
	uint8_t mask = (1 << (button & 7));
	if (val) {
		if (button < 8) joystick_Report[1] |= mask;
		else if (button < 16) joystick_Report[2] |= mask;
		else if (button < 24) joystick_Report[3] |= mask;
		else if (button < 32) joystick_Report[4] |= mask;
	} else {
		mask = ~mask;
		if (button < 8) joystick_Report[1] &= mask;
		else if (button < 16) joystick_Report[2] &= mask;
		else if (button < 24) joystick_Report[3] &= mask;
		else if (button < 32) joystick_Report[4] &= mask;
	}
	
	
	while (usb_hid_is_transmitting() != 0) {
    }
	sendReport();
}

void HIDJoystick::X(uint16_t val){
	if (val > 1023) val = 1023;
	joystick_Report[5] = (joystick_Report[5] & 0x0F) | (val << 4);
	joystick_Report[6] = (joystick_Report[6] & 0xC0) | (val >> 4);
	
	
	while (usb_hid_is_transmitting() != 0) {
    }
	sendReport();
}

void HIDJoystick::Y(uint16_t val){
	if (val > 1023) val = 1023;
	joystick_Report[6] = (joystick_Report[6] & 0x3F) | (val << 6);
	joystick_Report[7] = (val >> 2);
	
	
	while (usb_hid_is_transmitting() != 0) {
    }
	sendReport();
}

void HIDJoystick::position(uint16_t x, uint16_t y){
	if (x > 1023) x = 1023;
	if (y > 1023) y = 1023;
	joystick_Report[5] = (joystick_Report[5] & 0x0F) | (x << 4);
	joystick_Report[6] = (x >> 4) | (y << 6);
	joystick_Report[7] = (y >> 2);
	
	
	while (usb_hid_is_transmitting() != 0) {
    }
	sendReport();
}

void HIDJoystick::Z(uint16_t val){
	if (val > 1023) val = 1023;
	joystick_Report[8] = val;
	joystick_Report[9] = (joystick_Report[9] & 0xFC) | (val >> 8);
	
	
	while (usb_hid_is_transmitting() != 0) {
    }
	sendReport();
}

void HIDJoystick::Zrotate(uint16_t val){
	if (val > 1023) val = 1023;
	joystick_Report[9] = (joystick_Report[9] & 0x03) | (val << 2);
	joystick_Report[10] = (joystick_Report[10] & 0xF0) | (val >> 6);
	
	
	while (usb_hid_is_transmitting() != 0) {
    }
	sendReport();
}

void HIDJoystick::sliderLeft(uint16_t val){
	if (val > 1023) val = 1023;
	joystick_Report[10] = (joystick_Report[10] & 0x0F) | (val << 4);
	joystick_Report[11] = (joystick_Report[11] & 0xC0) | (val >> 4);
	
	
	while (usb_hid_is_transmitting() != 0) {
    }
	sendReport();
}

void HIDJoystick::sliderRight(uint16_t val){
	if (val > 1023) val = 1023;
	joystick_Report[11] = (joystick_Report[11] & 0x3F) | (val << 6);
	joystick_Report[12] = (val >> 2);
	
	
	while (usb_hid_is_transmitting() != 0) {
    }
	sendReport();
}

void HIDJoystick::slider(uint16_t val){
	if (val > 1023) val = 1023;
	joystick_Report[10] = (joystick_Report[10] & 0x0F) | (val << 4);
	joystick_Report[11] = (val >> 4) | (val << 6);
	joystick_Report[12] = (val >> 2);
	
	
	while (usb_hid_is_transmitting() != 0) {
    }
	sendReport();
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
	joystick_Report[5] = (joystick_Report[5] & 0xF0) | val;
	
	
	while (usb_hid_is_transmitting() != 0) {
    }
	sendReport();
}


HIDDevice HID;
#if defined(USB_HID_KMJ) || defined(USB_HID_KM)
HIDMouse Mouse;
HIDKeyboard Keyboard;
#endif
#if defined(USB_HID_KMJ) || defined(USB_HID_J)
HIDJoystick Joystick;
#endif


#endif
