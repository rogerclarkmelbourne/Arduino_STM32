/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010, 2011, 2012 LeafLabs LLC.
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

/*
 * NOTE: This API is _unstable_ and will change drastically over time.
 */

#ifndef _LIBMAPLE_USB_H_
#define _LIBMAPLE_USB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <libmaple/libmaple_types.h>
#include <libmaple/rcc.h>

/*
 * Descriptors and other paraphernalia
 */

/* Descriptor types */

#define USB_DESCRIPTOR_TYPE_DEVICE        0x01
#define USB_DESCRIPTOR_TYPE_CONFIGURATION 0x02
#define USB_DESCRIPTOR_TYPE_STRING        0x03
#define USB_DESCRIPTOR_TYPE_INTERFACE     0x04
#define USB_DESCRIPTOR_TYPE_ENDPOINT      0x05

/* Descriptor structs and declaration helpers */

#define USB_DESCRIPTOR_STRING_LEN(x) (2 + (x << 1))

#define USB_DESCRIPTOR_STRING(len)              \
  struct {                                      \
      uint8 bLength;                            \
      uint8 bDescriptorType;                    \
      uint16 bString[len];                      \
  } __packed

typedef struct usb_descriptor_device {
    uint8  bLength;
    uint8  bDescriptorType;
    uint16 bcdUSB;
    uint8  bDeviceClass;
    uint8  bDeviceSubClass;
    uint8  bDeviceProtocol;
    uint8  bMaxPacketSize0;
    uint16 idVendor;
    uint16 idProduct;
    uint16 bcdDevice;
    uint8  iManufacturer;
    uint8  iProduct;
    uint8  iSerialNumber;
    uint8  bNumConfigurations;
} __packed usb_descriptor_device;

typedef struct usb_descriptor_config_header {
    uint8  bLength;
    uint8  bDescriptorType;
    uint16 wTotalLength;
    uint8  bNumInterfaces;
    uint8  bConfigurationValue;
    uint8  iConfiguration;
    uint8  bmAttributes;
    uint8  bMaxPower;
} __packed usb_descriptor_config_header;

typedef struct usb_descriptor_interface {
    uint8 bLength;
    uint8 bDescriptorType;
    uint8 bInterfaceNumber;
    uint8 bAlternateSetting;
    uint8 bNumEndpoints;
    uint8 bInterfaceClass;
    uint8 bInterfaceSubClass;
    uint8 bInterfaceProtocol;
    uint8 iInterface;
} __packed usb_descriptor_interface;

typedef struct usb_descriptor_endpoint {
    uint8  bLength;
    uint8  bDescriptorType;
    uint8  bEndpointAddress;
    uint8  bmAttributes;
    uint16 wMaxPacketSize;
    uint8  bInterval;
} __packed usb_descriptor_endpoint;

typedef struct usb_descriptor_string {
    uint8 bLength;
    uint8 bDescriptorType;
    uint8 bString[];
} usb_descriptor_string;

/* Common values that go inside descriptors */

#define USB_CONFIG_ATTR_BUSPOWERED        0b10000000
#define USB_CONFIG_ATTR_SELF_POWERED      0b11000000

#define USB_EP_TYPE_INTERRUPT             0x03
#define USB_EP_TYPE_BULK                  0x02

#define USB_DESCRIPTOR_ENDPOINT_IN        0x80
#define USB_DESCRIPTOR_ENDPOINT_OUT       0x00

/*
 * USB module core
 */

#ifndef USB_ISR_MSK
/* Handle CTRM, WKUPM, SUSPM, ERRM, SOFM, ESOFM, RESETM */
#define USB_ISR_MSK 0xBF00
#endif

typedef enum usb_dev_state {
    USB_UNCONNECTED,
    USB_ATTACHED,
    USB_POWERED,
    USB_SUSPENDED,
    USB_ADDRESSED,
    USB_CONFIGURED
} usb_dev_state;

/* Encapsulates global state formerly handled by usb_lib/ */
typedef struct usblib_dev {
    uint32 irq_mask;
    void (**ep_int_in)(void);
    void (**ep_int_out)(void);
    usb_dev_state state;
    usb_dev_state prevState;
    rcc_clk_id clk_id;
} usblib_dev;

extern usblib_dev *USBLIB;

void usb_init_usblib(usblib_dev *dev,
                     void (**ep_int_in)(void),
                     void (**ep_int_out)(void));

static inline uint8 usb_is_connected(usblib_dev *dev) {
    return dev->state != USB_UNCONNECTED;
}

static inline uint8 usb_is_configured(usblib_dev *dev) {
    return dev->state == USB_CONFIGURED;
}

#ifdef __cplusplus
}
#endif

#endif
