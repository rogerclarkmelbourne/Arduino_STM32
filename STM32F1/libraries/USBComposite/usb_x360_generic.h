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

#ifndef _USB_X360_GENERIC_H
#define _USB_X360_GENERIC_H

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

#define USB_X360_MAX_CONTROLLERS         4
#define USB_X360_TX_EPSIZE            0x20
#define USB_X360_RX_EPSIZE            0x20
#define USB_X360_BUFFER_SIZE_PER_CONTROLLER USB_X360_RX_EPSIZE 

extern USBEndpointInfo x360Endpoints[];
extern uint32 x360_num_controllers;
extern USBCompositePart usbX360Part;
/*
 * HID interface
 */

uint32 x360_tx(uint32 controller, const uint8* buf, uint32 len);
uint8 x360_is_transmitting(uint32 controller);
void x360_set_rumble_callback(uint32 controller, void (*callback)(uint8 left, uint8 right));
void x360_set_led_callback(uint32 controller, void (*callback)(uint8 pattern));
void x360_generic_initialize_controller_data(uint32 _numControllers, uint8* buffers);

#define X360_INTERFACE_OFFSET 0
#define X360_INTERFACE_NUMBER (X360_INTERFACE_OFFSET+usbX360Part.startInterface)
#define X360_NUM_INTERFACES 1
#define X360_ENDPOINT_TX 0
#define X360_ENDPOINT_RX 1
#define X360_NUM_ENDPOINTS 2

#define USB_X360_RX_ENDPOINT_INFO(i) &x360Endpoints[(i)*X360_NUM_ENDPOINTS+X360_ENDPOINT_RX]
#define USB_X360_TX_ENDPOINT_INFO(i) &x360Endpoints[(i)*X360_NUM_ENDPOINTS+X360_ENDPOINT_TX]
#define USB_X360_RX_PMA_PTR(i) x360Endpoints[(i)*X360_NUM_ENDPOINTS+X360_ENDPOINT_RX].pma
#define USB_X360_TX_PMA_PTR(i) x360Endpoints[(i)*X360_NUM_ENDPOINTS+X360_ENDPOINT_TX].pma
#define USB_X360_RX_ENDP(i) x360Endpoints[(i)*X360_NUM_ENDPOINTS+X360_ENDPOINT_RX].address
#define USB_X360_TX_ENDP(i) x360Endpoints[(i)*X360_NUM_ENDPOINTS+X360_ENDPOINT_TX].address

#ifdef __cplusplus
}
#endif

#endif
