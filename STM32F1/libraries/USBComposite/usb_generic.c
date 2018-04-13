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

#include <string.h>
#include <libmaple/libmaple_types.h>
#include <libmaple/usb.h>
#include <libmaple/nvic.h>
#include <libmaple/delay.h>
#include <libmaple/gpio.h>
#include <usb_lib_globals.h>
#include <usb_reg_map.h>
//#include <usb_core.h>
#include <board/board.h>


//uint16 GetEPTxAddr(uint8 /*bEpNum*/);

/* usb_lib headers */
#include "usb_type.h"
#include "usb_core.h"
#include "usb_def.h"

#include "usb_generic.h"

// Are we currently sending an IN packet?
volatile int8 usbGenericTransmitting = -1;

static uint8* usbGetConfigDescriptor(uint16 length);
static void usbInit(void);
static void usbReset(void);
static void usbClearFeature(void);
static void usbSetConfiguration(void);
static RESULT usbDataSetup(uint8 request);
static RESULT usbNoDataSetup(uint8 request);
static RESULT usbGetInterfaceSetting(uint8 interface, uint8 alt_setting);
static uint8* usbGetStringDescriptor(uint16 length);
static uint8* usbGetConfigDescriptor(uint16 length);
static uint8* usbGetDeviceDescriptor(uint16 length);
static void usbSetConfiguration(void);
static void usbSetDeviceAddress(void);

#define LEAFLABS_ID_VENDOR                0x1EAF
#define MAPLE_ID_PRODUCT                  0x0004 // was 0x0024
#define USB_DEVICE_CLASS              	  0x00
#define USB_DEVICE_SUBCLASS	           	  0x00
#define DEVICE_PROTOCOL					  0x01
#define HID_DESCRIPTOR_TYPE               0x21

static usb_descriptor_device usbGenericDescriptor_Device =
  {                                                                     
      .bLength            = sizeof(usb_descriptor_device),              
      .bDescriptorType    = USB_DESCRIPTOR_TYPE_DEVICE,                 
      .bcdUSB             = 0x0200,                                     
      .bDeviceClass       = USB_DEVICE_CLASS,                       	
      .bDeviceSubClass    = USB_DEVICE_SUBCLASS,                    	
      .bDeviceProtocol    = DEVICE_PROTOCOL,                            
      .bMaxPacketSize0    = 0x40,                                       
      .idVendor           = LEAFLABS_ID_VENDOR,                         
      .idProduct          = MAPLE_ID_PRODUCT,                           
      .bcdDevice          = 0x0200,                                     
      .iManufacturer      = 0x01,                                       
      .iProduct           = 0x02,                                       
      .iSerialNumber      = 0x00,                                       
      .bNumConfigurations = 0x01,                                       
};

typedef struct {
    usb_descriptor_config_header Config_Header;
    uint8 descriptorData[MAX_USB_DESCRIPTOR_DATA_SIZE];
} __packed usb_descriptor_config;

static usb_descriptor_config usbConfig;

#define MAX_POWER (100 >> 1)

static const usb_descriptor_config_header Base_Header = {
        .bLength              = sizeof(usb_descriptor_config_header),
        .bDescriptorType      = USB_DESCRIPTOR_TYPE_CONFIGURATION,
        .wTotalLength         = 0,
        .bNumInterfaces       = 0, 
        .bConfigurationValue  = 0x01,
        .iConfiguration       = 0x00,
        .bmAttributes         = (USB_CONFIG_ATTR_BUSPOWERED |
                                 USB_CONFIG_ATTR_SELF_POWERED),
        .bMaxPower            = MAX_POWER,
};

static ONE_DESCRIPTOR Device_Descriptor = {
    (uint8*)&usbGenericDescriptor_Device,
    sizeof(usb_descriptor_device)
};

static ONE_DESCRIPTOR Config_Descriptor = {
    (uint8*)&usbConfig,
    0
};

static DEVICE my_Device_Table = {
    .Total_Endpoint      = 0,
    .Total_Configuration = 1
};

/* Unicode language identifier: 0x0409 is US English */
/* FIXME move to Wirish */
static const usb_descriptor_string usbHIDDescriptor_LangID = {
    .bLength         = USB_DESCRIPTOR_STRING_LEN(1),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString         = {0x09, 0x04},
};

#define default_iManufacturer_length 8
const usb_descriptor_string usb_generic_default_iManufacturer = {
    .bLength         = USB_DESCRIPTOR_STRING_LEN(default_iManufacturer_length),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString         = {'L', 0, 'e', 0, 'a', 0, 'f', 0, 'L', 0, 'a', 0, 'b', 0, 's', 0},
};

#define default_iProduct_length 5
const usb_descriptor_string usb_generic_default_iProduct = {
    .bLength         = USB_DESCRIPTOR_STRING_LEN(default_iProduct_length),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString         = {'M', 0, 'a', 0, 'p', 0, 'l', 0, 'e', 0},
};


#define MAX_PACKET_SIZE            0x40  /* 64B, maximum for USB FS Devices */
static DEVICE_PROP my_Device_Property = {
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

static const USER_STANDARD_REQUESTS my_User_Standard_Requests = {
    .User_GetConfiguration   = NOP_Process,
    .User_SetConfiguration   = usbSetConfiguration,
    .User_GetInterface       = NOP_Process,
    .User_SetInterface       = NOP_Process,
    .User_GetStatus          = NOP_Process,
    .User_ClearFeature       = usbClearFeature,
    .User_SetEndPointFeature = NOP_Process,
    .User_SetDeviceFeature   = NOP_Process,
    .User_SetDeviceAddress   = usbSetDeviceAddress
};

static uint8 numStringDescriptors = 3;


#define MAX_STRING_DESCRIPTORS 4
static ONE_DESCRIPTOR String_Descriptor[MAX_STRING_DESCRIPTORS] = {
    {(uint8*)&usbHIDDescriptor_LangID,       USB_DESCRIPTOR_STRING_LEN(1)},
    {(uint8*)&usb_generic_default_iManufacturer,         USB_DESCRIPTOR_STRING_LEN(default_iManufacturer_length)},
    {(uint8*)&usb_generic_default_iProduct,              USB_DESCRIPTOR_STRING_LEN(default_iProduct_length)},
    {NULL,                                            0},
};

static USBCompositePart** parts;
static uint32 numParts;
static DEVICE saved_Device_Table;
static DEVICE_PROP saved_Device_Property;
static USER_STANDARD_REQUESTS saved_User_Standard_Requests;

static void (*ep_int_in[7])(void);
static void (*ep_int_out[7])(void);

uint8 usb_generic_set_parts(USBCompositePart** _parts, unsigned _numParts) {
    parts = _parts;
    numParts = _numParts;
    unsigned numInterfaces = 0;
    unsigned numEndpoints = 1;
    uint16 usbDescriptorSize = 0;
    uint16 pmaOffset = USB_EP0_RX_BUFFER_ADDRESS + USB_EP0_BUFFER_SIZE;
    
    for (unsigned i = 0 ; i < 7 ; i++) {
        ep_int_in[i] = NOP_Process;
        ep_int_out[i] = NOP_Process;
    }
    
    usbDescriptorSize = 0;
    for (unsigned i = 0 ; i < _numParts ; i++ ) {
        parts[i]->startInterface = numInterfaces;
        numInterfaces += parts[i]->numInterfaces;
        if (numEndpoints + parts[i]->numEndpoints > 8) {
            return 0;
		}
        if (usbDescriptorSize + parts[i]->descriptorSize > MAX_USB_DESCRIPTOR_DATA_SIZE) {
            return 0;
		}
        parts[i]->startEndpoint = numEndpoints;
        USBEndpointInfo* ep = parts[i]->endpoints;
        for (unsigned j = 0 ; j < parts[i]->numEndpoints ; j++) {
            if (ep[j].bufferSize + pmaOffset > PMA_MEMORY_SIZE) { 
                return 0;
			}
            ep[j].pmaAddress = pmaOffset;
            pmaOffset += ep[j].bufferSize;
            ep[j].address = numEndpoints;
            if (ep[j].callback == NULL)
                ep[j].callback = NOP_Process;
            if (ep[j].tx) {
                ep_int_in[numEndpoints - 1] = ep[j].callback;
            }
            else {
                ep_int_out[numEndpoints - 1] = ep[j].callback;
            }
            numEndpoints++;
        }
        parts[i]->getPartDescriptor(usbConfig.descriptorData + usbDescriptorSize);
        usbDescriptorSize += parts[i]->descriptorSize;
    }
    
    usbConfig.Config_Header = Base_Header;    
    usbConfig.Config_Header.bNumInterfaces = numInterfaces;
    usbConfig.Config_Header.wTotalLength = usbDescriptorSize + sizeof(Base_Header);
    Config_Descriptor.Descriptor_Size = usbConfig.Config_Header.wTotalLength;
    
    my_Device_Table.Total_Endpoint = numEndpoints;
        
    return 1;
}

void usb_generic_set_info( uint16 idVendor, uint16 idProduct, const uint8* iManufacturer, const uint8* iProduct, const uint8* iSerialNumber) {
    if (idVendor != 0)
        usbGenericDescriptor_Device.idVendor = idVendor;
    else
        usbGenericDescriptor_Device.idVendor = LEAFLABS_ID_VENDOR;
     
    if (idProduct != 0)
        usbGenericDescriptor_Device.idProduct = idProduct;
    else
        usbGenericDescriptor_Device.idProduct = MAPLE_ID_PRODUCT;
    
    if (iManufacturer == NULL) {
        iManufacturer = (uint8*)&usb_generic_default_iManufacturer;
    }
           
    String_Descriptor[1].Descriptor = (uint8*)iManufacturer;
    String_Descriptor[1].Descriptor_Size = iManufacturer[0];
     
    if (iProduct == NULL) {
        iProduct = (uint8*)&usb_generic_default_iProduct;
    }
           
    String_Descriptor[2].Descriptor = (uint8*)iProduct;
    String_Descriptor[2].Descriptor_Size = iProduct[0];
    
    if (iSerialNumber == NULL) {
        numStringDescriptors = 3;
        usbGenericDescriptor_Device.iSerialNumber = 0;
    }
    else {
        String_Descriptor[3].Descriptor = (uint8*)iSerialNumber;
        String_Descriptor[3].Descriptor_Size = iSerialNumber[0];
        numStringDescriptors = 4;
        usbGenericDescriptor_Device.iSerialNumber = 3;
    }    
}
 
void usb_generic_enable(void) {
    /* Present ourselves to the host. Writing 0 to "disc" pin must
     * pull USB_DP pin up while leaving USB_DM pulled down by the
     * transceiver. See USB 2.0 spec, section 7.1.7.3. */
     
#ifdef GENERIC_BOOTLOADER			
    //Reset the USB interface on generic boards - developed by Victor PV
    gpio_set_mode(GPIOA, 12, GPIO_OUTPUT_PP);
    gpio_write_bit(GPIOA, 12, 0);
    
    for(volatile unsigned int i=0;i<512;i++);// Only small delay seems to be needed
    gpio_set_mode(GPIOA, 12, GPIO_INPUT_FLOATING);
#endif			

    if (BOARD_USB_DISC_DEV != NULL) {
        gpio_set_mode(BOARD_USB_DISC_DEV, (uint8)(uint32)BOARD_USB_DISC_BIT, GPIO_OUTPUT_PP);
        gpio_write_bit(BOARD_USB_DISC_DEV, (uint8)(uint32)BOARD_USB_DISC_BIT, 0);
    }

    saved_Device_Table = Device_Table;
    saved_Device_Property = Device_Property;
    saved_User_Standard_Requests = User_Standard_Requests;
    Device_Table = my_Device_Table;
    Device_Property = my_Device_Property;
    User_Standard_Requests = my_User_Standard_Requests;
    
    /* Initialize the USB peripheral. */
    usb_init_usblib(USBLIB, ep_int_in, ep_int_out); 
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

    for (unsigned i = 0 ; i < numParts ; i++)
        if(parts[i]->usbInit != NULL)
            parts[i]->usbInit();

    USBLIB->state = USB_UNCONNECTED;
}

#define BTABLE_ADDRESS 0x00

static void usbReset(void) {
    pInformation->Current_Configuration = 0;

    /* current feature is current bmAttributes */
    pInformation->Current_Feature = (USB_CONFIG_ATTR_BUSPOWERED |
                                     USB_CONFIG_ATTR_SELF_POWERED);

    USB_BASE->BTABLE = BTABLE_ADDRESS;

    /* setup control endpoint 0 */
    usb_set_ep_type(USB_EP0, USB_EP_EP_TYPE_CONTROL);
    usb_set_ep_tx_stat(USB_EP0, USB_EP_STAT_TX_STALL);
    usb_set_ep_rx_addr(USB_EP0, USB_EP0_RX_BUFFER_ADDRESS);
    usb_set_ep_tx_addr(USB_EP0, USB_EP0_TX_BUFFER_ADDRESS);
    usb_clear_status_out(USB_EP0);

    usb_set_ep_rx_count(USB_EP0, USB_EP0_BUFFER_SIZE);
    usb_set_ep_rx_stat(USB_EP0, USB_EP_STAT_RX_VALID);
    
    for (unsigned i = 0 ; i < numParts ; i++) {
        for (unsigned j = 0 ; j < parts[i]->numEndpoints ; j++) {
            USBEndpointInfo* e = &(parts[i]->endpoints[j]);
            uint8 address = e->address;
            usb_set_ep_type(address, e->type);
            if (parts[i]->endpoints[j].tx) {
                usb_set_ep_tx_addr(address, e->pmaAddress);
                usb_set_ep_tx_stat(address, USB_EP_STAT_TX_NAK);
                usb_set_ep_rx_stat(address, USB_EP_STAT_RX_DISABLED);
            }
            else {
                usb_set_ep_rx_addr(address, e->pmaAddress);
                usb_set_ep_rx_count(address, e->bufferSize);
                usb_set_ep_rx_stat(address, USB_EP_STAT_RX_VALID);
            }
        }
        if (parts[i]->usbReset != NULL)
            parts[i]->usbReset();
    }
    
    usbGenericTransmitting = -1;
    
    USBLIB->state = USB_ATTACHED;
    SetDeviceAddress(0);

}

static void usb_power_down(void) {
    USB_BASE->CNTR = USB_CNTR_FRES;
    USB_BASE->ISTR = 0;
    USB_BASE->CNTR = USB_CNTR_FRES + USB_CNTR_PDWN;
}

void usb_generic_disable(void) {
    /* Turn off the interrupt and signal disconnect (see e.g. USB 2.0
     * spec, section 7.1.7.3). */
    nvic_irq_disable(NVIC_USB_LP_CAN_RX0);
    
    if (BOARD_USB_DISC_DEV != NULL) {
        gpio_write_bit(BOARD_USB_DISC_DEV, (uint8)(uint32)BOARD_USB_DISC_BIT, 1);
    }
    
    usb_power_down();

    Device_Table = saved_Device_Table;
    Device_Property = saved_Device_Property;
    User_Standard_Requests = saved_User_Standard_Requests;    
}

static RESULT usbDataSetup(uint8 request) {
    uint8* (*CopyRoutine)(uint16) = 0;
    
	if(Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT) && request == GET_DESCRIPTOR &&
        pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE){
            CopyRoutine = usbGetConfigDescriptor;
    }

	if (CopyRoutine == NULL){
        for (unsigned i = 0 ; i < numParts ; i++) {
            RESULT r = parts[i]->usbDataSetup(request);
            if (USB_UNSUPPORT != r)
                return r;
        }
        return USB_UNSUPPORT;
    }
    
    pInformation->Ctrl_Info.CopyData = CopyRoutine;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    (*CopyRoutine)(0);
    return USB_SUCCESS;
}

static RESULT usbNoDataSetup(uint8 request) {
    for (unsigned i = 0 ; i < numParts ; i++) {
        RESULT r = parts[i]->usbNoDataSetup(request);
        if (USB_UNSUPPORT != r)
            return r;
    }

    return USB_UNSUPPORT;
}

static void usbSetConfiguration(void) {
    if (pInformation->Current_Configuration != 0) {
        USBLIB->state = USB_CONFIGURED;
    }
    for (unsigned i = 0 ; i < numParts ; i++) {
        if (parts[i]->usbSetConfiguration != NULL)
            parts[i]->usbSetConfiguration();
    }
}

static void usbClearFeature(void) {
    for (unsigned i = 0 ; i < numParts ; i++) {
        if (parts[i]->usbClearFeature != NULL)
            parts[i]->usbClearFeature();
    }
}

static void usbSetDeviceAddress(void) {
    USBLIB->state = USB_ADDRESSED;
}

static uint8* usbGetDeviceDescriptor(uint16 length) {
    return Standard_GetDescriptorData(length, &Device_Descriptor);
}

static uint8* usbGetConfigDescriptor(uint16 length) {
    return Standard_GetDescriptorData(length, &Config_Descriptor);
}

static uint8* usbGetStringDescriptor(uint16 length) {    
    uint8 wValue0 = pInformation->USBwValue0;
    
    if (wValue0 >= numStringDescriptors) {
        return NULL;
    }
    return Standard_GetDescriptorData(length, &String_Descriptor[wValue0]);
}


static RESULT usbGetInterfaceSetting(uint8 interface, uint8 alt_setting) {
    if (alt_setting > 0) {
        return USB_UNSUPPORT;
    } else if (interface >= usbConfig.Config_Header.bNumInterfaces) {
        return USB_UNSUPPORT;
    }

    return USB_SUCCESS;
}

void usb_copy_to_pma(const uint8 *buf, uint16 len, uint16 pma_offset) {
    uint16 *dst = (uint16*)usb_pma_ptr(pma_offset);
    uint16 n = len >> 1;
    uint16 i;
    for (i = 0; i < n; i++) {
        *dst = (uint16)(*buf) | *(buf + 1) << 8;
        buf += 2;
        dst += 2;
    }
    if (len & 1) {
        *dst = *buf;
    }
}

void usb_copy_from_pma(uint8 *buf, uint16 len, uint16 pma_offset) {
    uint32 *src = (uint32*)usb_pma_ptr(pma_offset);
    uint16 *dst = (uint16*)buf;
    uint16 n = len >> 1;
    uint16 i;
    for (i = 0; i < n; i++) {
        *dst++ = *src++;
    }
    if (len & 1) {
        *dst = *src & 0xFF;
    }
}

