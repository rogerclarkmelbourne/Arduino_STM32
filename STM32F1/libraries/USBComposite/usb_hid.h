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

/**
 * @file libmaple/include/libmaple/usb_device.h
 * @brief USB Composite with CDC ACM and HID support
 *
 * IMPORTANT: this API is unstable, and may change without notice.
 */

#ifndef _USB_HID_H_
#define _USB_HID_H_

#include <libmaple/libmaple_types.h>
#include <libmaple/usb.h>
#include "usb_generic.h"

#define MAX_HID_BUFFERS 8
#define HID_BUFFER_SIZE(n,reportID) ((n)+((reportID)!=0))
#define HID_BUFFER_ALLOCATE_SIZE(n,reportID) ((HID_BUFFER_SIZE((n),(reportID))+1)/2*2)

#define HID_BUFFER_MODE_NO_WAIT 1
#define HID_BUFFER_MODE_OUTPUT  2

#define HID_BUFFER_EMPTY    0 
#define HID_BUFFER_UNREAD   1
#define HID_BUFFER_READ     2

extern USBCompositePart usbHIDPart;

typedef struct HIDBuffer_t {
    volatile uint8_t* buffer; // use HID_BUFFER_ALLOCATE_SIZE() to calculate amount of memory to allocate                            
    uint16_t bufferSize; // this should match HID_BUFFER_SIZE
    uint8_t  reportID;
    uint8_t  mode;
    uint16_t currentDataSize;
    uint8_t  state; // HID_BUFFER_EMPTY, etc.
#ifdef __cplusplus
    inline HIDBuffer_t(volatile uint8_t* _buffer=NULL, uint16_t _bufferSize=0, uint8_t _reportID=0, uint8_t _mode=0) {
        reportID = _reportID;
        buffer = _buffer;
        bufferSize = _bufferSize;
        mode = _mode;
    }
#endif
} HIDBuffer_t;

#ifdef __cplusplus
extern "C" {
#endif

#define USB_HID_TX_EPSIZE            	0x40

void usb_hid_set_report_descriptor(const uint8* report_descriptor, uint16 report_descriptor_length);
void usb_hid_clear_buffers(uint8_t type);
uint8_t usb_hid_add_buffer(uint8_t type, volatile HIDBuffer_t* buf);
void usb_hid_set_buffers(uint8_t type, volatile HIDBuffer_t* featureBuffers, int count);    
uint16_t usb_hid_get_data(uint8_t type, uint8_t reportID, uint8_t* out, uint8_t poll);
void usb_hid_set_feature(uint8_t reportID, uint8_t* data);

 

/*
 * HID Requests
 */

typedef enum _HID_REQUESTS
{
 
  GET_REPORT = 1,
  GET_IDLE,
  GET_PROTOCOL,
 
  SET_REPORT = 9,
  SET_IDLE,
  SET_PROTOCOL
 
} HID_REQUESTS;
 
#define HID_REPORT_TYPE_INPUT         0x01
#define HID_REPORT_TYPE_OUTPUT        0x02
#define HID_REPORT_TYPE_FEATURE       0x03 
 

/*
 * HID Descriptors, etc.
 */
 
#define HID_ENDPOINT_INT 				1
 
#define HID_DESCRIPTOR_TYPE             0x21 
#define REPORT_DESCRIPTOR               0x22


typedef struct
{
	uint8_t len;			// 9
	uint8_t dtype;			// 0x21
	uint8_t	versionL;		// 0x101
	uint8_t	versionH;		// 0x101
	uint8_t	country;
	uint8_t	numDesc;
	uint8_t	desctype;		// 0x22 report
	uint8_t	descLenL;
	uint8_t	descLenH;
} HIDDescriptor;


#define USB_ENDPOINT_TYPE_INTERRUPT     0x03

#define USB_INTERFACE_CLASS_HID           0x03
#define USB_INTERFACE_SUBCLASS_HID		  0x01

 /*
 * HID interface
 */

void   usb_hid_putc(char ch);
uint32 usb_hid_tx(const uint8* buf, uint32 len);
uint32 usb_hid_tx_mod(const uint8* buf, uint32 len);

uint32 usb_hid_data_available(void); /* in RX buffer */
uint16 usb_hid_get_pending(void);


#ifdef __cplusplus
}
#endif

#endif
