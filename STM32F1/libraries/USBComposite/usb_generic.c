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

//#define MATCHING_ENDPOINT_RANGES // make RX and TX endpoints fall in the same range for each part

#include <string.h>
#include <libmaple/libmaple_types.h>
#include <libmaple/usb.h>
#include <libmaple/delay.h>
#include <libmaple/gpio.h>
#include <usb_lib_globals.h>
#include <usb_reg_map.h>
//#include <usb_core.h>
#include <board/board.h>



/* usb_lib headers */
#include "usb_type.h"
#include "usb_core.h"
#include "usb_def.h"

#include "usb_generic.h"

const char DEFAULT_PRODUCT[] = "Maple";
const char DEFAULT_MANUFACTURER[] = "LeafLabs";

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
static uint32 disconnect_delay = 500; // in microseconds

static struct usb_chunk* control_tx_chunk_list = NULL;
static uint8 control_tx_chunk_buffer[USB_EP0_BUFFER_SIZE];

static volatile uint8* control_tx_buffer = NULL;
static uint16 control_tx_length = 0;
static volatile uint8* control_tx_done = NULL;
static volatile uint8* control_rx_buffer = NULL;
static uint16 control_rx_length = 0;
static volatile uint8* control_rx_done = NULL;

uint16 epTypes[4] = {
    USB_EP_EP_TYPE_BULK,
    USB_EP_EP_TYPE_CONTROL,
    USB_EP_EP_TYPE_ISO, 
    USB_EP_EP_TYPE_INTERRUPT
};

#define LEAFLABS_ID_VENDOR                0x1EAF
#define MAPLE_ID_PRODUCT                  0x0024 // was 0x0024
#define USB_DEVICE_CLASS              	  0x00
#define USB_DEVICE_SUBCLASS	           	  0x00
#define DEVICE_PROTOCOL					  0x01
//#define REQUEST_TYPE                      0b01100000u
#define REQUEST_RECIPIENT                 0b00011111u

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
static const usb_descriptor_string usb_LangID = {
    .bLength         = USB_DESCRIPTOR_STRING_LEN(1),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    .bString         = {0x09, 0x04},
};

static USB_DESCRIPTOR_STRING(USB_DESCRIPTOR_STRING_LEN(USB_MAX_STRING_DESCRIPTOR_LENGTH)) string_descriptor_buffer = {
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING
};

static ONE_DESCRIPTOR generic_string_descriptor = {
    (uint8*)&string_descriptor_buffer, 0
};

#define MAX_PACKET_SIZE            0x40  /* 64B, maximum for USB FS Devices */
static const DEVICE_PROP my_Device_Property = {
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
    {(uint8*)&usb_LangID,                  USB_DESCRIPTOR_STRING_LEN(1)},
    {(uint8*)DEFAULT_MANUFACTURER,         0}, // a 0 in the length field indicates that we synthesize this from an asciiz string
    {(uint8*)DEFAULT_PRODUCT,              0},
    {NULL,                                 0},
};

static USBCompositePart** parts;
static uint32 numParts;
static DEVICE saved_Device_Table;
static DEVICE_PROP saved_Device_Property;
static USER_STANDARD_REQUESTS saved_User_Standard_Requests;

static void (*ep_int_in[7])(void);
static void (*ep_int_out[7])(void);

static uint8 minimum_address;

static uint8 acceptable_endpoint_number(unsigned partNum, unsigned endpointNum, uint8 address) {
    USBEndpointInfo* ep = &(parts[partNum]->endpoints[endpointNum]);
    for (unsigned i = 0 ; i <= partNum ; i++)
        for(unsigned j = 0 ; (i < partNum && j < parts[partNum]->numEndpoints) || (i == partNum && j < endpointNum) ; j++) {
            USBEndpointInfo* ep1 = &(parts[i]->endpoints[j]);
            if (ep1->address != address)
                continue;
            if (ep1->tx == ep->tx || ep->exclusive || ep1->exclusive || ep1->type != ep->type || ep1->doubleBuffer != ep->doubleBuffer)
                return 0;
        }
    return 1;
}

static int8 allocate_endpoint_address(unsigned partNum, unsigned endpointNum) {
    for (uint8 address = minimum_address ; address < 8 ; address++)
        if (acceptable_endpoint_number(partNum, endpointNum, address))
            return address;
    return -1;
}

uint8 usb_generic_set_parts(USBCompositePart** _parts, unsigned _numParts) {
    parts = _parts;
    numParts = _numParts;
    unsigned numInterfaces = 0;
    minimum_address = 1;
    uint8 maxAddress = 0;

    uint16 usbDescriptorSize = 0;
    uint16 pmaOffset = USB_EP0_RX_BUFFER_ADDRESS + USB_EP0_BUFFER_SIZE;
    
    for (unsigned i = 0 ; i < 7 ; i++) {
        ep_int_in[i] = NOP_Process;
        ep_int_out[i] = NOP_Process;
    }
    
    usbDescriptorSize = 0;
    for (unsigned i = 0 ; i < _numParts ; i++ ) {
        USBCompositePart* part = parts[i];
        part->startInterface = numInterfaces;
        numInterfaces += part->numInterfaces;
        if (usbDescriptorSize + part->descriptorSize > MAX_USB_DESCRIPTOR_DATA_SIZE) {
            return 0;
		}
        USBEndpointInfo* ep = part->endpoints;
        for (unsigned j = 0 ; j < part->numEndpoints ; j++) {
            if (ep[j].align) 
                minimum_address = maxAddress + 1;
            int8 address = allocate_endpoint_address(i, j);
            if (address < 0)
                return 0;

            ep[j].pma = usb_pma_ptr(pmaOffset);
            uint32 size = ep[j].pmaSize;
            // rx has special length alignment issues
            if (ep[j].doubleBuffer) {
                if (size <= 124 || ep[j].tx) {
                    size = (size+3)/4*4;
                }
                else {
                    size = (size+63)/64*64;
                }
            }
            else {
                if (size <= 62 || ep[j].tx) {
                    size = (size+1)/2*2;
                }
                else {
                    size = (size+31)/32*32;
                }
            }
            pmaOffset += size;
            if (pmaOffset > PMA_MEMORY_SIZE)
                return 0;
            if (ep[j].callback == NULL)
                ep[j].callback = NOP_Process;
            ep[j].address = address;
            if (ep[j].tx) {
                ep_int_in[address-1] = ep[j].callback;
            }
            else {
                ep_int_out[address-1] = ep[j].callback;
            }
            if (maxAddress < address)
                maxAddress = address;
        }
        part->getPartDescriptor(usbConfig.descriptorData + usbDescriptorSize);
        usbDescriptorSize += part->descriptorSize;
#ifdef MATCHING_ENDPOINT_RANGES        
        minimum_address = maxAddress + 1;
#endif
    }
    
    usbConfig.Config_Header = Base_Header;    
    usbConfig.Config_Header.bNumInterfaces = numInterfaces;
    usbConfig.Config_Header.wTotalLength = usbDescriptorSize + sizeof(Base_Header);
    Config_Descriptor.Descriptor_Size = usbConfig.Config_Header.wTotalLength;
    
    my_Device_Table.Total_Endpoint = maxAddress + 1;
    
    return 1;
}

void usb_generic_set_info(uint16 idVendor, uint16 idProduct, const char* iManufacturer, const char* iProduct, const char* iSerialNumber) {
    if (idVendor != 0)
        usbGenericDescriptor_Device.idVendor = idVendor;
    else
        usbGenericDescriptor_Device.idVendor = LEAFLABS_ID_VENDOR;
     
    if (idProduct != 0)
        usbGenericDescriptor_Device.idProduct = idProduct;
    else
        usbGenericDescriptor_Device.idProduct = MAPLE_ID_PRODUCT;
    
    if (iManufacturer == NULL) {
        iManufacturer = DEFAULT_MANUFACTURER;
    }
           
    String_Descriptor[1].Descriptor = (uint8*)iManufacturer;
    String_Descriptor[1].Descriptor_Size = 0;
     
    if (iProduct == NULL) {
        iProduct = DEFAULT_PRODUCT;
    }
           
    String_Descriptor[2].Descriptor = (uint8*)iProduct;
    String_Descriptor[2].Descriptor_Size = 0;
    
    if (iSerialNumber == NULL) {
        numStringDescriptors = 3;
        usbGenericDescriptor_Device.iSerialNumber = 0;
    }
    else {
        String_Descriptor[3].Descriptor = (uint8*)iSerialNumber;
        String_Descriptor[3].Descriptor_Size = 0;
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

    delay_us(disconnect_delay);
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

void usb_generic_set_disconnect_delay(uint32 delay) {
    disconnect_delay = delay;
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

    usb_generic_enable_interrupts_ep0();

    for (unsigned i = 0 ; i < numParts ; i++)
        if(parts[i]->usbInit != NULL)
            parts[i]->usbInit();

    USBLIB->state = USB_UNCONNECTED;
}

#define BTABLE_ADDRESS 0x00

static inline uint16 pma_ptr_to_offset(uint32* p) {
    return (uint16)(((uint32*)p-(uint32*)USB_PMA_BASE) * 2);
}

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
    
    for (unsigned i = 1 ; i < 8 ; i++) {
        usb_set_ep_rx_stat(i, USB_EP_STAT_RX_DISABLED);
        usb_clear_ep_dtog_rx(i);
        usb_set_ep_tx_stat(i, USB_EP_STAT_TX_DISABLED);
        usb_clear_ep_dtog_tx(i);
   }
    
    for (unsigned i = 0 ; i < numParts ; i++) {
        for (unsigned j = 0 ; j < parts[i]->numEndpoints ; j++) {
            USBEndpointInfo* e = &(parts[i]->endpoints[j]);
            uint8 address = e->address;
            usb_set_ep_type(address, epTypes[e->type]);
            usb_set_ep_kind(address, e->doubleBuffer ? USB_EP_EP_KIND_DBL_BUF : 0);
            uint16 pmaOffset = pma_ptr_to_offset(e->pma);
            if (e->tx) {
                usb_set_ep_tx_addr(address, pmaOffset);
                usb_set_ep_tx_stat(address, USB_EP_STAT_TX_NAK);
                if (e->doubleBuffer) {
                    usb_set_ep_tx_buf0_addr(address, pmaOffset);
                    usb_set_ep_tx_buf1_addr(address, pmaOffset+e->pmaSize/2);
                    usb_set_ep_tx_buf0_count(address, e->pmaSize/2);
                    usb_set_ep_tx_buf1_count(address, e->pmaSize/2);
                }
            }
            else {
                usb_set_ep_rx_addr(address, pmaOffset);
				if (! e->doubleBuffer) {
					usb_set_ep_rx_count(address, e->pmaSize);
				}
				usb_set_ep_rx_stat(address, USB_EP_STAT_RX_VALID);
            }
        }
        if (parts[i]->usbReset != NULL)
            parts[i]->usbReset();
    }
    
    control_rx_length = 0;
    control_tx_length = 0;
    
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
    usb_generic_disable_interrupts_ep0();
    
    if (BOARD_USB_DISC_DEV != NULL) {
        gpio_write_bit(BOARD_USB_DISC_DEV, (uint8)(uint32)BOARD_USB_DISC_BIT, 1);
    }
    
    usb_power_down();

    Device_Table = saved_Device_Table;
    Device_Property = saved_Device_Property;
    User_Standard_Requests = saved_User_Standard_Requests;    
    
    for (uint32 i=0; i < numParts; i++)
        if (parts[i]->clear)
            parts[i]->clear();
}

static uint8* control_data_tx(uint16 length) {
    unsigned wOffset = pInformation->Ctrl_Info.Usb_wOffset;
    
    if (length == 0) {
        pInformation->Ctrl_Info.Usb_wLength = control_tx_length - wOffset;
        return NULL;
    }

    if (control_tx_done && pInformation->USBwLengths.w <= wOffset + pInformation->Ctrl_Info.PacketSize) {
        *control_tx_done = USB_CONTROL_DONE; // this may be a bit premature, but it's our best try
    }

    if (control_tx_buffer == NULL)
        return NULL;
    else
        return (uint8*)control_tx_buffer + wOffset;
}

void usb_generic_control_tx_setup(volatile void* buffer, uint16 length, volatile uint8* done) {
    control_tx_buffer = buffer;
    control_tx_length = length;
    control_tx_done = done;
    pInformation->Ctrl_Info.CopyData = control_data_tx;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    control_data_tx(0);
}

uint32 usb_generic_chunks_length(struct usb_chunk* chunk) {
    uint32 l=0;
    
    while (chunk != NULL) {
        l += chunk->dataLength;
        chunk = chunk->next;
    }

    return l;
}

static uint8* control_data_chunk_tx(uint16 length) {
    unsigned wOffset = pInformation->Ctrl_Info.Usb_wOffset;
    
    if (length == 0) {
        pInformation->Ctrl_Info.Usb_wLength = usb_generic_chunks_length(control_tx_chunk_list) - wOffset;
        
        return NULL;
    }

    if (control_tx_chunk_list == NULL) {
        return NULL;
    }
    else {
        uint32 chunks_offset = 0;
        uint32 buf_offset = 0;
        
        for (struct usb_chunk* chunk = control_tx_chunk_list ; chunk != NULL && chunks_offset < wOffset + length ; chunk = chunk->next ) {
            uint32 len = chunk->dataLength;
            
            if (len == 0)
                continue;
            
            if (wOffset < chunks_offset + len) {
                /* need to copy some data from this chunk */
                uint32 start;
                if (wOffset <= chunks_offset) {
                    start = 0;
                }
                else {
                    start = wOffset - chunks_offset;
                }
                
                uint32 to_copy;
                if (wOffset + length <= chunks_offset + len) {
                    to_copy = wOffset + length - chunks_offset - start;
                }
                else {
                    to_copy = len - start;
                }
                
                memcpy(control_tx_chunk_buffer + buf_offset, chunk->data + start, to_copy);
                buf_offset += to_copy;
            }
            
            chunks_offset += len;
        }
        
        return (uint8*)control_tx_chunk_buffer;
    }
}

void usb_generic_control_tx_chunk_setup(struct usb_chunk* chunk) {
    control_tx_chunk_list = chunk;
    pInformation->Ctrl_Info.CopyData = control_data_chunk_tx;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    control_data_chunk_tx(0);
}

static uint8* control_data_rx(uint16 length) {
    unsigned wOffset = pInformation->Ctrl_Info.Usb_wOffset;
    
    if (length ==0) {
        uint16 len = pInformation->USBwLengths.w;
        if (len > control_rx_length)
            len = control_rx_length;
        
        if (wOffset < len) { 
            pInformation->Ctrl_Info.Usb_wLength = len - wOffset;
        }
        else {
            pInformation->Ctrl_Info.Usb_wLength = 0; 
        }

        return NULL;
    }
    
    if (control_rx_done && pInformation->USBwLengths.w <= wOffset + pInformation->Ctrl_Info.PacketSize) {
        *control_rx_done = USB_CONTROL_DONE; // this may be a bit premature, but it's our best try
    }
    
    if (control_rx_buffer == NULL)
        return NULL;
    else
        return (uint8*)control_rx_buffer + wOffset;
}

void usb_generic_control_rx_setup(volatile void* buffer, uint16 length, volatile uint8* done) {
    control_rx_buffer = buffer;
    control_rx_length = length;
    control_rx_done = done;
    pInformation->Ctrl_Info.CopyData = control_data_rx;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    control_data_rx(0);
}

void usb_generic_control_descriptor_tx(ONE_DESCRIPTOR* d) {
    usb_generic_control_tx_setup(d->Descriptor, d->Descriptor_Size, NULL);
}

static RESULT usbDataSetup(uint8 request) {
    if ((Type_Recipient & REQUEST_RECIPIENT) == INTERFACE_RECIPIENT) {
        uint8 interface  = pInformation->USBwIndex0;
        for (unsigned i = 0 ; i < numParts ; i++) {
            USBCompositePart* p = parts[i];
            if (p->usbDataSetup && p->startInterface <= interface && interface < p->startInterface + p->numInterfaces) {
                // uint8 request, uint8 interface, uint8 requestType, uint8 wValue0, uint8 wValue1, uint16 wIndex, uint16 wLength
                return parts[i]->usbDataSetup(request, interface - p->startInterface, pInformation->USBbmRequestType, pInformation->USBwValue0, 
                    pInformation->USBwValue1, pInformation->USBwIndex, pInformation->USBwLength);
            }
        }
    }

    return USB_UNSUPPORT;
    
}

static RESULT usbNoDataSetup(uint8 request) {
    if ((Type_Recipient & REQUEST_RECIPIENT) == INTERFACE_RECIPIENT) {
        uint8 interface  = pInformation->USBwIndex0;
        
        for (unsigned i = 0 ; i < numParts ; i++) {
            USBCompositePart* p = parts[i];
            // uint8 request, uint8 interface, uint8 requestType, uint8 wValue0, uint8 wValue1, uint16 wIndex, uint16 wLength
            if (p->usbNoDataSetup && p->startInterface <= interface && interface < p->startInterface + p->numInterfaces)
                return parts[i]->usbNoDataSetup(request, interface - p->startInterface, pInformation->USBbmRequestType, pInformation->USBwValue0, 
                    pInformation->USBwValue1, pInformation->USBwIndex);
        }
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
    
    ONE_DESCRIPTOR* d = &String_Descriptor[wValue0];
    if (d->Descriptor_Size != 0) {
        return Standard_GetDescriptorData(length, &String_Descriptor[wValue0]);
    }
    else {
        const char* s = (char*)d->Descriptor;
        
        uint32 i = 0;
        
        while(*s && i < USB_MAX_STRING_DESCRIPTOR_LENGTH) {
            string_descriptor_buffer.bString[i] = (uint8)*s;
            s++;
            i++;
        }
        
        string_descriptor_buffer.bLength = USB_DESCRIPTOR_STRING_LEN(i);
        generic_string_descriptor.Descriptor_Size = string_descriptor_buffer.bLength;
        if (length > string_descriptor_buffer.bLength)
            length = string_descriptor_buffer.bLength;
        return Standard_GetDescriptorData(length, &generic_string_descriptor);
    }            
}


static RESULT usbGetInterfaceSetting(uint8 interface, uint8 alt_setting) {
    if (alt_setting > 1) {
        return USB_UNSUPPORT;
    } else if (interface >= usbConfig.Config_Header.bNumInterfaces) {
        return USB_UNSUPPORT;
    }

    return USB_SUCCESS;
}

void usb_copy_to_pma_ptr(volatile const uint8 *buf, uint16 len, uint32* dst) {
    uint16 n = len >> 1;
    uint16 i;
    for (i = 0; i < n; i++) {
        *(uint16*)dst = (uint16)(*buf) | *(buf + 1) << 8;
        buf += 2;
        dst++;
    }
    if (len & 1) {
        *dst = *buf;
    }
}

void usb_copy_from_pma_ptr(volatile uint8 *buf, uint16 len, uint32* src) {
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

// return bytes read
uint32 usb_generic_read_to_circular_buffer(USBEndpointInfo* ep, volatile uint8* buf, uint32 circularBufferSize, volatile uint32* headP) {
    uint32 head = *headP;
    uint32 ep_rx_size = usb_get_ep_rx_count(ep->address);
    /* This copy won't overwrite unread bytes as long as there is
     * enough room in the USB Rx buffer for next packet */
    volatile uint32 *src = ep->pma;

    for (uint32 i = 0; i < ep_rx_size; i++) {
        uint16 tmp = *src++;
        buf[head] = (uint8)tmp;
        head = (head + 1) % circularBufferSize;
        i++;
        if (i >= ep_rx_size) {
            break;
        }
        buf[head] = (uint8)(tmp>>8);
        head = (head + 1) % circularBufferSize;
    }

    *headP = head;
    
    return ep_rx_size;
}

// returns number of bytes read
// buf should be uint16-aligned
uint32 usb_generic_read_to_buffer(USBEndpointInfo* ep, volatile uint8* buf, uint32 bufferSize) {
    uint32 ep_rx_size = usb_get_ep_rx_count(ep->address);
    if (ep_rx_size > bufferSize)
        ep_rx_size = bufferSize;
    usb_copy_from_pma_ptr(buf, ep_rx_size, ep->pma);
    return ep_rx_size;
}

uint32 usb_generic_send_from_buffer(USBEndpointInfo* ep, volatile uint8* buf, uint32 amount) {
    if (amount > ep->pmaSize)
        amount = ep->pmaSize;
    
    usb_copy_to_pma_ptr(buf, amount, ep->pma);
    usb_set_ep_tx_count(ep->address, amount);
    usb_generic_enable_tx(ep);
    
    return amount;
}

// transmitting = 1 when transmitting, 0 when done but not flushed, negative when done and flushed
uint32 usb_generic_send_from_circular_buffer(USBEndpointInfo* ep, volatile uint8* buf, uint32 circularBufferSize, uint32 head, volatile uint32* tailP, volatile int8* transmittingP) {
    uint32 tail = *tailP;
	int32 amount = (head - tail) % circularBufferSize;
    if (amount < 0) {
        // wish we could count on % returning a non-negative answer
        amount += circularBufferSize;
    }
    
	if (amount==0) {
        if (*transmittingP <= 0) {
            *transmittingP = -1;
            return 0; // it was already flushed, keep Tx endpoint disabled
        }
        *transmittingP = 0;
        goto flush; // no more data to send
	}
    
	*transmittingP = 1;

    if (amount > ep->pmaSize) {
        amount = ep->pmaSize;
    }
    
	// copy the bytes from USB Tx buffer to PMA buffer
	uint32 *dst = ep->pma;

    for (int32 i = 0; i < amount; i++) {
        uint16 low = buf[tail];
        tail = (tail + 1) % circularBufferSize;
        i++;
        if (i >= amount) {
            *dst = low;
            break;
        }
        *dst++ = ((uint16)buf[tail] << 8) | low;
        tail = (tail + 1) % circularBufferSize;
    }

    *tailP = tail; /* store volatile variable */
    
flush:
	// enable Tx endpoint
    usb_set_ep_tx_count(ep->address, amount);
    usb_generic_enable_tx(ep);
    
    return amount;
}


uint32 usb_generic_send_from_circular_buffer_double_buffered(USBEndpointInfo* ep, volatile uint8* buf, uint32 circularBufferSize, uint32 amount, volatile uint32* tailP) {
    uint32 tail = *tailP; /* load volatile variable */

    uint32 dtog_tx = usb_get_ep_dtog_tx(ep->address);

    /* copy the bytes from USB Tx buffer to PMA buffer */
    uint32 *dst;
    if (dtog_tx)
        dst = PMA_PTR_BUF1(ep);
    else
        dst = PMA_PTR_BUF0(ep);
    
    if (amount > ep->pmaSize / 2)
        amount = ep->pmaSize / 2;

    for (uint32 i = 0; i < amount; i++) {
        uint16 low = buf[tail];
        tail = (tail + 1) % circularBufferSize;
        i++;
        if (i >= amount) {
            *dst = low;
            break;
        }
        *dst++ = ((uint16)buf[tail] << 8) | low;
        tail = (tail + 1) % circularBufferSize;
    }

    *tailP = tail; /* store volatile variable */

    if (dtog_tx)
        usb_set_ep_tx_buf1_count(ep->address, amount);
    else
        usb_set_ep_tx_buf0_count(ep->address, amount);
    
    return amount;
}


uint16_t usb_generic_roundUpToPowerOf2(uint16_t x) {
    uint16_t xx;
    for (xx = 1 ; xx < x && xx != 0; xx *= 2) ;
    if (xx == 0)
        return x;
    else
        return xx;
}
