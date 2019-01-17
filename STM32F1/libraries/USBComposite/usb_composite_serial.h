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

#ifndef _USB_SERIAL_H_
#define _USB_SERIAL_H_

#include <libmaple/libmaple_types.h>
#include <libmaple/usb.h>
#include "usb_generic.h"

#ifdef __cplusplus
extern "C" {
#endif

extern USBCompositePart usbSerialPart;

/*
 * CDC ACM Requests
 */

#define USBHID_CDCACM_SET_LINE_CODING        0x20
#define USBHID_CDCACM_GET_LINE_CODING        0x21
#define USBHID_CDCACM_SET_COMM_FEATURE       0x02
#define USBHID_CDCACM_SET_CONTROL_LINE_STATE 0x22
#define USBHID_CDCACM_CONTROL_LINE_DTR       (0x01)
#define USBHID_CDCACM_CONTROL_LINE_RTS       (0x02)

#define USBHID_CDCACM_MANAGEMENT_EPSIZE      0x10
//#define USBHID_CDCACM_RX_EPSIZE              0x40
//#define USBHID_CDCACM_TX_EPSIZE              0x40
/*
 * Descriptors, etc.
 */



#define USBHID_CDCACM_CTRL_ENDP            0

typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bFirstInterface;
	uint8_t bInterfaceCount;
	uint8_t bFunctionClass;
	uint8_t bFunctionSubClass;
	uint8_t bFunctionProtocol;
	uint8_t iFunction;
} IADescriptor;

#define CDC_FUNCTIONAL_DESCRIPTOR_SIZE(DataSize) (3 + DataSize)
#define CDC_FUNCTIONAL_DESCRIPTOR(DataSize)     \
  struct {                                      \
      uint8 bLength;                            \
      uint8 bDescriptorType;                    \
      uint8 SubType;                            \
      uint8 Data[DataSize];                     \
  } __packed
 
#define USB_DEVICE_CLASS_CDC              0x02
#define USB_DEVICE_SUBCLASS_CDC           0x00
#define USB_INTERFACE_CLASS_CDC           0x02
#define USB_INTERFACE_SUBCLASS_CDC_ACM    0x02
#define USB_INTERFACE_CLASS_DIC           0x0A

/*
 * CDC ACM interface
 */

void   composite_cdcacm_putc(char ch);
uint32 composite_cdcacm_tx(const uint8* buf, uint32 len);
uint32 composite_cdcacm_rx(uint8* buf, uint32 len);
uint32 composite_cdcacm_peek(uint8* buf, uint32 len);
uint32 composite_cdcacm_peek_ex(uint8* buf, uint32 offset, uint32 len);
void composite_cdcacm_setTXEPSize(uint32_t size);
void composite_cdcacm_setRXEPSize(uint32_t size);

uint32 composite_cdcacm_data_available(void); /* in RX buffer */
uint16 composite_cdcacm_get_pending(void);
uint8 usb_is_transmitting(void);

uint8 composite_cdcacm_get_dtr(void);
uint8 composite_cdcacm_get_rts(void);

typedef struct composite_cdcacm_line_coding {
    uint32 dwDTERate;           /* Baud rate */

#define USBHID_CDCACM_STOP_BITS_1   0
#define USBHID_CDCACM_STOP_BITS_1_5 1
#define USBHID_CDCACM_STOP_BITS_2   2
    uint8 bCharFormat;          /* Stop bits */

#define USBHID_CDCACM_PARITY_NONE  0
#define USBHID_CDCACM_PARITY_ODD   1
#define USBHID_CDCACM_PARITY_EVEN  2
#define USBHID_CDCACM_PARITY_MARK  3
#define USBHID_CDCACM_PARITY_SPACE 4
    uint8 bParityType;          /* Parity type */

    uint8 bDataBits;            /* Data bits: 5, 6, 7, 8, or 16 */
} __packed composite_cdcacm_line_coding;

/* Retrieve a copy of the current line coding structure. */
void composite_cdcacm_get_line_coding(composite_cdcacm_line_coding*);

/* Line coding conveniences. */
int composite_cdcacm_get_baud(void);        /* dwDTERate */
int composite_cdcacm_get_stop_bits(void);   /* bCharFormat */
int composite_cdcacm_get_parity(void);      /* bParityType */
int composite_cdcacm_get_n_data_bits(void); /* bDataBits */

/*
 * Hack: hooks for bootloader reset signalling
 */

#define USBHID_CDCACM_HOOK_RX 0x1
#define USBHID_CDCACM_HOOK_IFACE_SETUP 0x2

void composite_cdcacm_set_hooks(unsigned hook_flags, void (*hook)(unsigned, void*));

#define composite_cdcacm_remove_hooks(hook_flags) composite_cdcacm_remove_hooks(hook_flags, 0)

#ifdef __cplusplus
}
#endif

#endif
