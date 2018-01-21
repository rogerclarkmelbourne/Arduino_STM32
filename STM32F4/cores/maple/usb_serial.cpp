/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 * @brief USB virtual serial terminal
 */

#include <string.h>

#include "wirish.h"
#include "usb.h"

#ifdef SERIAL_USB

#define USB_TIMEOUT 50
bool USBSerial::_hasBegun = false;

USBSerial::USBSerial(void) {
}

void USBSerial::begin(void) {
    if (_hasBegun)
        return;
    _hasBegun = true;
    setupUSB();
}

void USBSerial::begin(int) {
    this->begin();
}

void USBSerial::end(void) {
    disableUSB();
	_hasBegun = false;
}

size_t USBSerial::write(uint8 ch) {
    const uint8 buf[] = {ch};
    return this->write(buf, 1);
}

size_t USBSerial::write(const char *str) {
    return this->write(str, strlen(str));
}

size_t USBSerial::write(const void *buf, uint32 len) {
    if (!(usbIsConnected() && usbIsConfigured()) || !buf) {
        return 0;
    }

    uint32 txed = 0;
    uint32 old_txed = 0;
    uint32 start = millis();

    while (txed < len && (millis() - start < USB_TIMEOUT)) {
        txed += usbSendBytes((const uint8*)buf + txed, len - txed);
        if (old_txed != txed) {
            start = millis();
        }
        old_txed = txed;
    }
    return txed;
}

int USBSerial::available(void) {
    return usbBytesAvailable();
}

int USBSerial::read(void *buf, uint32 len) {
    if (!buf) {
        return 0;
    }

    uint32 rxed = 0;
    while (rxed < len) {
        rxed += usbReceiveBytes((uint8*)buf + rxed, len - rxed);
    }

    return rxed;
}

/* Blocks forever until 1 byte is received */
int USBSerial::read(void) {
    uint8 buf[1];
    this->read(buf, 1);
    return buf[0];
}

int USBSerial::peek(void)
{
    // ThingToDo : Don't do any thing yet, since F4 doesn't have usb_cdcacm_peek() yet.
    /*
    uint8 b;
    if (usb_cdcacm_peek(&b, 1)==1)
    {
	return b;
    }
    else */
    {
	return -1;
    }
}

void USBSerial::flush(void)
{
    /*Roger Clark. Rather slow method. Need to improve this */
    uint8 b;
    while(usbBytesAvailable())
    {
	this->read(&b, 1);
    }
    return;
}

uint8 USBSerial::pending(void) {
    return usbGetPending();
}

USBSerial::operator bool() {
    return usbIsConnected() && usbIsConfigured();
}

uint8 USBSerial::getDTR(void) {
    return usbGetDTR();
}

uint8 USBSerial::getRTS(void) {
    return usbGetRTS();
}

void USBSerial::enableBlockingTx(void) {
	usbEnableBlockingTx();
}

void USBSerial::disableBlockingTx(void) {
	usbDisableBlockingTx();
}

USBSerial SerialUSB;

#endif
