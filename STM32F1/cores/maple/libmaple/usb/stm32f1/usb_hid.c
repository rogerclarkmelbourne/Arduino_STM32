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
 * @file libmaple/usb/stm32f1/usb_hid.c
 * @brief USB HID (human interface device) support
 *
 * FIXME: this works on the STM32F1 USB peripherals, and probably no
 * place else. Nonportable bits really need to be factored out, and
 * the result made cleaner.
 */

#if defined(USB_HID_KMJ) || defined(USB_HID_KM) || defined(USB_HID_J)
 
 
#include <libmaple/usb_hid.h>

#include <libmaple/usb.h>
#include <libmaple/nvic.h>
#include <libmaple/delay.h>

/* Private headers */
#include "usb_lib_globals.h"
#include "usb_reg_map.h"

/* usb_lib headers */
#include "usb_type.h"
#include "usb_core.h"
#include "usb_def.h"

/******************************************************************************
 ******************************************************************************
 ***
 ***   HACK ALERT! FIXME FIXME FIXME FIXME!
 ***
 ***   A bunch of LeafLabs-specific configuration lives in here for
 ***   now.  This mess REALLY needs to get teased apart, with
 ***   appropriate pieces moved into Wirish.
 ***
 ******************************************************************************
 *****************************************************************************/

#if !(defined(BOARD_maple) || defined(BOARD_maple_RET6) ||      \
      defined(BOARD_maple_mini) || defined(BOARD_maple_native))
#warning USB HID relies on LeafLabs board-specific configuration.\
    You may have problems on non-LeafLabs boards.
#endif


uint32 ProtocolValue;

static void hidDataTxCb(void);
static void hidDataRxCb(void);

static void usbInit(void);
static void usbReset(void);
static RESULT usbDataSetup(uint8 request);
static RESULT usbNoDataSetup(uint8 request);
static RESULT usbGetInterfaceSetting(uint8 interface, uint8 alt_setting);
static uint8* usbGetDeviceDescriptor(uint16 length);
static uint8* usbGetConfigDescriptor(uint16 length);
static uint8* usbGetStringDescriptor(uint16 length);
static void usbSetConfiguration(void);
static void usbSetDeviceAddress(void);

/*
 * Descriptors
 */
 

const uint8_t hid_report_descriptor[] = {
#if defined(USB_HID_KMJ) || defined(USB_HID_KM)
	//	Mouse
    0x05, 0x01,						// USAGE_PAGE (Generic Desktop)	// 54
    0x09, 0x02,						// USAGE (Mouse)
    0xa1, 0x01,						// COLLECTION (Application)
    0x85, 0x01,						//   REPORT_ID (1)
    0x09, 0x01,						//   USAGE (Pointer)
    0xa1, 0x00,						//   COLLECTION (Physical)
    0x05, 0x09,						//     USAGE_PAGE (Button)
    0x19, 0x01,						//     USAGE_MINIMUM (Button 1)
    0x29, 0x03,						//     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,						//     LOGICAL_MINIMUM (0)
    0x25, 0x01,						//     LOGICAL_MAXIMUM (1)
    0x95, 0x03,						//     REPORT_COUNT (3)
    0x75, 0x01,						//     REPORT_SIZE (1)
    0x81, 0x02,						//     INPUT (Data,Var,Abs)
    0x95, 0x01,						//     REPORT_COUNT (1)
    0x75, 0x05,						//     REPORT_SIZE (5)
    0x81, 0x03,						//     INPUT (Cnst,Var,Abs)
    0x05, 0x01,						//     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,						//     USAGE (X)
    0x09, 0x31,						//     USAGE (Y)
    0x09, 0x38,						//     USAGE (Wheel)
    0x15, 0x81,						//     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,						//     LOGICAL_MAXIMUM (127)
    0x75, 0x08,						//     REPORT_SIZE (8)
    0x95, 0x03,						//     REPORT_COUNT (3)
    0x81, 0x06,						//     INPUT (Data,Var,Rel)
    0xc0,      						//   END_COLLECTION
    0xc0,      						// END_COLLECTION

	//	Keyboard
    0x05, 0x01,						// USAGE_PAGE (Generic Desktop)	// 47
    0x09, 0x06,						// USAGE (Keyboard)
    0xa1, 0x01,						// COLLECTION (Application)
    0x85, 0x02,						//   REPORT_ID (2)
    0x05, 0x07,						//   USAGE_PAGE (Keyboard)

	0x19, 0xe0,						//   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,						//   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,						//   LOGICAL_MINIMUM (0)
    0x25, 0x01,						//   LOGICAL_MAXIMUM (1)
    0x75, 0x01,						//   REPORT_SIZE (1)

	0x95, 0x08,						//   REPORT_COUNT (8)
    0x81, 0x02,						//   INPUT (Data,Var,Abs)
    0x95, 0x01,						//   REPORT_COUNT (1)
    0x75, 0x08,						//   REPORT_SIZE (8)
    0x81, 0x03,						//   INPUT (Cnst,Var,Abs)

	0x95, 0x06,						//   REPORT_COUNT (6)
    0x75, 0x08,						//   REPORT_SIZE (8)
    0x15, 0x00,						//   LOGICAL_MINIMUM (0)
    0x25, 0x65,						//   LOGICAL_MAXIMUM (101)
    0x05, 0x07,						//   USAGE_PAGE (Keyboard)

	0x19, 0x00,						//   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,						//   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,						//   INPUT (Data,Ary,Abs)
    0xc0,      						// END_COLLECTION
#endif
	
#if defined(USB_HID_KMJ) || defined(USB_HID_J)	
	//  Joystick
	0x05, 0x01,						// Usage Page (Generic Desktop)
	0x09, 0x04,						// Usage (Joystick)
	0xA1, 0x01,						// Collection (Application)
    0x85, 0x03,						//   REPORT_ID (3)
	0x15, 0x00,						//	 Logical Minimum (0)
	0x25, 0x01,						//   Logical Maximum (1)
	0x75, 0x01,						//   Report Size (1)
	0x95, 0x20,						//   Report Count (32)
	0x05, 0x09,						//   Usage Page (Button)
	0x19, 0x01,						//   Usage Minimum (Button #1)
	0x29, 0x20,						//   Usage Maximum (Button #32)
	0x81, 0x02,						//   Input (variable,absolute)
	0x15, 0x00,						//   Logical Minimum (0)
	0x25, 0x07,						//   Logical Maximum (7)
	0x35, 0x00,						//   Physical Minimum (0)
	0x46, 0x3B, 0x01,				//   Physical Maximum (315)
	0x75, 0x04,						//   Report Size (4)
	0x95, 0x01,						//   Report Count (1)
	0x65, 0x14,						//   Unit (20)
    0x05, 0x01,                     //   Usage Page (Generic Desktop)
	0x09, 0x39,						//   Usage (Hat switch)
	0x81, 0x42,						//   Input (variable,absolute,null_state)
    0x05, 0x01,                     //Usage Page (Generic Desktop)
	0x09, 0x01,						//Usage (Pointer)
    0xA1, 0x00,                     //Collection ()
	0x15, 0x00,						//   Logical Minimum (0)
	0x26, 0xFF, 0x03,				//   Logical Maximum (1023)
	0x75, 0x0A,						//   Report Size (10)
	0x95, 0x04,						//   Report Count (4)
	0x09, 0x30,						//   Usage (X)
	0x09, 0x31,						//   Usage (Y)
	0x09, 0x32,						//   Usage (Z)
	0x09, 0x35,						//   Usage (Rz)
	0x81, 0x02,						//   Input (variable,absolute)
    0xC0,                           // End Collection
	0x15, 0x00,						// Logical Minimum (0)
	0x26, 0xFF, 0x03,				// Logical Maximum (1023)
	0x75, 0x0A,						// Report Size (10)
	0x95, 0x02,						// Report Count (2)
	0x09, 0x36,						// Usage (Slider)
	0x09, 0x36,						// Usage (Slider)
	0x81, 0x02,						// Input (variable,absolute)
    0xC0,                           // End Collection
#endif
	
	
#ifdef USB_RAWHID
	//	RAW HID
	0x06, LSB(RAWHID_USAGE_PAGE), MSB(RAWHID_USAGE_PAGE),	// 30
	0x0A, LSB(RAWHID_USAGE), MSB(RAWHID_USAGE),

	0xA1, 0x01,				// Collection 0x01
    0x85, 0x03,             // REPORT_ID (3)
	0x75, 0x08,				// report size = 8 bits
	0x15, 0x00,				// logical minimum = 0
	0x26, 0xFF, 0x00,		// logical maximum = 255

	0x95, 64,				// report count TX
	0x09, 0x01,				// usage
	0x81, 0x02,				// Input (array)

	0x95, 64,				// report count RX
	0x09, 0x02,				// usage
	0x91, 0x02,				// Output (array)
	0xC0					// end collection
#endif
};

/* FIXME move to Wirish */
#define LEAFLABS_ID_VENDOR                0x1EAF
#define MAPLE_ID_PRODUCT                  0x0024
static const usb_descriptor_device usbHIDDescriptor_Device =
    USB_HID_DECLARE_DEV_DESC(LEAFLABS_ID_VENDOR, MAPLE_ID_PRODUCT);
	

typedef struct {
    usb_descriptor_config_header Config_Header;
    usb_descriptor_interface     HID_Interface;
	HIDDescriptor			 	 HID_Descriptor;
    usb_descriptor_endpoint      DataInEndpoint;
    usb_descriptor_endpoint      DataOutEndpoint;
} __packed usb_descriptor_config;


#define MAX_POWER (100 >> 1)
static const usb_descriptor_config usbHIDDescriptor_Config =
{
	.Config_Header = {
		.bLength 			  = sizeof(usb_descriptor_config_header),
        .bDescriptorType      = USB_DESCRIPTOR_TYPE_CONFIGURATION,
        .wTotalLength         = sizeof(usb_descriptor_config),//0,
        .bNumInterfaces       = 0x01,
        .bConfigurationValue  = 0x01,
        .iConfiguration       = 0x00,
        .bmAttributes         = (USB_CONFIG_ATTR_BUSPOWERED |
                                 USB_CONFIG_ATTR_SELF_POWERED),
        .bMaxPower            = MAX_POWER,
	},
	
	.HID_Interface = {
		.bLength            = sizeof(usb_descriptor_interface),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber   = 0x00,
        .bAlternateSetting  = 0x00,
        .bNumEndpoints      = 0x02,
        .bInterfaceClass    = USB_INTERFACE_CLASS_HID,
        .bInterfaceSubClass = USB_INTERFACE_SUBCLASS_HID,
        .bInterfaceProtocol = 0x00, /* Common AT Commands */
        .iInterface         = 0x00,
	},
	
	.HID_Descriptor = {
		.len				= 9,//sizeof(HIDDescDescriptor),
		.dtype				= HID_DESCRIPTOR_TYPE,
		.versionL			= 0x10,
		.versionH			= 0x01,
		.country			= 0x00,
		.numDesc			= 0x01,
		.desctype			= REPORT_DESCRIPTOR,//0x22,
		.descLenL			= sizeof(hid_report_descriptor),
		.descLenH			= 0x00,
	},
	
	.DataInEndpoint = {
		.bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_IN | USB_HID_TX_ENDP),//0x81,//USB_HID_TX_ADDR,
        .bmAttributes     = USB_ENDPOINT_TYPE_INTERRUPT,
        .wMaxPacketSize   = USB_HID_TX_EPSIZE,//0x40,//big enough for a keyboard 9 byte packet and for a mouse 5 byte packet
        .bInterval        = 0x0A,
	},

    .DataOutEndpoint = {
        .bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_OUT | USB_HID_RX_ENDP),
        .bmAttributes     = USB_EP_TYPE_BULK,
        .wMaxPacketSize   = USB_HID_RX_EPSIZE,
        .bInterval        = 0x00,
    },
};

/*
  String Descriptors:

  we may choose to specify any or none of the following string
  identifiers:

  iManufacturer:    LeafLabs
  iProduct:         Maple
  iSerialNumber:    NONE
  iConfiguration:   NONE
  iInterface(CCI):  NONE
  iInterface(DCI):  NONE

*/

/* Unicode language identifier: 0x0409 is US English */
/* FIXME move to Wirish */
static const usb_descriptor_string usbHIDDescriptor_LangID = {
    .bLength         = USB_DESCRIPTOR_STRING_LEN(1),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString         = {0x09, 0x04},
};

/* FIXME move to Wirish */
static const usb_descriptor_string usbHIDDescriptor_iManufacturer = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(8),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString = {'L', 0, 'e', 0, 'a', 0, 'f', 0,
                'L', 0, 'a', 0, 'b', 0, 's', 0},
};

/* FIXME move to Wirish */
static const usb_descriptor_string usbHIDDescriptor_iProduct = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(5),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString = {'M', 0, 'a', 0, 'p', 0, 'l', 0, 'e', 0},
};

/* FIXME move to Wirish */
static const usb_descriptor_string usbHIDDescriptor_iInterface = {
    .bLength = USB_DESCRIPTOR_STRING_LEN(3),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString = {'H', 0, 'I', 0, 'D', 0},
};

static ONE_DESCRIPTOR usbHIDDevice_Descriptor = {
    (uint8*)&usbHIDDescriptor_Device,
    sizeof(usb_descriptor_device)
};

static ONE_DESCRIPTOR usbHIDConfig_Descriptor = {
    (uint8*)&usbHIDDescriptor_Config,
    sizeof(usbHIDDescriptor_Config)
};
 
static ONE_DESCRIPTOR HID_Report_Descriptor = {
    (uint8*)&hid_report_descriptor,
    sizeof(hid_report_descriptor)
};

#define N_STRING_DESCRIPTORS 3
static ONE_DESCRIPTOR usbHIDString_Descriptor[N_STRING_DESCRIPTORS] = {
    {(uint8*)&usbHIDDescriptor_LangID,       USB_DESCRIPTOR_STRING_LEN(1)},
    {(uint8*)&usbHIDDescriptor_iManufacturer,USB_DESCRIPTOR_STRING_LEN(8)},
    {(uint8*)&usbHIDDescriptor_iProduct,     USB_DESCRIPTOR_STRING_LEN(5)},
    {(uint8*)&usbHIDDescriptor_iInterface,     USB_DESCRIPTOR_STRING_LEN(3)}
};

/*
 * Etc.
 */

/* I/O state */

#define HID_BUFFER_SIZE	512

/* Received data */
static volatile uint8 hidBufferRx[HID_BUFFER_SIZE];
/* Read index into hidBufferRx */
static volatile uint32 rx_offset = 0;
/* Number of bytes left to transmit */
static volatile uint32 n_unsent_bytes = 0;
/* Are we currently sending an IN packet? */
static volatile uint8 transmitting = 0;
/* Number of unread bytes */
static volatile uint32 n_unread_bytes = 0;

/*
 * Endpoint callbacks
 */

static void (*ep_int_in[7])(void) =
    {hidDataTxCb,
     NOP_Process,
     NOP_Process,
     NOP_Process,
     NOP_Process,
     NOP_Process,
     NOP_Process};

static void (*ep_int_out[7])(void) =
    {NOP_Process,
     hidDataRxCb,//NOP_Process,
     NOP_Process,
     NOP_Process,
     NOP_Process,
     NOP_Process,
     NOP_Process};

/*
 * Globals required by usb_lib/
 *
 * Mark these weak so they can be overriden to implement other USB
 * functionality.
 */

#define NUM_ENDPTS                0x02
__weak DEVICE Device_Table = {
    .Total_Endpoint      = NUM_ENDPTS,
    .Total_Configuration = 1
};

#define MAX_PACKET_SIZE            0x40  /* 64B, maximum for USB FS Devices */
__weak DEVICE_PROP Device_Property = {
    .Init                        = usbInit,
    .Reset                       = usbReset,
    .Process_Status_IN           = NOP_Process,
    .Process_Status_OUT          = NOP_Process,
    .Class_Data_Setup            = usbDataSetup,
    .Class_NoData_Setup          = usbNoDataSetup,
    .Class_Get_Interface_Setting = usbGetInterfaceSetting,
    .GetDeviceDescriptor         = usbGetDeviceDescriptor,
    .GetConfigDescriptor         = usbGetConfigDescriptor,
    .GetStringDescriptor         = usbGetStringDescriptor,
    .RxEP_buffer                 = NULL,
    .MaxPacketSize               = MAX_PACKET_SIZE
};

__weak USER_STANDARD_REQUESTS User_Standard_Requests = {
    .User_GetConfiguration   = NOP_Process,
    .User_SetConfiguration   = usbSetConfiguration,
    .User_GetInterface       = NOP_Process,
    .User_SetInterface       = NOP_Process,
    .User_GetStatus          = NOP_Process,
    .User_ClearFeature       = NOP_Process,
    .User_SetEndPointFeature = NOP_Process,
    .User_SetDeviceFeature   = NOP_Process,
    .User_SetDeviceAddress   = usbSetDeviceAddress
};

/*
 * HID interface
 */

void usb_hid_enable(gpio_dev *disc_dev, uint8 disc_bit) {
    /* Present ourselves to the host. Writing 0 to "disc" pin must
     * pull USB_DP pin up while leaving USB_DM pulled down by the
     * transceiver. See USB 2.0 spec, section 7.1.7.3. */
    gpio_set_mode(disc_dev, disc_bit, GPIO_OUTPUT_PP);
    gpio_write_bit(disc_dev, disc_bit, 0);

    /* Initialize the USB peripheral. */
    usb_init_usblib(USBLIB, ep_int_in, ep_int_out);
}

void usb_hid_disable(gpio_dev *disc_dev, uint8 disc_bit) {
    /* Turn off the interrupt and signal disconnect (see e.g. USB 2.0
     * spec, section 7.1.7.3). */
    nvic_irq_disable(NVIC_USB_LP_CAN_RX0);
    gpio_write_bit(disc_dev, disc_bit, 1);
}

void usb_hid_putc(char ch) {
    while (!usb_hid_tx((uint8*)&ch, 1))
        ;
}

/* This function is non-blocking.
 *
 * It copies data from a usercode buffer into the USB peripheral TX
 * buffer, and returns the number of bytes copied. */
uint32 usb_hid_tx(const uint8* buf, uint32 len) {
    /* Last transmission hasn't finished, so abort. */
    if (usb_hid_is_transmitting()) {
        return 0;
    }

    /* We can only put USB_HID_TX_EPSIZE bytes in the buffer. */
    if (len > USB_HID_TX_EPSIZE) {
        len = USB_HID_TX_EPSIZE;
    }

    /* Queue bytes for sending. */
    if (len) {
        usb_copy_to_pma(buf, len, GetEPTxAddr(USB_HID_TX_ENDP));//USB_HID_TX_ADDR);
    }
    // We still need to wait for the interrupt, even if we're sending
    // zero bytes. (Sending zero-size packets is useful for flushing
    // host-side buffers.)
    usb_set_ep_tx_count(USB_HID_TX_ENDP, len);
    n_unsent_bytes = len;
    transmitting = 1;
    usb_set_ep_tx_stat(USB_HID_TX_ENDP, USB_EP_STAT_TX_VALID);

    return len;
}

uint32 usb_hid_data_available(void) {
    return n_unread_bytes;
}

uint8 usb_hid_is_transmitting(void) {
    return transmitting;
}

uint16 usb_hid_get_pending(void) {
    return n_unsent_bytes;
}

/* Nonblocking byte receive.
 *
 * Copies up to len bytes from our private data buffer (*NOT* the PMA)
 * into buf and deq's the FIFO. */
uint32 usb_hid_rx(uint8* buf, uint32 len) {
    /* Copy bytes to buffer. */
    uint32 n_copied = usb_hid_peek(buf, len);

    /* Mark bytes as read. */
    n_unread_bytes -= n_copied;
    rx_offset = (rx_offset + n_copied) % HID_BUFFER_SIZE;

    /* If all bytes have been read, re-enable the RX endpoint, which
     * was set to NAK when the current batch of bytes was received. */
    if (n_unread_bytes <= (HID_BUFFER_SIZE - USB_HID_RX_EPSIZE)) {
        usb_set_ep_rx_count(USB_HID_RX_ENDP, USB_HID_RX_EPSIZE);
        usb_set_ep_rx_stat(USB_HID_RX_ENDP, USB_EP_STAT_RX_VALID);
		rx_offset = 0;
    }

    return n_copied;
}

/*
 * Callbacks
 */

static void hidDataTxCb(void) {
    n_unsent_bytes = 0;
    transmitting = 0;
}

static void hidDataRxCb(void) {
	uint32 ep_rx_size;
	uint32 tail = (rx_offset + n_unread_bytes) % HID_BUFFER_SIZE;
	uint8 ep_rx_data[USB_HID_RX_EPSIZE];
	uint32 i;

    usb_set_ep_rx_stat(USB_HID_RX_ENDP, USB_EP_STAT_RX_NAK);
    ep_rx_size = usb_get_ep_rx_count(USB_HID_RX_ENDP);
    /* This copy won't overwrite unread bytes, since we've set the RX
     * endpoint to NAK, and will only set it to VALID when all bytes
     * have been read. */
    usb_copy_from_pma((uint8*)ep_rx_data, ep_rx_size,
                      USB_HID_RX_ADDR);

	for (i = 0; i < ep_rx_size; i++) {
		hidBufferRx[tail] = ep_rx_data[i];
		tail = (tail + 1) % HID_BUFFER_SIZE;
	}

	n_unread_bytes += ep_rx_size;

    if (n_unread_bytes <= (HID_BUFFER_SIZE - USB_HID_RX_EPSIZE)) {
        usb_set_ep_rx_count(USB_HID_RX_ENDP, USB_HID_RX_EPSIZE);
        usb_set_ep_rx_stat(USB_HID_RX_ENDP, USB_EP_STAT_RX_VALID);
    }
}

static void usbInit(void) {
    pInformation->Current_Configuration = 0;

    USB_BASE->CNTR = USB_CNTR_FRES;

    USBLIB->irq_mask = 0;
    USB_BASE->CNTR = USBLIB->irq_mask;
    USB_BASE->ISTR = 0;
    USBLIB->irq_mask = USB_CNTR_RESETM | USB_CNTR_SUSPM | USB_CNTR_WKUPM;
    USB_BASE->CNTR = USBLIB->irq_mask;

    USB_BASE->ISTR = 0;
    USBLIB->irq_mask = USB_ISR_MSK;
    USB_BASE->CNTR = USBLIB->irq_mask;

    nvic_irq_enable(NVIC_USB_LP_CAN_RX0);
    USBLIB->state = USB_UNCONNECTED;
}

#define BTABLE_ADDRESS        0x00
static void usbReset(void) {
    pInformation->Current_Configuration = 0;

    /* current feature is current bmAttributes */
    pInformation->Current_Feature = (USB_CONFIG_ATTR_BUSPOWERED |
                                     USB_CONFIG_ATTR_SELF_POWERED);

    USB_BASE->BTABLE = BTABLE_ADDRESS;

    /* setup control endpoint 0 */
    usb_set_ep_type(USB_EP0, USB_EP_EP_TYPE_CONTROL);
    usb_set_ep_tx_stat(USB_EP0, USB_EP_STAT_TX_STALL);
    usb_set_ep_rx_addr(USB_EP0, USB_HID_CTRL_RX_ADDR);
    usb_set_ep_tx_addr(USB_EP0, USB_HID_CTRL_TX_ADDR);
    usb_clear_status_out(USB_EP0);

    usb_set_ep_rx_count(USB_EP0, pProperty->MaxPacketSize);
    usb_set_ep_rx_stat(USB_EP0, USB_EP_STAT_RX_VALID);

    /* TODO figure out differences in style between RX/TX EP setup */

    /* set up data endpoint OUT (RX) */
    usb_set_ep_type(USB_HID_RX_ENDP, USB_EP_EP_TYPE_BULK);
    usb_set_ep_rx_addr(USB_HID_RX_ENDP, USB_HID_RX_ADDR);
    usb_set_ep_rx_count(USB_HID_RX_ENDP, USB_HID_RX_EPSIZE);
    usb_set_ep_rx_stat(USB_HID_RX_ENDP, USB_EP_STAT_RX_VALID);

    /* set up data endpoint IN (TX)  */
    usb_set_ep_type(USB_HID_TX_ENDP, USB_EP_EP_TYPE_BULK);
    usb_set_ep_tx_addr(USB_HID_TX_ENDP, USB_HID_TX_ADDR);
    usb_set_ep_tx_stat(USB_HID_TX_ENDP, USB_EP_STAT_TX_NAK);
    usb_set_ep_rx_stat(USB_HID_TX_ENDP, USB_EP_STAT_RX_DISABLED);

    USBLIB->state = USB_ATTACHED;
    SetDeviceAddress(0);

    /* Reset the RX/TX state */
    n_unread_bytes = 0;
    n_unsent_bytes = 0;
    rx_offset = 0;
    transmitting = 0;
}

static RESULT usbDataSetup(uint8 request) {
    uint8* (*CopyRoutine)(uint16) = 0;
	
	if ((request == GET_DESCRIPTOR)
		&& (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
		&& (pInformation->USBwIndex0 == 0)){
			
		if (pInformation->USBwValue1 == REPORT_DESCRIPTOR){
			CopyRoutine = HID_GetReportDescriptor;
		} else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE){
			CopyRoutine = HID_GetHIDDescriptor;
		}
		
	} /* End of GET_DESCRIPTOR */
	  /*** GET_PROTOCOL ***/
	else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))//){
			 && request == GET_PROTOCOL){
		CopyRoutine = HID_GetProtocolValue;
	}
	
	if (CopyRoutine == NULL){
		return USB_UNSUPPORT;
	}

    pInformation->Ctrl_Info.CopyData = CopyRoutine;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    (*CopyRoutine)(0);
    return USB_SUCCESS;
}

static RESULT usbNoDataSetup(uint8 request) {
	if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
		&& (request == SET_PROTOCOL)){
		uint8 wValue0 = pInformation->USBwValue0;
		ProtocolValue = wValue0;
		return USB_SUCCESS;
		//return HID_SetProtocol();
	}else{
		return USB_UNSUPPORT;
	}
}

static RESULT usbGetInterfaceSetting(uint8 interface, uint8 alt_setting) {
    if (alt_setting > 0) {
        return USB_UNSUPPORT;
    } else if (interface > 1) {
        return USB_UNSUPPORT;
    }

    return USB_SUCCESS;
}

static uint8* usbGetDeviceDescriptor(uint16 length) {
    return Standard_GetDescriptorData(length, &usbHIDDevice_Descriptor);
}

static uint8* usbGetConfigDescriptor(uint16 length) {
    return Standard_GetDescriptorData(length, &usbHIDConfig_Descriptor);
}

static uint8* usbGetStringDescriptor(uint16 length) {
    uint8 wValue0 = pInformation->USBwValue0;

    if (wValue0 > N_STRING_DESCRIPTORS) {
        return NULL;
    }
    return Standard_GetDescriptorData(length, &usbHIDString_Descriptor[wValue0]);
}

/*
static RESULT HID_SetProtocol(void){
	uint8 wValue0 = pInformation->USBwValue0;
	ProtocolValue = wValue0;
	return USB_SUCCESS;
}
*/
static uint8* HID_GetProtocolValue(uint16 Length){
	if (Length == 0){
		pInformation->Ctrl_Info.Usb_wLength = 1;
		return NULL;
	} else {
		return (uint8 *)(&ProtocolValue);
	}
}

static uint8* HID_GetReportDescriptor(uint16 Length){
  return Standard_GetDescriptorData(Length, &HID_Report_Descriptor);
}

static uint8* HID_GetHIDDescriptor(uint16 Length)
{
  return Standard_GetDescriptorData(Length, &usbHIDConfig_Descriptor);
}

static void usbSetConfiguration(void) {
    if (pInformation->Current_Configuration != 0) {
        USBLIB->state = USB_CONFIGURED;
    }
}

static void usbSetDeviceAddress(void) {
    USBLIB->state = USB_ADDRESSED;
}

#endif