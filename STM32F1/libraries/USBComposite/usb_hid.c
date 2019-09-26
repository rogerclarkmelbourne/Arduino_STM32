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

#include "usb_hid.h"
#include <string.h>
#include <libmaple/usb.h>
#include <libmaple/nvic.h>
#include <libmaple/delay.h>

/* Private headers */
#include "usb_lib_globals.h"
#include "usb_reg_map.h"

uint16 GetEPTxAddr(uint8 /*bEpNum*/);

/* usb_lib headers */
#include "usb_type.h"
#include "usb_core.h"
#include "usb_def.h"

static uint32 ProtocolValue = 0;
static uint32 txEPSize = 64;

static void hidDataTxCb(void);
static void hidUSBReset(void);
static RESULT hidUSBDataSetup(uint8 request);
static RESULT hidUSBNoDataSetup(uint8 request);
//static RESULT usbGetInterfaceSetting(uint8 interface, uint8 alt_setting);
static uint8* HID_GetReportDescriptor(uint16 Length);
static uint8* HID_GetProtocolValue(uint16 Length);

static volatile HIDBuffer_t hidBuffers[MAX_HID_BUFFERS] = {{ 0 }};
static volatile HIDBuffer_t* currentHIDBuffer = NULL;

//#define DUMMY_BUFFER_SIZE 0x40 // at least as big as a buffer size

#define HID_INTERFACE_OFFSET 	0x00
#define NUM_HID_ENDPOINTS          1
#define HID_INTERFACE_NUMBER (HID_INTERFACE_OFFSET+usbHIDPart.startInterface)

/*
 * Descriptors
 */
 
static ONE_DESCRIPTOR HID_Report_Descriptor = {
    (uint8*)NULL,
    0
};


#define HID_ENDPOINT_TX      0

typedef struct {
    //HID
    usb_descriptor_interface     	HID_Interface;
	HIDDescriptor			 	 	HID_Descriptor;
    usb_descriptor_endpoint      	HIDDataInEndpoint;
} __packed hid_part_config;

static const hid_part_config hidPartConfigData = {
	.HID_Interface = {
		.bLength            = sizeof(usb_descriptor_interface),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber   = HID_INTERFACE_OFFSET, // PATCH
        .bAlternateSetting  = 0x00,
        .bNumEndpoints      = NUM_HID_ENDPOINTS,    
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
		.descLenL			= 0x00, //PATCH
		.descLenH			= 0x00, //PATCH
	},
	.HIDDataInEndpoint = {
		.bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = USB_DESCRIPTOR_ENDPOINT_IN | HID_ENDPOINT_TX, // PATCH
        .bmAttributes     = USB_ENDPOINT_TYPE_INTERRUPT,
        .wMaxPacketSize   = 64, //PATCH
        .bInterval        = 0x0A,
	}
};

static USBEndpointInfo hidEndpoints[1] = {
    {
        .callback = hidDataTxCb,
        .bufferSize = 64,
        .type = USB_EP_EP_TYPE_INTERRUPT, // TODO: interrupt???
        .tx = 1,
    }
};

void usb_hid_setTXEPSize(uint32_t size) {
    if (size == 0 || size > 64)
        size = 64;
    hidEndpoints[0].bufferSize = size;
    txEPSize = size;
}

#define OUT_BYTE(s,v) out[(uint8*)&(s.v)-(uint8*)&s]
#define OUT_16(s,v) *(uint16_t*)&OUT_BYTE(s,v) // OK on Cortex which can handle unaligned writes

static void getHIDPartDescriptor(uint8* out) {
    memcpy(out, &hidPartConfigData, sizeof(hid_part_config));
    // patch to reflect where the part goes in the descriptor
    OUT_BYTE(hidPartConfigData, HID_Interface.bInterfaceNumber) += usbHIDPart.startInterface;
    OUT_BYTE(hidPartConfigData, HIDDataInEndpoint.bEndpointAddress) += usbHIDPart.startEndpoint;
    OUT_BYTE(hidPartConfigData, HID_Descriptor.descLenL) = (uint8)HID_Report_Descriptor.Descriptor_Size;
    OUT_BYTE(hidPartConfigData, HID_Descriptor.descLenH) = (uint8)(HID_Report_Descriptor.Descriptor_Size>>8);
    OUT_16(hidPartConfigData, HIDDataInEndpoint.wMaxPacketSize) = txEPSize;
}

USBCompositePart usbHIDPart = {
    .numInterfaces = 1,
    .numEndpoints = sizeof(hidEndpoints)/sizeof(*hidEndpoints),
    .descriptorSize = sizeof(hid_part_config),
    .getPartDescriptor = getHIDPartDescriptor,
    .usbInit = NULL,
    .usbReset = hidUSBReset,
    .usbDataSetup = hidUSBDataSetup,
    .usbNoDataSetup = hidUSBNoDataSetup,
    .usbClearFeature = NULL,
    .usbSetConfiguration = NULL,
    .endpoints = hidEndpoints
};


#define HID_TX_BUFFER_SIZE	256 // must be power of 2
#define HID_TX_BUFFER_SIZE_MASK (HID_TX_BUFFER_SIZE-1)
// Tx data
static volatile uint8 hidBufferTx[HID_TX_BUFFER_SIZE];
// Write index to hidBufferTx
static volatile uint32 hid_tx_head = 0;
// Read index from hidBufferTx
static volatile uint32 hid_tx_tail = 0;

#define CDC_SERIAL_RX_BUFFER_SIZE	256 // must be power of 2
#define CDC_SERIAL_RX_BUFFER_SIZE_MASK (CDC_SERIAL_RX_BUFFER_SIZE-1)

 


void usb_hid_putc(char ch) {
    while (!usb_hid_tx((uint8*)&ch, 1))
        ;
}

    /*
static void hidStatusIn() {
    if (pInformation->ControlState == WAIT_STATUS_IN) {
        if (currentInFeature >= 0) {
            if (featureBuffers[currentInFeature].bufferSize == featureBuffers[currentInFeature].currentDataSize) 
                featureBuffers[currentInFeature].state = HID_BUFFER_UNREAD;
            currentInFeature = -1;
        }
        if (currentOutput >= 0) {
            if (outputBuffers[currentOutput].bufferSize == outputBuffers[currentOutput].currentDataSize) 
                outputBuffers[currentOutput].state = HID_BUFFER_UNREAD;
            currentOutput = -1;
        }
    }
}
    */

void usb_hid_set_report_descriptor(const uint8* report_descriptor, uint16 report_descriptor_length) {    
    HID_Report_Descriptor.Descriptor = (uint8*)report_descriptor;
    HID_Report_Descriptor.Descriptor_Size = report_descriptor_length;        
}

    
static volatile HIDBuffer_t* usb_hid_find_buffer(uint8 type, uint8 reportID) {
    uint8 typeTest = type == HID_REPORT_TYPE_OUTPUT ? HID_BUFFER_MODE_OUTPUT : 0;
    for (int i=0; i<MAX_HID_BUFFERS; i++) {
        if ( hidBuffers[i].buffer != NULL &&
             ( hidBuffers[i].mode & HID_BUFFER_MODE_OUTPUT ) == typeTest && 
             hidBuffers[i].reportID == reportID) {
            return hidBuffers+i;
        }
    }
    return NULL;
}

void usb_hid_set_feature(uint8 reportID, uint8* data) {
    volatile HIDBuffer_t* buffer = usb_hid_find_buffer(HID_REPORT_TYPE_FEATURE, reportID);
    if (buffer != NULL) {
        usb_set_ep_rx_stat(USB_EP0, USB_EP_STAT_RX_NAK);
        unsigned delta = reportID != 0;
        memcpy((uint8*)buffer->buffer+delta, data, buffer->bufferSize-delta);
        if (reportID)
            buffer->buffer[0] = reportID;
        buffer->currentDataSize = buffer->bufferSize;
        buffer->state = HID_BUFFER_READ;
        usb_set_ep_rx_stat(USB_EP0, USB_EP_STAT_RX_VALID);
        return;
    }
}

static uint8 have_unread_data_in_hid_buffer() {
    for (int i=0;i<MAX_HID_BUFFERS; i++) {
        if (hidBuffers[i].buffer != NULL && hidBuffers[i].state == HID_BUFFER_UNREAD)
            return 1;
    }
    return 0;
}

uint16_t usb_hid_get_data(uint8 type, uint8 reportID, uint8* out, uint8 poll) {
    volatile HIDBuffer_t* buffer;
    unsigned ret = 0;
    
    buffer = usb_hid_find_buffer(type, reportID);
    
    if (buffer == NULL)
        return 0;

    nvic_irq_disable(NVIC_USB_LP_CAN_RX0);

    if (buffer->reportID == reportID && buffer->state != HID_BUFFER_EMPTY && !(poll && buffer->state == HID_BUFFER_READ)) {
        if (buffer->bufferSize != buffer->currentDataSize) {
           buffer->state = HID_BUFFER_EMPTY;
           ret = 0;
        }
        else {
            unsigned delta = reportID != 0;
            if (out != NULL)
                memcpy(out, (uint8*)buffer->buffer+delta, buffer->bufferSize-delta);
            
            if (poll) {
                buffer->state = HID_BUFFER_READ;
            }

            ret = buffer->bufferSize-delta;
        }
    }
    
    if (! have_unread_data_in_hid_buffer() ) {
        usb_set_ep_rx_stat(USB_EP0, USB_EP_STAT_RX_VALID);
    }

    nvic_irq_enable(NVIC_USB_LP_CAN_RX0);
            
    return ret;
}

void usb_hid_clear_buffers(uint8 type) {
    uint8 typeTest = type == HID_REPORT_TYPE_OUTPUT ? HID_BUFFER_MODE_OUTPUT : 0;
    for (int i=0; i<MAX_HID_BUFFERS; i++) {
        if (( hidBuffers[i].mode & HID_BUFFER_MODE_OUTPUT ) == typeTest) {
            hidBuffers[i].buffer = NULL;
        }
    }
}

uint8 usb_hid_add_buffer(uint8 type, volatile HIDBuffer_t* buf) {
    if (type == HID_BUFFER_MODE_OUTPUT) 
        buf->mode |= HID_BUFFER_MODE_OUTPUT;
    else
        buf->mode &= ~HID_BUFFER_MODE_OUTPUT;
    memset((void*)buf->buffer, 0, buf->bufferSize);
    buf->buffer[0] = buf->reportID;

    volatile HIDBuffer_t* buffer = usb_hid_find_buffer(type, buf->reportID);

    if (buffer != NULL) {
        *buffer = *buf;
        return 1;
    }
    else {
        for (int i=0; i<MAX_HID_BUFFERS; i++) {
            if (hidBuffers[i].buffer == NULL) {
                hidBuffers[i] = *buf;
                return 1;
            }
        }
        return 0;
    }
}

void usb_hid_set_buffers(uint8 type, volatile HIDBuffer_t* bufs, int n) {
    uint8 typeMask = type == HID_REPORT_TYPE_OUTPUT ? HID_BUFFER_MODE_OUTPUT : 0;
    usb_hid_clear_buffers(type);
    for (int i=0; i<n; i++) {
        bufs[i].mode &= ~HID_REPORT_TYPE_OUTPUT;
        bufs[i].mode |= typeMask;
        usb_hid_add_buffer(type, bufs+i);
    }
    currentHIDBuffer = NULL;
}

/* This function is non-blocking.
 *
 * It copies data from a user buffer into the USB peripheral TX
 * buffer, and returns the number of bytes copied. */
uint32 usb_hid_tx(const uint8* buf, uint32 len)
{
	if (len==0) return 0; // no data to send

	uint32 head = hid_tx_head; // load volatile variable
	uint32 tx_unsent = (head - hid_tx_tail) & HID_TX_BUFFER_SIZE_MASK;

    // We can only put bytes in the buffer if there is place
    if (len > (HID_TX_BUFFER_SIZE-tx_unsent-1) ) {
        len = (HID_TX_BUFFER_SIZE-tx_unsent-1);
    }
	if (len==0) return 0; // buffer full

	uint16 i;
	// copy data from user buffer to USB Tx buffer
	for (i=0; i<len; i++) {
		hidBufferTx[head] = buf[i];
		head = (head+1) & HID_TX_BUFFER_SIZE_MASK;
	}
	hid_tx_head = head; // store volatile variable

	while(usbGenericTransmitting >= 0);
	
	if (usbGenericTransmitting<0) {
		hidDataTxCb(); // initiate data transmission
	}

    return len;
}



uint16 usb_hid_get_pending(void) {
    return (hid_tx_head - hid_tx_tail) & HID_TX_BUFFER_SIZE_MASK;
}

static void hidDataTxCb(void)
{
	uint32 tail = hid_tx_tail; // load volatile variable
	uint32 tx_unsent = (hid_tx_head - tail) & HID_TX_BUFFER_SIZE_MASK;
	if (tx_unsent==0) {
		if ( (--usbGenericTransmitting)==0) goto flush_hid; // no more data to send
		return; // it was already flushed, keep Tx endpoint disabled
	}
	usbGenericTransmitting = 1;
    // We can only send up to USBHID_CDCACM_TX_EPSIZE bytes in the endpoint.
    if (tx_unsent > txEPSize) {
        tx_unsent = txEPSize;
    }
	// copy the bytes from USB Tx buffer to PMA buffer
	uint32 *dst = usb_pma_ptr(usbHIDPart.endpoints[HID_ENDPOINT_TX].pmaAddress);
    uint16 tmp = 0;
	uint16 val;
	unsigned i;
	for (i = 0; i < tx_unsent; i++) {
		val = hidBufferTx[tail];
		tail = (tail + 1) & HID_TX_BUFFER_SIZE_MASK;
		if (i&1) {
			*dst++ = tmp | (val<<8);
		} else {
			tmp = val;
		}
	}
    if ( tx_unsent&1 ) {
        *dst = tmp;
    }
	hid_tx_tail = tail; // store volatile variable
    
flush_hid:
	// enable Tx endpoint
    usb_set_ep_tx_count(usbHIDPart.endpoints[HID_ENDPOINT_TX].address, tx_unsent);
    usb_set_ep_tx_stat(usbHIDPart.endpoints[HID_ENDPOINT_TX].address, USB_EP_STAT_TX_VALID);
}



static void hidUSBReset(void) {
    /* Reset the RX/TX state */
	hid_tx_head = 0;
	hid_tx_tail = 0;

    currentHIDBuffer = NULL;
}

static uint8* HID_Set(uint16 length) {
    if (currentHIDBuffer == NULL)
        return NULL;
    
    if (length ==0) {
        if ( (0 == (currentHIDBuffer->mode & HID_BUFFER_MODE_NO_WAIT)) && 
                currentHIDBuffer->state == HID_BUFFER_UNREAD && 
                pInformation->Ctrl_Info.Usb_wOffset < pInformation->USBwLengths.w) {
            pInformation->Ctrl_Info.Usb_wLength = 0xFFFF;
            return NULL;
        }

        uint16 len = pInformation->USBwLengths.w;
        if (len > currentHIDBuffer->bufferSize)
            len = currentHIDBuffer->bufferSize;
        
        currentHIDBuffer->currentDataSize = len;
        
        currentHIDBuffer->state = HID_BUFFER_EMPTY;
        
        if (pInformation->Ctrl_Info.Usb_wOffset < len) { 
            pInformation->Ctrl_Info.Usb_wLength = len - pInformation->Ctrl_Info.Usb_wOffset;
        }
        else {
            pInformation->Ctrl_Info.Usb_wLength = 0; 
        }

        return NULL;
    }
    
    if (pInformation->USBwLengths.w <= pInformation->Ctrl_Info.Usb_wOffset + pInformation->Ctrl_Info.PacketSize) {
        currentHIDBuffer->state = HID_BUFFER_UNREAD;
    }
    
    return (uint8*)currentHIDBuffer->buffer + pInformation->Ctrl_Info.Usb_wOffset;
}

static uint8* HID_GetFeature(uint16 length) {
    if (currentHIDBuffer == NULL)
        return NULL;
    
    unsigned wOffset = pInformation->Ctrl_Info.Usb_wOffset;
    
    if (length == 0)
    {
        pInformation->Ctrl_Info.Usb_wLength = currentHIDBuffer->bufferSize - wOffset;
        return NULL;
    }

    return (uint8*)currentHIDBuffer->buffer + wOffset;
}

static RESULT hidUSBDataSetup(uint8 request) {
    uint8* (*CopyRoutine)(uint16) = 0;
	
	if (pInformation->USBwIndex0 != HID_INTERFACE_NUMBER)
		return USB_UNSUPPORT;
    
    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
        switch (request) {
        case SET_REPORT:
			if (pInformation->USBwValue1 == HID_REPORT_TYPE_FEATURE) {
				volatile HIDBuffer_t* buffer = usb_hid_find_buffer(HID_REPORT_TYPE_FEATURE, pInformation->USBwValue0);
				
				if (buffer == NULL) {
					return USB_UNSUPPORT;
				}
				
				if (0 == (buffer->mode & HID_BUFFER_MODE_NO_WAIT) && buffer->state == HID_BUFFER_UNREAD) {
					return USB_NOT_READY;
				} 
				else 
				{
					currentHIDBuffer = buffer;
					CopyRoutine = HID_Set;        
				}
			}
			else if (pInformation->USBwValue1 == HID_REPORT_TYPE_OUTPUT) {
				volatile HIDBuffer_t* buffer = usb_hid_find_buffer(HID_REPORT_TYPE_OUTPUT, pInformation->USBwValue0);
					
				if (buffer == NULL) {
					return USB_UNSUPPORT;
				}
				
				if (0 == (buffer->mode & HID_BUFFER_MODE_NO_WAIT) && buffer->state == HID_BUFFER_UNREAD) {
					return USB_NOT_READY;
				} 
				else 
				{
					currentHIDBuffer = buffer;
					CopyRoutine = HID_Set;        
				}
			}
            break;
        case GET_REPORT:
            if (pInformation->USBwValue1 == HID_REPORT_TYPE_FEATURE) {
				volatile HIDBuffer_t* buffer = usb_hid_find_buffer(HID_REPORT_TYPE_FEATURE, pInformation->USBwValue0);
				
				if (buffer == NULL || buffer->state == HID_BUFFER_EMPTY) {
					return USB_UNSUPPORT;
				}

				currentHIDBuffer = buffer;
				CopyRoutine = HID_GetFeature;        
				break;
			}
        default:
            break;
        }
    }
	
	if(Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT)){
    	switch (request){
    		case GET_DESCRIPTOR:
				if (pInformation->USBwValue1 == REPORT_DESCRIPTOR){
					CopyRoutine = HID_GetReportDescriptor;
				} 					
    			break;
    		case GET_PROTOCOL: // TODO: check for interface number?
    			CopyRoutine = HID_GetProtocolValue;
    			break;
		}
	}

	if (CopyRoutine == NULL){
		return USB_UNSUPPORT;
	}
    
    pInformation->Ctrl_Info.CopyData = CopyRoutine;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    (*CopyRoutine)(0);
    return USB_SUCCESS;
}

static RESULT hidUSBNoDataSetup(uint8 request) {
	if (pInformation->USBwIndex0 != HID_INTERFACE_NUMBER)
		return USB_UNSUPPORT;
    
    RESULT ret = USB_UNSUPPORT;
    
	if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
        switch(request) {
            case SET_PROTOCOL:
                ProtocolValue = pInformation->USBwValue0;
                ret = USB_SUCCESS;
                break;
        }
    }
    return ret;
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

