/*****************************f*************************************************
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
#include "usb_x360_generic.h"
#include "usb_x360w.h"

#include <string.h>

#include <libmaple/usb.h>
#include <libmaple/delay.h>

/* Private headers */
#include "usb_lib_globals.h"
#include "usb_reg_map.h"

/* usb_lib headers */
#include "usb_type.h"
#include "usb_core.h"
#include "usb_def.h"

typedef struct {
//    usb_descriptor_config_header Config_Header;
    usb_descriptor_interface     HID_Interface;
    uint8                        unknown_descriptor1[20];
    usb_descriptor_endpoint      DataInEndpoint;
    usb_descriptor_endpoint      DataOutEndpoint;
} __packed usb_descriptor_config;


#define MAX_POWER (100 >> 1)
static const usb_descriptor_config X360WDescriptor_Config =
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
        .bInterfaceProtocol = 129, 
        .iInterface         = 0x00,
	},
    
    .unknown_descriptor1 = { 
//        17,33,0,1,1,37,129/* PATCH 0x80 | USB_X360W_TX_ENDP */,20,0,0,0,0,19,2/* was 2, now PATCH: USB_X360W_RX_ENDP */,8,0,0,
        0x14,0x22,0x00,0x01,0x13,0x81/* PATCH 0x80 | USB_X360W_TX_ENDP */,0x1d,0x00,0x17,0x01,0x02,0x08,0x13,0x01/* PATCH: USB_X360W_RX_ENDP */,0x0c,0x00,0x0c,0x01,0x02,0x08
    }, 
	
	.DataInEndpoint = {
		.bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_IN | 0),//PATCH: USB_X360W_TX_ENDP
        .bmAttributes     = USB_EP_TYPE_INTERRUPT, 
        .wMaxPacketSize   = 0x20, 
        .bInterval        = 4, 
	},

    .DataOutEndpoint = {
        .bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_OUT | 0),//PATCH: USB_X360W_RX_ENDP
        .bmAttributes     = USB_EP_TYPE_INTERRUPT, 
        .wMaxPacketSize   = 0x20, 
        .bInterval        = 8, 
    },
};

#define OUT_BYTE(s,v) out[(uint8*)&(s.v)-(uint8*)&s]
#define OUT_16(s,v) *(uint16_t*)&OUT_BYTE(s,v) // OK on Cortex which can handle unaligned writes

static void getX360WPartDescriptor(uint8* out) {
    for(uint32 i=0; i<x360_num_controllers; i++) {
        memcpy(out, &X360WDescriptor_Config, sizeof(X360WDescriptor_Config));
        // patch to reflect where the part goes in the descriptor
        OUT_BYTE(X360WDescriptor_Config, HID_Interface.bInterfaceNumber) += usbX360Part.startInterface+X360_NUM_INTERFACES*i;
        uint8 rx_endp = USB_X360_RX_ENDP(i);
        uint8 tx_endp = USB_X360_TX_ENDP(i);
        OUT_BYTE(X360WDescriptor_Config, DataOutEndpoint.bEndpointAddress) += rx_endp;
        OUT_BYTE(X360WDescriptor_Config, DataInEndpoint.bEndpointAddress) += tx_endp;
        OUT_BYTE(X360WDescriptor_Config, unknown_descriptor1[5]) = 0x80 | tx_endp;
        OUT_BYTE(X360WDescriptor_Config, unknown_descriptor1[13]) = rx_endp;
        out += sizeof(X360WDescriptor_Config);
    }
}


void x360w_initialize_controller_data(uint32 _numControllers, uint8* buffers) {
    x360_generic_initialize_controller_data(_numControllers, buffers);
    usbX360Part.descriptorSize = sizeof(X360WDescriptor_Config) * x360_num_controllers;
    usbX360Part.getPartDescriptor = getX360WPartDescriptor;
}
