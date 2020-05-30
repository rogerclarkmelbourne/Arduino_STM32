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

#include "usb_multi_serial.h"
#include "usb_generic.h"
#include <string.h>
#include <libmaple/usb.h>
#include <libmaple/delay.h>

/* Private headers */
#include "usb_lib_globals.h"
#include "usb_reg_map.h"

#define CDCACM_ENDPOINT_TX         0
#define CDCACM_ENDPOINT_MANAGEMENT 1
#define CDCACM_ENDPOINT_RX         2

#define NUM_SERIAL_ENDPOINTS       3
#define CCI_INTERFACE_OFFSET 	0x00
#define DCI_INTERFACE_OFFSET 	0x01
#define NUM_INTERFACES             2
#define SERIAL_MANAGEMENT_INTERFACE_NUMBER(i) (usbMultiSerialPart.startInterface+CCI_INTERFACE_OFFSET+(port)*NUM_INTERFACES)

#define CDC_SERIAL_RX_BUFFER_SIZE	256 // must be power of 2
#define CDC_SERIAL_RX_BUFFER_SIZE_MASK (CDC_SERIAL_RX_BUFFER_SIZE-1)
#define CDC_SERIAL_TX_BUFFER_SIZE	256 // must be power of 2
#define CDC_SERIAL_TX_BUFFER_SIZE_MASK (CDC_SERIAL_TX_BUFFER_SIZE-1)

#define USB_CDCACM_MANAGEMENT_ENDP(port)    (serialEndpoints[NUM_SERIAL_ENDPOINTS*(port)+CDCACM_ENDPOINT_MANAGEMENT].address)
#define USB_CDCACM_MANAGEMENT_ENDPOINT_INFO(port)   (&serialEndpoints[NUM_SERIAL_ENDPOINTS*(port)+CDCACM_ENDPOINT_MANAGEMENT])
#define USB_CDCACM_RX_ENDPOINT_INFO(port)   (&serialEndpoints[NUM_SERIAL_ENDPOINTS*(port)+CDCACM_ENDPOINT_RX])
#define USB_CDCACM_TX_ENDPOINT_INFO(port)   (&serialEndpoints[NUM_SERIAL_ENDPOINTS*(port)+CDCACM_ENDPOINT_TX])
#define USB_CDCACM_TX_ENDP(port)            (serialEndpoints[NUM_SERIAL_ENDPOINTS*(port)+CDCACM_ENDPOINT_TX].address)
#define USB_CDCACM_RX_ENDP(port)            (serialEndpoints[NUM_SERIAL_ENDPOINTS*(port)+CDCACM_ENDPOINT_RX].address)
#define USB_CDCACM_TX_PMA_PTR(port)         (serialEndpoints[NUM_SERIAL_ENDPOINTS*(port)+CDCACM_ENDPOINT_TX].pma)
#define USB_CDCACM_RX_PMA_PTR(port)         (serialEndpoints[NUM_SERIAL_ENDPOINTS*(port)+CDCACM_ENDPOINT_RX].pma)

/* usb_lib headers */
#include "usb_type.h"
#include "usb_core.h"
#include "usb_def.h"

static void serialUSBReset(void);
static RESULT serialUSBDataSetup(uint8 request, uint8 interface, uint8 requestType, uint8 wValue0, uint8 wValue1, uint16 wIndex, uint16 wLength);
static RESULT serialUSBNoDataSetup(uint8 request, uint8 interface, uint8 requestType, uint8 wValue0, uint8 wValue1, uint16 wIndex);
static void vcomDataTxCb0(void);
static void vcomDataRxCb0(void);
static void vcomDataTxCb1(void);
static void vcomDataRxCb1(void);
static void vcomDataTxCb2(void);
static void vcomDataRxCb2(void);

static volatile struct port_data {
    /* Received data */
    uint8* vcomBufferRx;
    /* Write index to vcomBufferRx */
    uint32 vcom_rx_head;
    /* Read index from vcomBufferRx */
    uint32 vcom_rx_tail;

    // Tx data
    uint8* vcomBufferTx;
    // Write index to vcomBufferTx
    uint32 vcom_tx_head;
    // Read index from vcomBufferTx
    uint32 vcom_tx_tail;
    composite_cdcacm_line_coding line_coding;
    uint8 line_dtr_rts;
    int8 transmitting;
    void (*rx_hook)(unsigned, void*);
    void (*iface_setup_hook)(unsigned, void*);
    uint32_t txEPSize;
    uint32_t rxEPSize;
} ports[USB_MULTI_SERIAL_MAX_PORTS] = {{0}};

static void vcomDataTxCb(uint32 port);
static void vcomDataRxCb(uint32 port);

static void vcomDataTxCb0(void) {
    vcomDataTxCb(0);
}

static void vcomDataTxCb1(void) {
    vcomDataTxCb(1);
}

static void vcomDataTxCb2(void) {
    vcomDataTxCb(2);
}

static void vcomDataRxCb0(void) {
    vcomDataRxCb(0);
}

static void vcomDataRxCb1(void) {
    vcomDataRxCb(1);
}

static void vcomDataRxCb2(void) {
    vcomDataRxCb(2);
}


static uint32 numPorts = 3; 

static void usb_multi_serial_clear(void) {
    memset((void*)ports, 0, sizeof ports);
    numPorts = USB_MULTI_SERIAL_MAX_PORTS;
}


/*
 * Descriptors
 */

typedef struct {
    //CDCACM
	IADescriptor 					IAD;
    usb_descriptor_interface     	CCI_Interface;
    CDC_FUNCTIONAL_DESCRIPTOR(2) 	CDC_Functional_IntHeader;
    CDC_FUNCTIONAL_DESCRIPTOR(2) 	CDC_Functional_CallManagement;
    CDC_FUNCTIONAL_DESCRIPTOR(1) 	CDC_Functional_ACM;
    CDC_FUNCTIONAL_DESCRIPTOR(2) 	CDC_Functional_Union;
    usb_descriptor_endpoint      	ManagementEndpoint;
    usb_descriptor_interface     	DCI_Interface;
    usb_descriptor_endpoint      	DataOutEndpoint;
    usb_descriptor_endpoint      	DataInEndpoint;
} __packed serial_part_config;


static const serial_part_config serialPartConfigData = {
	.IAD = {
		.bLength			= 0x08,
		.bDescriptorType	= 0x0B,
		.bFirstInterface	= CCI_INTERFACE_OFFSET, // PATCH
		.bInterfaceCount	= 0x02,
		.bFunctionClass		= 0x02,
		.bFunctionSubClass	= 0x02,
		.bFunctionProtocol	= 0x01,
		.iFunction			= 0x02,
	}, 
    .CCI_Interface = {
        .bLength            = sizeof(usb_descriptor_interface),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber   = CCI_INTERFACE_OFFSET, // PATCH
        .bAlternateSetting  = 0x00,
        .bNumEndpoints      = 0x01,
        .bInterfaceClass    = USB_INTERFACE_CLASS_CDC,
        .bInterfaceSubClass = USB_INTERFACE_SUBCLASS_CDC_ACM,
        .bInterfaceProtocol = 0x01, /* Common AT Commands */
        .iInterface         = 0x00,
    },

    .CDC_Functional_IntHeader = {
        .bLength         = CDC_FUNCTIONAL_DESCRIPTOR_SIZE(2),
        .bDescriptorType = 0x24,
        .SubType         = 0x00,
        .Data            = {0x01, 0x10},
    },

    .CDC_Functional_CallManagement = {
        .bLength         = CDC_FUNCTIONAL_DESCRIPTOR_SIZE(2),
        .bDescriptorType = 0x24,
        .SubType         = 0x01,
        .Data            = {0x03, DCI_INTERFACE_OFFSET}, // PATCH
    },

    .CDC_Functional_ACM = {
        .bLength         = CDC_FUNCTIONAL_DESCRIPTOR_SIZE(1),
        .bDescriptorType = 0x24,
        .SubType         = 0x02,
        .Data            = {0x06},
    },

    .CDC_Functional_Union = {
        .bLength         = CDC_FUNCTIONAL_DESCRIPTOR_SIZE(2),
        .bDescriptorType = 0x24,
        .SubType         = 0x06,
        .Data            = {CCI_INTERFACE_OFFSET, DCI_INTERFACE_OFFSET}, // PATCH, PATCH
    },

    .ManagementEndpoint = {
        .bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_IN |
                             0), // PATCH: CDCACM_ENDPOINT_MANAGEMENT
        .bmAttributes     = USB_EP_TYPE_INTERRUPT,
        .wMaxPacketSize   = USBHID_CDCACM_MANAGEMENT_EPSIZE,
        .bInterval        = 0xFF,
    },

    .DCI_Interface = {
        .bLength            = sizeof(usb_descriptor_interface),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber   = DCI_INTERFACE_OFFSET, // PATCH
        .bAlternateSetting  = 0x00,
        .bNumEndpoints      = 0x02,
        .bInterfaceClass    = USB_INTERFACE_CLASS_DIC,
        .bInterfaceSubClass = 0x00, /* None */
        .bInterfaceProtocol = 0x00, /* None */
        .iInterface         = 0x00,
    },

    .DataOutEndpoint = {
        .bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_OUT |
                             0), // patch: CDCACM_ENDPOINT_RX
        .bmAttributes     = USB_EP_TYPE_BULK,
        .wMaxPacketSize   = USB_MULTI_SERIAL_DEFAULT_RX_SIZE, // patch
        .bInterval        = 0x00,
    },

    .DataInEndpoint = {
        .bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_IN | 0), // PATCH: CDCACM_ENDPOINT_TX
        .bmAttributes     = USB_EP_TYPE_BULK,
        .wMaxPacketSize   = USB_MULTI_SERIAL_DEFAULT_TX_SIZE, // patch
        .bInterval        = 0x00,
    }
};

static USBEndpointInfo serialEndpoints[NUM_SERIAL_ENDPOINTS*USB_MULTI_SERIAL_MAX_PORTS] = {
    {
        .callback = vcomDataTxCb0,
        .pmaSize = USB_MULTI_SERIAL_DEFAULT_TX_SIZE, // patch
        .type = USB_GENERIC_ENDPOINT_TYPE_BULK,
        .tx = 1,
    },
    {
        .callback = NULL,
        .pmaSize = USBHID_CDCACM_MANAGEMENT_EPSIZE,
        .type = USB_GENERIC_ENDPOINT_TYPE_INTERRUPT,
        .tx = 1,
    },
    {
        .callback = vcomDataRxCb0,
        .pmaSize = USB_MULTI_SERIAL_DEFAULT_RX_SIZE, // patch
        .type = USB_GENERIC_ENDPOINT_TYPE_BULK,
        .tx = 0,
    },
    {
        .callback = vcomDataTxCb1,
        .pmaSize = USB_MULTI_SERIAL_DEFAULT_TX_SIZE, // patch
        .type = USB_GENERIC_ENDPOINT_TYPE_BULK,
        .tx = 1,
    },
    {
        .callback = NULL,
        .pmaSize = USBHID_CDCACM_MANAGEMENT_EPSIZE,
        .type = USB_GENERIC_ENDPOINT_TYPE_INTERRUPT,
        .tx = 1,
    },
    {
        .callback = vcomDataRxCb1,
        .pmaSize = USB_MULTI_SERIAL_DEFAULT_RX_SIZE, // patch
        .type = USB_GENERIC_ENDPOINT_TYPE_BULK,
        .tx = 0,
    },
    {
        .callback = vcomDataTxCb2,
        .pmaSize = USB_MULTI_SERIAL_DEFAULT_TX_SIZE, // patch
        .type = USB_GENERIC_ENDPOINT_TYPE_BULK,
        .tx = 1,
    },
    {
        .callback = NULL,
        .pmaSize = USBHID_CDCACM_MANAGEMENT_EPSIZE,
        .type = USB_GENERIC_ENDPOINT_TYPE_INTERRUPT,
        .tx = 1,
    },
    {
        .callback = vcomDataRxCb2,
        .pmaSize = USB_MULTI_SERIAL_DEFAULT_RX_SIZE, // patch
        .type = USB_GENERIC_ENDPOINT_TYPE_BULK,
        .tx = 0,
    },
};

#define OUT_BYTE(s,v) out[(uint8*)&(s.v)-(uint8*)&s]
#define OUT_16(s,v) *(uint16_t*)&OUT_BYTE(s,v) // OK on Cortex which can handle unaligned writes

static void getSerialPartDescriptor(uint8* _out) {
    uint8* out = _out;
    uint16 interface = usbMultiSerialPart.startInterface;
    for (uint32 i=0; i<numPorts; i++) {
        memcpy(out, &serialPartConfigData, sizeof(serial_part_config));

        // patch to reflect where the part goes in the descriptor
        OUT_BYTE(serialPartConfigData, ManagementEndpoint.bEndpointAddress) += USB_CDCACM_MANAGEMENT_ENDP(i);
        OUT_BYTE(serialPartConfigData, DataOutEndpoint.bEndpointAddress) += USB_CDCACM_RX_ENDP(i);
        OUT_BYTE(serialPartConfigData, DataInEndpoint.bEndpointAddress) += USB_CDCACM_TX_ENDP(i);

        OUT_BYTE(serialPartConfigData, IAD.bFirstInterface) += interface;
        OUT_BYTE(serialPartConfigData, CCI_Interface.bInterfaceNumber) += interface;
        OUT_BYTE(serialPartConfigData, DCI_Interface.bInterfaceNumber) += interface;
        OUT_BYTE(serialPartConfigData, CDC_Functional_CallManagement.Data[1]) += interface;
        OUT_BYTE(serialPartConfigData, CDC_Functional_Union.Data[0]) += interface;
        OUT_BYTE(serialPartConfigData, CDC_Functional_Union.Data[1]) += interface;
        
        OUT_16(serialPartConfigData, DataOutEndpoint.wMaxPacketSize) = ports[i].rxEPSize;
        OUT_16(serialPartConfigData, DataInEndpoint.wMaxPacketSize) = ports[i].txEPSize;
        
        out += sizeof(serial_part_config);
        interface += NUM_INTERFACES;
    }
}

void multi_serial_setTXEPSize(uint32 port, uint16_t size) {
    if (size == 0)
        size = 64;
    serialEndpoints[NUM_SERIAL_ENDPOINTS*port+0].pmaSize = size;
    ports[port].txEPSize = size;
}

void multi_serial_setRXEPSize(uint32 port, uint16_t size) {
    if (size == 0)
        size = 64;
    size = usb_generic_roundUpToPowerOf2(size);
    serialEndpoints[NUM_SERIAL_ENDPOINTS*port+2].pmaSize = size;
    ports[port].rxEPSize = size; 
}

USBCompositePart usbMultiSerialPart = {
    .numInterfaces = 6,
    .numEndpoints = sizeof(serialEndpoints)/sizeof(*serialEndpoints),
    .descriptorSize = sizeof(serial_part_config)*3,
    .getPartDescriptor = getSerialPartDescriptor,
    .usbInit = NULL,
    .usbReset = serialUSBReset,
    .usbDataSetup = serialUSBDataSetup,
    .usbNoDataSetup = serialUSBNoDataSetup,
    .clear = usb_multi_serial_clear,
    .endpoints = serialEndpoints
};

// buffers must hold enough space for all the rx and tx buffers
void multi_serial_initialize_port_data(uint32 _numPorts, uint8* buffers) {
    numPorts = _numPorts;
    
    for (uint32 i=0; i<numPorts; i++) {
        volatile struct port_data* p = &ports[i];
        p->line_coding.dwDTERate = 115200;
        p->line_coding.bCharFormat = USBHID_CDCACM_STOP_BITS_1;
        p->line_coding.bParityType = USBHID_CDCACM_PARITY_NONE;
        p->line_coding.bDataBits = 8;
        p->vcomBufferTx = buffers;
        buffers += CDC_SERIAL_TX_BUFFER_SIZE;
        p->vcomBufferRx = buffers;
        p->rxEPSize = USB_MULTI_SERIAL_DEFAULT_RX_SIZE;
        p->txEPSize = USB_MULTI_SERIAL_DEFAULT_TX_SIZE;
        buffers += CDC_SERIAL_RX_BUFFER_SIZE;
    }
    
    usbMultiSerialPart.numInterfaces = NUM_INTERFACES * numPorts;
    usbMultiSerialPart.descriptorSize = sizeof(serial_part_config) * numPorts;
    usbMultiSerialPart.numEndpoints = NUM_SERIAL_ENDPOINTS * numPorts;
}

/* Other state (line coding, DTR/RTS) */

/* DTR in bit 0, RTS in bit 1. */

void multi_serial_set_hooks(uint32 port, unsigned hook_flags, void (*hook)(unsigned, void*)) {
    volatile struct port_data* p = &ports[port];
    if (hook_flags & USBHID_CDCACM_HOOK_RX) {
        p->rx_hook = hook;
    }
    if (hook_flags & USBHID_CDCACM_HOOK_IFACE_SETUP) {
        p->iface_setup_hook = hook;
    }
}

/* This function is non-blocking.
 *
 * It copies data from a user buffer into the USB peripheral TX
 * buffer, and returns the number of bytes copied. */
uint32 multi_serial_tx(uint32 port, const uint8* buf, uint32 len)
{
	if (len==0) return 0; // no data to send
    
    volatile struct port_data* p = &ports[port];

	uint32 head = p->vcom_tx_head; // load volatile variable
	uint32 tx_unsent = (head - p->vcom_tx_tail) & CDC_SERIAL_TX_BUFFER_SIZE_MASK;

    // We can only put bytes in the buffer if there is place
    if (len > (CDC_SERIAL_TX_BUFFER_SIZE-tx_unsent-1) ) {
        len = (CDC_SERIAL_TX_BUFFER_SIZE-tx_unsent-1);
    }
	if (len==0) return 0; // buffer full

	uint16 i;
	// copy data from user buffer to USB Tx buffer
	for (i=0; i<len; i++) {
		p->vcomBufferTx[head] = buf[i];
		head = (head+1) & CDC_SERIAL_TX_BUFFER_SIZE_MASK;
	}
	p->vcom_tx_head = head; // store volatile variable
	
	while(p->transmitting >= 0);
	
	if (p->transmitting < 0) {
		vcomDataTxCb(port); // initiate data transmission
	}

    return len;
}



uint32 multi_serial_data_available(uint32 port) {
    volatile struct port_data* p = &ports[port];
    return (p->vcom_rx_head - p->vcom_rx_tail) & CDC_SERIAL_RX_BUFFER_SIZE_MASK;
}

uint16 multi_serial_get_pending(uint32 port) {
    volatile struct port_data* p = &ports[port];
    return (p->vcom_tx_head - p->vcom_tx_tail) & CDC_SERIAL_TX_BUFFER_SIZE_MASK;
}

/* Non-blocking byte receive.
 *
 * Copies up to len bytes from our private data buffer (*NOT* the PMA)
 * into buf and deq's the FIFO. */
uint32 multi_serial_rx(uint32 port, uint8* buf, uint32 len)
{
    volatile struct port_data* p = &ports[port];
    /* Copy bytes to buffer. */
    uint32 n_copied = multi_serial_peek(port, buf, len);

    /* Mark bytes as read. */
	uint16 tail = p->vcom_rx_tail; // load volatile variable
	tail = (tail + n_copied) & CDC_SERIAL_RX_BUFFER_SIZE_MASK;
	p->vcom_rx_tail = tail; // store volatile variable

	uint32 rx_unread = (p->vcom_rx_head - tail) & CDC_SERIAL_RX_BUFFER_SIZE_MASK;
    // If buffer was emptied to a pre-set value, re-enable the RX endpoint
    if ( rx_unread <= 64 ) { // experimental value, gives the best performance
        usb_generic_enable_rx(USB_CDCACM_RX_ENDPOINT_INFO(port));
	}
    return n_copied;
}

/* Non-blocking byte lookahead.
 *
 * Looks at unread bytes without marking them as read. */
uint32 multi_serial_peek(uint32 port, uint8* buf, uint32 len)
{
    volatile struct port_data* p = &ports[port];
    unsigned i;
    uint32 tail = p->vcom_rx_tail;
	uint32 rx_unread = (p->vcom_rx_head-tail) & CDC_SERIAL_RX_BUFFER_SIZE_MASK;

    if (len > rx_unread) {
        len = rx_unread;
    }

    for (i = 0; i < len; i++) {
        buf[i] = p->vcomBufferRx[tail];
        tail = (tail + 1) & CDC_SERIAL_RX_BUFFER_SIZE_MASK;
    }

    return len;
}

uint32 multi_serial_peek_ex(uint32 port, uint8* buf, uint32 offset, uint32 len)
{
    volatile struct port_data* p = &ports[port];
    unsigned i;
    uint32 tail = (p->vcom_rx_tail + offset) & CDC_SERIAL_RX_BUFFER_SIZE_MASK ;
	uint32 rx_unread = (p->vcom_rx_head-tail) & CDC_SERIAL_RX_BUFFER_SIZE_MASK;

    if (len + offset > rx_unread) {
        len = rx_unread - offset;
    }

    for (i = 0; i < len; i++) {
        buf[i] = p->vcomBufferRx[tail];
        tail = (tail + 1) & CDC_SERIAL_RX_BUFFER_SIZE_MASK;
    }

    return len;
}

/* Roger Clark. Added. for Arduino 1.0 API support of Serial.peek() */
int multi_serial_peek_char(uint32 port) 
{
    if (multi_serial_data_available(port) == 0) 
	{
		return -1;
    }

    return ports[port].vcomBufferRx[ports[port].vcom_rx_tail];
}

uint8 multi_serial_get_dtr(uint32 port) {
    return ((ports[port].line_dtr_rts & USBHID_CDCACM_CONTROL_LINE_DTR) != 0);
}

uint8 multi_serial_get_rts(uint32 port) {
    return ((ports[port].line_dtr_rts & USBHID_CDCACM_CONTROL_LINE_RTS) != 0);
}

void multi_serial_get_line_coding(uint32 port, composite_cdcacm_line_coding *ret) {
    volatile struct port_data* p = &ports[port];
    ret->dwDTERate = p->line_coding.dwDTERate;
    ret->bCharFormat = p->line_coding.bCharFormat;
    ret->bParityType = p->line_coding.bParityType;
    ret->bDataBits = p->line_coding.bDataBits;
}

int multi_serial_get_baud(uint32 port) {
    return ports[port].line_coding.dwDTERate;
}

int multi_serial_get_stop_bits(uint32 port) {
    return ports[port].line_coding.bCharFormat;
}

int multi_serial_get_parity(uint32 port) {
    return ports[port].line_coding.bParityType;
}

int multi_serial_get_n_data_bits(uint32 port) {
    return ports[port].line_coding.bDataBits;
}

/*
 * Callbacks
 */
static void vcomDataTxCb(uint32 port)
{
    volatile struct port_data* p = &ports[port];
    usb_generic_send_from_circular_buffer(USB_CDCACM_TX_ENDPOINT_INFO(port),
        p->vcomBufferTx, CDC_SERIAL_TX_BUFFER_SIZE, p->vcom_tx_head, &(p->vcom_tx_tail), &(p->transmitting));
}


static void vcomDataRxCb(uint32 port)
{
    volatile struct port_data* p = &ports[port];
	uint32 head = p->vcom_rx_head;
    usb_generic_read_to_circular_buffer(USB_CDCACM_RX_ENDPOINT_INFO(port),
                    p->vcomBufferRx, CDC_SERIAL_RX_BUFFER_SIZE, &head);
	p->vcom_rx_head = head; // store volatile variable

	uint32 rx_unread = (head - p->vcom_rx_tail) & CDC_SERIAL_RX_BUFFER_SIZE_MASK;
	// only enable further Rx if there is enough room to receive one more packet
	if ( rx_unread < (CDC_SERIAL_RX_BUFFER_SIZE-p->rxEPSize) ) {
        usb_generic_enable_rx(USB_CDCACM_RX_ENDPOINT_INFO(port));
	}

    if (p->rx_hook) {
        p->rx_hook(USBHID_CDCACM_HOOK_RX, 0);
    }
}

static void serialUSBReset(void) {
    //VCOM
    for (uint32 port = 0; port<numPorts; port++) {
        volatile struct port_data* p = &ports[port];
        p->vcom_rx_head = 0;
        p->vcom_rx_tail = 0;
        p->vcom_tx_head = 0;
        p->vcom_tx_tail = 0;
        p->transmitting = -1;
    }
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
static RESULT serialUSBDataSetup(uint8 request, uint8 interface, uint8 requestType, uint8 wValue0, uint8 wValue1, uint16 wIndex, uint16 wLength) {
    RESULT ret = USB_UNSUPPORT;
    
    if ((requestType & (REQUEST_TYPE | RECIPIENT)) == (CLASS_REQUEST | INTERFACE_RECIPIENT) && interface % NUM_INTERFACES == CCI_INTERFACE_OFFSET) {
            uint32 port = interface / NUM_INTERFACES;
            switch (request) {
            case USBHID_CDCACM_GET_LINE_CODING:
                usb_generic_control_tx_setup(&ports[port].line_coding, sizeof(ports[port].line_coding), NULL);
                ret = USB_SUCCESS;
                break;
            case USBHID_CDCACM_SET_LINE_CODING:
                usb_generic_control_rx_setup(&ports[port].line_coding, sizeof(ports[port].line_coding), NULL);
                ret = USB_SUCCESS;
                break;
            default:
                break;
            }
            /* Call the user hook. */
            if (ports[port].iface_setup_hook) {
                uint8 req_copy = request;
                ports[port].iface_setup_hook(USBHID_CDCACM_HOOK_IFACE_SETUP, &req_copy);
            }
    }
	
    return ret;
}

static RESULT serialUSBNoDataSetup(uint8 request, uint8 interface, uint8 requestType, uint8 wValue0, uint8 wValue1, uint16 wIndex) {
    RESULT ret = USB_UNSUPPORT;
    
	if ((requestType & (REQUEST_TYPE | RECIPIENT)) == (CLASS_REQUEST | INTERFACE_RECIPIENT) && interface % NUM_INTERFACES == CCI_INTERFACE_OFFSET) {
            volatile struct port_data* p = &ports[interface / NUM_INTERFACES];
            switch(request) {
                case USBHID_CDCACM_SET_COMM_FEATURE:
                    /* We support set comm. feature, but don't handle it. */
                    ret = USB_SUCCESS;
                    break;
                case USBHID_CDCACM_SET_CONTROL_LINE_STATE:
                    /* Track changes to DTR and RTS. */
                    p->line_dtr_rts = wValue0 &
                                        (USBHID_CDCACM_CONTROL_LINE_DTR |
                                         USBHID_CDCACM_CONTROL_LINE_RTS);
                    ret = USB_SUCCESS;
                    break;
            }
            /* Call the user hook. */
            if (p->iface_setup_hook) {
                uint8 req_copy = request;
                p->iface_setup_hook(USBHID_CDCACM_HOOK_IFACE_SETUP, &req_copy);
            }
    }
    return ret;
}

