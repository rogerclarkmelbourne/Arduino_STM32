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

#include "usb_generic.h"
#include "usb_hid.h"
#include <string.h>

uint16 GetEPTxAddr(uint8 /*bEpNum*/);

/* usb_lib headers */
#include "usb_type.h"
#include "usb_core.h"
#include "usb_def.h"


#include <libmaple/gpio.h>
#include <board/board.h>

static uint8 numEndpoints = 1;
static uint8 IdleValue = 1;
static uint8 ProtocolValue = 0;
static uint32 txEPSize = 64;
static uint32 rxEPSize = 64;
static uint8 txInterval = 0x0A;
static uint8 rxInterval = 0x0A;
static volatile int8 transmitting;
static struct usb_chunk* reportDescriptorChunks = NULL;

static void hidDataTxCb(void);
static void hidDataRxCb(void);
static void hidUSBReset(void);
static void usb_hid_clear(void);
static RESULT hidUSBDataSetup(uint8 request, uint8 interface, uint8 requestType, uint8 wValue0, uint8 wValue1, uint16 wIndex, uint16 wLength);
static RESULT hidUSBNoDataSetup(uint8 request, uint8 interface, uint8 requestType, uint8 wValue0, uint8 wValue1, uint16 wIndex);

static USBHIDOutputEndpointReceiver rxReceiver = NULL;
static void* rxReceiverExtra = NULL;
static volatile HIDBuffer_t hidBuffers[MAX_HID_BUFFERS] = {{ 0 }};
static volatile uint8* hidBufferRx = NULL;

#define HID_INTERFACE_OFFSET 	0x00
#define HID_INTERFACE_NUMBER (HID_INTERFACE_OFFSET+usbHIDPart.startInterface)

/*
 * Descriptors
 */
 

#define HID_ENDPOINT_TX      0
#define USB_HID_TX_ENDPOINT_INFO (&hidEndpoints[HID_ENDPOINT_TX])
#define USB_HID_TX_ENDP (hidEndpoints[HID_ENDPOINT_TX].address)

#define HID_ENDPOINT_RX      1
#define USB_HID_RX_ENDPOINT_INFO (&hidEndpoints[HID_ENDPOINT_RX])
#define USB_HID_RX_ENDP (hidEndpoints[HID_ENDPOINT_RX].address)

typedef struct {
    //HID
    usb_descriptor_interface     	HID_Interface;
	HIDDescriptor			 	 	HID_Descriptor;
    usb_descriptor_endpoint      	HIDDataInEndpoint;
    usb_descriptor_endpoint      	HIDDataOutEndpoint;
} __packed hid_part_config;

static const hid_part_config hidPartConfigData = {
	.HID_Interface = {
		.bLength            = sizeof(usb_descriptor_interface),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber   = HID_INTERFACE_OFFSET, // PATCH
        .bAlternateSetting  = 0x00,
        .bNumEndpoints      = 1, // PATCH    
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
        .bEndpointAddress = USB_DESCRIPTOR_ENDPOINT_IN | 0, // PATCH: USB_HID_TX_ENDP
        .bmAttributes     = USB_EP_TYPE_INTERRUPT,
        .wMaxPacketSize   = 64, //PATCH
        .bInterval        = 0x0A, //PATCH
	},
	.HIDDataOutEndpoint = {
		.bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = USB_DESCRIPTOR_ENDPOINT_OUT | 0, // PATCH: USB_HID_RX_ENDP
        .bmAttributes     = USB_EP_TYPE_INTERRUPT,
        .wMaxPacketSize   = 64, //PATCH
        .bInterval        = 0x0A, //PATCH
	},
};

#define SIZE_hidPartConfigData_ONE_ENDPOINT (sizeof(hidPartConfigData)-sizeof(hidPartConfigData.HIDDataOutEndpoint))
#define SIZE_hidPartConfigData_TWO_ENDPOINTS (sizeof(hidPartConfigData))

static ONE_DESCRIPTOR HID_Hid_Descriptor = {
    (uint8*)&hidPartConfigData.HID_Descriptor,
    sizeof(hidPartConfigData.HID_Descriptor)
};

void usb_hid_setTXInterval(uint8_t t) {
    txInterval = t;
}

void usb_hid_setRXInterval(uint8_t t) {
    rxInterval = t;
}

static USBEndpointInfo hidEndpoints[2] = {
    {
        .callback = hidDataTxCb,
        .pmaSize = 64,
        .type = USB_GENERIC_ENDPOINT_TYPE_INTERRUPT,
        .tx = 1,
    },
    {
        .callback = hidDataRxCb,
        .pmaSize = 64,
        .type = USB_GENERIC_ENDPOINT_TYPE_INTERRUPT,
        .tx = 0,
    }
};

void usb_hid_setTXEPSize(uint32_t size) {
    if (size == 0 || size > 64)
        size = 64;
    hidEndpoints[0].pmaSize = size;
    txEPSize = size;
}

#define OUT_BYTE(s,v) out[(uint8*)&(s.v)-(uint8*)&s]
#define OUT_16(s,v) *(uint16_t*)&OUT_BYTE(s,v) // OK on Cortex which can handle unaligned writes

static void getHIDPartDescriptor(uint8* out) {
    memcpy(out, &hidPartConfigData, numEndpoints > 1 ? SIZE_hidPartConfigData_TWO_ENDPOINTS : SIZE_hidPartConfigData_ONE_ENDPOINT);
    // patch to reflect where the part goes in the descriptor
    OUT_BYTE(hidPartConfigData, HID_Interface.bInterfaceNumber) += usbHIDPart.startInterface;
    OUT_BYTE(hidPartConfigData, HID_Interface.bNumEndpoints) = numEndpoints;
    OUT_BYTE(hidPartConfigData, HIDDataInEndpoint.bEndpointAddress) += USB_HID_TX_ENDP;
    uint16 size = usb_generic_chunks_length(reportDescriptorChunks);
    OUT_BYTE(hidPartConfigData, HID_Descriptor.descLenL) = (uint8)size;
    OUT_BYTE(hidPartConfigData, HID_Descriptor.descLenH) = (uint8)(size>>8);
    OUT_16(hidPartConfigData, HIDDataInEndpoint.wMaxPacketSize) = txEPSize;
    OUT_BYTE(hidPartConfigData, HIDDataInEndpoint.bInterval) = txInterval;
    if (numEndpoints > 1) {
        OUT_BYTE(hidPartConfigData, HIDDataOutEndpoint.bEndpointAddress) += USB_HID_RX_ENDP;
        OUT_BYTE(hidPartConfigData, HIDDataOutEndpoint.bInterval) = rxInterval;
        OUT_16(hidPartConfigData, HIDDataOutEndpoint.wMaxPacketSize) = rxEPSize;
    }
}

USBCompositePart usbHIDPart = {
    .numInterfaces = 1,
    .numEndpoints = sizeof(hidEndpoints)/sizeof(*hidEndpoints),
    .descriptorSize = SIZE_hidPartConfigData_ONE_ENDPOINT,
    .getPartDescriptor = getHIDPartDescriptor,
    .usbInit = NULL,
    .usbReset = hidUSBReset,
    .usbDataSetup = hidUSBDataSetup,
    .usbNoDataSetup = hidUSBNoDataSetup,
    .usbClearFeature = NULL,
    .usbSetConfiguration = NULL,
    .clear = usb_hid_clear,
    .endpoints = hidEndpoints
};

void usb_hid_setDedicatedRXEndpoint(void* buffer, uint16_t size, USBHIDOutputEndpointReceiver receiver, void* extra) {
    if (buffer != NULL) {
        numEndpoints = 2;
        usbHIDPart.descriptorSize = SIZE_hidPartConfigData_TWO_ENDPOINTS;
        hidEndpoints[1].pmaSize = size;
        rxEPSize = size;
        hidBufferRx = buffer;
        rxReceiver = receiver;
        rxReceiverExtra = extra;
    }
    else {
        numEndpoints = 1;
        usbHIDPart.descriptorSize = SIZE_hidPartConfigData_ONE_ENDPOINT;
        hidBufferRx = NULL;
        rxReceiver = NULL;
        rxReceiverExtra = NULL;
    }
}


#define HID_TX_BUFFER_SIZE	256 // must be power of 2
#define HID_TX_BUFFER_SIZE_MASK (HID_TX_BUFFER_SIZE-1)
// Tx data
static volatile uint8 hidBufferTx[HID_TX_BUFFER_SIZE];
// Write index to hidBufferTx
static volatile uint32 hid_tx_head = 0;
// Read index from hidBufferTx
static volatile uint32 hid_tx_tail = 0;

void usb_hid_set_report_descriptor(struct usb_chunk* chunks) {
    reportDescriptorChunks = chunks;
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
        usb_generic_pause_rx_ep0();
        unsigned delta = reportID != 0;
        memcpy((uint8*)buffer->buffer+delta, data, buffer->bufferSize-delta);
        if (reportID)
            buffer->buffer[0] = reportID;
        buffer->state = HID_BUFFER_READ;
        usb_generic_enable_rx_ep0();
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

    usb_generic_disable_interrupts_ep0();
    
    if (buffer->reportID == reportID && buffer->state != HID_BUFFER_EMPTY && !(poll && buffer->state == HID_BUFFER_READ)) {
        unsigned delta = reportID != 0;
        
        if (out != NULL)
            memcpy(out, (uint8*)buffer->buffer+delta, buffer->bufferSize-delta);
        
        if (poll) {
            buffer->state = HID_BUFFER_READ;
        }

        ret = buffer->bufferSize-delta;
    }
    
    if (! have_unread_data_in_hid_buffer() ) {
        usb_generic_enable_rx_ep0();
    }

    usb_generic_enable_interrupts_ep0();
            
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

static void usb_hid_clear(void) {
    ProtocolValue = 0;
    IdleValue = 1;
    usb_hid_clear_buffers(HID_REPORT_TYPE_OUTPUT);
    usb_hid_clear_buffers(HID_REPORT_TYPE_FEATURE);
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

	while(transmitting >= 0);
	
	if (transmitting<0) {
		hidDataTxCb(); // initiate data transmission
	}

    return len;
}



uint32 usb_hid_get_pending(void) {
    return (hid_tx_head - hid_tx_tail) & HID_TX_BUFFER_SIZE_MASK;
}


static void hidDataTxCb(void)
{
    usb_generic_send_from_circular_buffer(USB_HID_TX_ENDPOINT_INFO, 
        hidBufferTx, HID_TX_BUFFER_SIZE, hid_tx_head, &hid_tx_tail, &transmitting);
}

static void hidDataRxCb(void)
{
    USBEndpointInfo* ep = USB_HID_RX_ENDPOINT_INFO; 
    
    if (hidBufferRx != NULL) {
        uint32 didRead = usb_generic_read_to_buffer(ep, hidBufferRx, rxEPSize);

        if (didRead > 0) {
            if (rxReceiver != NULL)
                rxReceiver(rxReceiverExtra, hidBufferRx, didRead);
        }
    }
    
    usb_generic_enable_rx(ep);
}



static void hidUSBReset(void) {
    /* Reset the RX/TX state */
	hid_tx_head = 0;
	hid_tx_tail = 0;
    transmitting = -1;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
static RESULT hidUSBDataSetup(uint8 request, uint8 interface, uint8 requestType, uint8 wValue0, uint8 wValue1, uint16 wIndex, uint16 wLength) {
    (void)interface; // only one interface

    if ((requestType & (REQUEST_TYPE | RECIPIENT)) == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
    switch (request) {
        case SET_REPORT:
			if (wValue1 == HID_REPORT_TYPE_FEATURE || wValue1 == HID_REPORT_TYPE_OUTPUT) {
                volatile HIDBuffer_t* buffer = usb_hid_find_buffer(wValue1, wValue0);
				
				if (buffer == NULL) {
					return USB_UNSUPPORT;
				}
				
				if (0 == (buffer->mode & HID_BUFFER_MODE_NO_WAIT) && buffer->state == HID_BUFFER_UNREAD) {
					return USB_NOT_READY;
				} 
				else 
				{
                    buffer->state = HID_BUFFER_EMPTY;
                    usb_generic_control_rx_setup(buffer->buffer, buffer->bufferSize, &(buffer->state));
				}
                return USB_SUCCESS;
			}
            break;
        case GET_REPORT:
            if (wValue1 == HID_REPORT_TYPE_FEATURE) {
				volatile HIDBuffer_t* buffer = usb_hid_find_buffer(HID_REPORT_TYPE_FEATURE, wValue0);
				
				if (buffer == NULL || buffer->state == HID_BUFFER_EMPTY) {
					return USB_UNSUPPORT; // TODO: maybe UNREADY on empty
				}

                usb_generic_control_tx_setup(buffer->buffer, buffer->bufferSize, NULL);
                return USB_SUCCESS;
			}
        default:
            break;
        }
    }

  if((requestType & (REQUEST_TYPE | RECIPIENT)) == (STANDARD_REQUEST | INTERFACE_RECIPIENT)){
    	switch (request){
    		case GET_DESCRIPTOR:
				if (wValue1 == REPORT_DESCRIPTOR) {
                    usb_generic_control_tx_chunk_setup(reportDescriptorChunks);
                    return USB_SUCCESS;
                } 		
				else if (wValue1 == HID_DESCRIPTOR_TYPE){
                    usb_generic_control_descriptor_tx(&HID_Hid_Descriptor);
                    return USB_SUCCESS;
				} 		
			
    			break;
    		case GET_PROTOCOL:
                usb_generic_control_tx_setup(&ProtocolValue, 1, NULL);
                return USB_SUCCESS;

            case GET_IDLE:
                usb_generic_control_tx_setup(&IdleValue, 1, NULL);
                return USB_SUCCESS;
		}
	}

    return USB_UNSUPPORT;
}


static RESULT hidUSBNoDataSetup(uint8 request, uint8 interface, uint8 requestType, uint8 wValue0, uint8 wValue1, uint16 wIndex) {
    (void)interface; // only one interface
    
	if ((requestType & (REQUEST_TYPE | RECIPIENT)) == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
        switch(request) {
            case SET_PROTOCOL:
                ProtocolValue = wValue0;
                return USB_SUCCESS;
            case SET_IDLE:
                IdleValue = wValue0;
                return USB_SUCCESS;
        }
    }
    return USB_UNSUPPORT;
}

