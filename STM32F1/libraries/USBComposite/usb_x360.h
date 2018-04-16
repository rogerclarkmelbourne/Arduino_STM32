/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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

#ifndef _USB_X360_H
#define _USB_X360_H

#include <libmaple/libmaple_types.h>
#include <libmaple/gpio.h>
#include <libmaple/usb.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Descriptors, etc.
 */
 
//extern const uint8_t hid_report_descriptor[];

/*
 * Endpoint configuration
 */

#define USB_X360_TX_EPSIZE            0x20
#define USB_X360_RX_EPSIZE            0x20

/*
 * HID interface
 */

extern USBCompositePart usbX360Part;
void x360_enable();
void x360_disable();

void   x360_putc(char ch);
uint32 x360_tx(const uint8* buf, uint32 len);
uint32 x360_rx(uint8* buf, uint32 len);
uint32 x360_hid_peek(uint8* buf, uint32 len);
uint32 x360_data_available(void); /* in RX buffer */
uint16 x360_get_pending(void);
uint8 x360_is_transmitting(void);
void x360_set_rx_callback(void (*callback)(const uint8* buffer, uint32 size));
void x360_set_rumble_callback(void (*callback)(uint8 left, uint8 right));
void x360_set_led_callback(void (*callback)(uint8 pattern));

#ifdef __cplusplus
}
#endif

#endif
