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
 * FIXME: this works on the STM32F1 USB peripherals, and probably no
 * place else. Nonportable bits really need to be factored out, and
 * the result made cleaner.
 */

#include "usb_generic.h"
#include "usb_x360.h"

#include <string.h>

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

typedef enum _HID_REQUESTS
{
 
  GET_REPORT = 1,
  GET_IDLE,
  GET_PROTOCOL,
 
  SET_REPORT = 9,
  SET_IDLE,
  SET_PROTOCOL
 
} HID_REQUESTS;

#define USB_ENDPOINT_IN(addr)           ((addr) | 0x80)
#define HID_ENDPOINT_INT 				1
#define USB_ENDPOINT_TYPE_INTERRUPT     0x03
 
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

#define X360_INTERFACE_OFFSET 0
#define X360_INTERFACE_NUMBER (X360_INTERFACE_OFFSET+usbX360Part.startInterface)
#define X360_ENDPOINT_TX 0
#define X360_ENDPOINT_RX 1
#define USB_X360_RX_ADDR x360Endpoints[X360_ENDPOINT_RX].pmaAddress
#define USB_X360_TX_ADDR x360Endpoints[X360_ENDPOINT_TX].pmaAddress
#define USB_X360_RX_ENDP x360Endpoints[X360_ENDPOINT_RX].address
#define USB_X360_TX_ENDP x360Endpoints[X360_ENDPOINT_TX].address

u16 GetEPTxAddr(u8 bEpNum);

static uint32 ProtocolValue;

static void x360DataTxCb(void);
static void x360DataRxCb(void);
static void (*x360_rumble_callback)(uint8 left, uint8 right);
static void (*x360_led_callback)(uint8 pattern);

static void x360Reset(void);
static RESULT x360DataSetup(uint8 request);
static RESULT x360NoDataSetup(uint8 request);
static uint8 *HID_GetProtocolValue(uint16 Length);

/*
 * Descriptors
 */
 
#if 0 
const uint8_t hid_report_descriptor[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
    0x09, 0x3a,                    //   USAGE (Counted Buffer)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x3f,                    //     USAGE (Reserved)
    0x09, 0x3b,                    //     USAGE (Byte Count)
    0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x35, 0x00,                    //     PHYSICAL_MINIMUM (0)
    0x45, 0x01,                    //     PHYSICAL_MAXIMUM (1)
    0x95, 0x04,                    //     REPORT_COUNT (4)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x0c,                    //     USAGE_MINIMUM (Button 12)
    0x29, 0x0f,                    //     USAGE_MAXIMUM (Button 15)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x35, 0x00,                    //     PHYSICAL_MINIMUM (0)
    0x45, 0x01,                    //     PHYSICAL_MAXIMUM (1)
    0x95, 0x04,                    //     REPORT_COUNT (4)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x09, 0x09,                    //     USAGE (Button 9)
    0x09, 0x0a,                    //     USAGE (Button 10)
    0x09, 0x07,                    //     USAGE (Button 7)
    0x09, 0x08,                    //     USAGE (Button 8)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x35, 0x00,                    //     PHYSICAL_MINIMUM (0)
    0x45, 0x01,                    //     PHYSICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x09, 0x05,                    //     USAGE (Button 5)
    0x09, 0x06,                    //     USAGE (Button 6)
    0x09, 0x0b,                    //     USAGE (Button 11)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x35, 0x00,                    //     PHYSICAL_MINIMUM (0)
    0x45, 0x01,                    //     PHYSICAL_MAXIMUM (1)
    0x95, 0x04,                    //     REPORT_COUNT (4)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x04,                    //     USAGE_MAXIMUM (Button 4)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //     LOGICAL_MAXIMUM (255)
    0x35, 0x00,                    //     PHYSICAL_MINIMUM (0)
    0x46, 0xff, 0x00,              //     PHYSICAL_MAXIMUM (255)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x32,                    //     USAGE (Z)
    0x09, 0x35,                    //     USAGE (Rz)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x16, 0x00, 0x80,              //     LOGICAL_MINIMUM (-32768)
    0x26, 0xff, 0x7f,              //     LOGICAL_MAXIMUM (32767)
    0x36, 0x00, 0x80,              //     PHYSICAL_MINIMUM (-32768)
    0x46, 0xff, 0x7f,              //     PHYSICAL_MAXIMUM (32767)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    //     USAGE (Pointer)
    0xa1, 0x00,                    //     COLLECTION (Physical)
    0x95, 0x02,                    //       REPORT_COUNT (2)
    0x05, 0x01,                    //       USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //       USAGE (X)
    0x09, 0x31,                    //       USAGE (Y)
    0x81, 0x02,                    //       INPUT (Data,Var,Abs)
    0xc0,                          //     END_COLLECTION
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    //     USAGE (Pointer)
    0xa1, 0x00,                    //     COLLECTION (Physical)
    0x95, 0x02,                    //       REPORT_COUNT (2)
    0x05, 0x01,                    //       USAGE_PAGE (Generic Desktop)
    0x09, 0x33,                    //       USAGE (Rx)
    0x09, 0x34,                    //       USAGE (Ry)
    0x81, 0x02,                    //       INPUT (Data,Var,Abs)
    0xc0,                          //     END_COLLECTION
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
};
#endif

typedef struct {
//    usb_descriptor_config_header Config_Header;
    usb_descriptor_interface     HID_Interface;
    uint8                        unknown_descriptor1[17];
    usb_descriptor_endpoint      DataInEndpoint;
    usb_descriptor_endpoint      DataOutEndpoint;
} __packed usb_descriptor_config;


#define MAX_POWER (100 >> 1)
static const usb_descriptor_config X360Descriptor_Config =
{
#if 0    
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
#endif    
	
	.HID_Interface = {
		.bLength            = sizeof(usb_descriptor_interface),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber   = X360_INTERFACE_OFFSET,  // PATCH
        .bAlternateSetting  = 0x00,
        .bNumEndpoints      = 0x02,  
        .bInterfaceClass    = 0xFF, 
        .bInterfaceSubClass = 0x5D,
        .bInterfaceProtocol = 0x01, 
        .iInterface         = 0x00,
	},
    
    .unknown_descriptor1 = {
        17,33,0,1,1,37,129,20,0,0,0,0,19,2,8,0,0,
    }, 
	
	.DataInEndpoint = {
		.bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_IN | X360_ENDPOINT_TX),//PATCH
        .bmAttributes     = USB_EP_TYPE_INTERRUPT, 
        .wMaxPacketSize   = 0x20, 
        .bInterval        = 4, 
	},

    .DataOutEndpoint = {
        .bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_OUT | X360_ENDPOINT_RX),//PATCH
        .bmAttributes     = USB_EP_TYPE_INTERRUPT, 
        .wMaxPacketSize   = 0x20, 
        .bInterval        = 8, 
    },
};

static USBEndpointInfo x360Endpoints[2] = {
    {
        .callback = x360DataTxCb,
        .bufferSize = 0x20,
        .type = USB_EP_EP_TYPE_INTERRUPT, 
        .tx = 1
    },
    {
        .callback = x360DataRxCb,
        .bufferSize = 0x20,
        .type = USB_EP_EP_TYPE_INTERRUPT, 
        .tx = 0,
    }
};

#define OUT_BYTE(s,v) out[(uint8*)&(s.v)-(uint8*)&s]

static void getX360PartDescriptor(uint8* out) {
    memcpy(out, &X360Descriptor_Config, sizeof(X360Descriptor_Config));
    // patch to reflect where the part goes in the descriptor
    OUT_BYTE(X360Descriptor_Config, HID_Interface.bInterfaceNumber) += usbX360Part.startInterface;
    OUT_BYTE(X360Descriptor_Config, DataOutEndpoint.bEndpointAddress) += usbX360Part.startEndpoint;
    OUT_BYTE(X360Descriptor_Config, DataInEndpoint.bEndpointAddress) += usbX360Part.startEndpoint;
}

USBCompositePart usbX360Part = {
    .numInterfaces = 1,
    .numEndpoints = sizeof(x360Endpoints)/sizeof(*x360Endpoints),
    .descriptorSize = sizeof(X360Descriptor_Config),
    .getPartDescriptor = getX360PartDescriptor,
    .usbInit = NULL,
    .usbReset = x360Reset,
    .usbDataSetup = x360DataSetup,
    .usbNoDataSetup = x360NoDataSetup,
    .endpoints = x360Endpoints
};


/*
 * Etc.
 */

/* I/O state */

/* Received data */
static volatile uint8 hidBufferRx[USB_X360_RX_EPSIZE];


/* Number of bytes left to transmit */
static volatile uint32 n_unsent_bytes = 0;
/* Are we currently sending an IN packet? */
static volatile uint8 transmitting = 0;


/*
 * HID interface
 */

void x360_set_rumble_callback(void (*callback)(uint8 left, uint8 right)) {
    x360_rumble_callback = callback;
}

void x360_set_led_callback(void (*callback)(uint8 pattern)) {
    x360_led_callback = callback;
}

/*void x360_disable(void) {
    x360_set_rumble_callback(NULL);
    x360_set_led_callback(NULL);
    usb_generic_disable();
}*/

void x360_putc(char ch) {
    while (!x360_tx((uint8*)&ch, 1))
        ;
}


/* This function is non-blocking.
 *
 * It copies data from a usercode buffer into the USB peripheral TX
 * buffer, and returns the number of bytes copied. */
uint32 x360_tx(const uint8* buf, uint32 len) {
    /* Last transmission hasn't finished, so abort. */
    if (x360_is_transmitting()) {
        return 0;
    }

    /* We can only put USB_X360_TX_EPSIZE bytes in the buffer. */
    if (len > USB_X360_TX_EPSIZE) {
        len = USB_X360_TX_EPSIZE;
    }

    /* Queue bytes for sending. */
    if (len) {
        usb_copy_to_pma(buf, len, GetEPTxAddr(USB_X360_TX_ENDP));//USB_X360_TX_ADDR);
    }
    // We still need to wait for the interrupt, even if we're sending
    // zero bytes. (Sending zero-size packets is useful for flushing
    // host-side buffers.)
    usb_set_ep_tx_count(USB_X360_TX_ENDP, len);
    n_unsent_bytes = len;
    transmitting = 1;
    usb_set_ep_tx_stat(USB_X360_TX_ENDP, USB_EP_STAT_TX_VALID);

    return len;
}

uint8 x360_is_transmitting(void) {
    return transmitting;
}

uint16 x360_get_pending(void) {
    return n_unsent_bytes;
}

static void x360DataRxCb(void)
{
	uint32 ep_rx_size = usb_get_ep_rx_count(USB_X360_RX_ENDP);
	// This copy won't overwrite unread bytes as long as there is 
	// enough room in the USB Rx buffer for next packet
	uint32 *src = usb_pma_ptr(USB_X360_RX_ADDR);
    uint16 tmp = 0;
	uint8 val;
	uint32 i;
	for (i = 0; i < ep_rx_size; i++) {
		if (i&1) {
			val = tmp>>8;
		} else {
			tmp = *src++;
			val = tmp&0xFF;
		}
		hidBufferRx[i] = val;
	}
    
    if (ep_rx_size == 3) {
        if (x360_led_callback != NULL && hidBufferRx[0] == 1 && hidBufferRx[1] == 3)
            x360_led_callback(hidBufferRx[2]);
    }
    else if (ep_rx_size == 8) {
        if (x360_rumble_callback != NULL && hidBufferRx[0] == 0 && hidBufferRx[1] == 8)
            x360_rumble_callback(hidBufferRx[3],hidBufferRx[4]);
    }
    usb_set_ep_rx_stat(USB_X360_RX_ENDP, USB_EP_STAT_RX_VALID);
}

/*
 * Callbacks
 */

static void x360DataTxCb(void) {
    n_unsent_bytes = 0;
    transmitting = 0;
}

static RESULT x360DataSetup(uint8 request) {
    uint8* (*CopyRoutine)(uint16) = 0;
	
#if 0			
	if (request == GET_DESCRIPTOR
        && pInformation->USBwIndex0 == X360_INTERFACE_NUMBER && 
		&& (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
		&& (pInformation->USBwIndex0 == 0)){
		if (pInformation->USBwValue1 == REPORT_DESCRIPTOR){
			CopyRoutine = HID_GetReportDescriptor;
		} else 
        if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE){
			CopyRoutine = HID_GetHIDDescriptor;
		}
		
	} /* End of GET_DESCRIPTOR */
	  /*** GET_PROTOCOL ***/
	else 
#endif            
        if(pInformation->USBwIndex0 == X360_INTERFACE_NUMBER && 
            (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
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

static RESULT x360NoDataSetup(uint8 request) {
	if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
		&& (request == SET_PROTOCOL)){
		uint8 wValue0 = pInformation->USBwValue0;
		ProtocolValue = wValue0;
		return USB_SUCCESS;
	}else{
		return USB_UNSUPPORT;
	}
}

static uint8* HID_GetProtocolValue(uint16 Length){
	if (Length == 0){
		pInformation->Ctrl_Info.Usb_wLength = 1;
		return NULL;
	} else {
		return (uint8 *)(&ProtocolValue);
	}
}

static void x360Reset(void) {
      /* Reset the RX/TX state */
    n_unsent_bytes = 0;
    transmitting = 0;
}