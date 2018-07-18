/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs LLC.
 * Copyright (c) 2013 Magnus Lundin.
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
 * @file libmaple/usb/stm32f1/usb_midi_device.c
 * @brief USB MIDI.
 *
 * FIXME: this works on the STM32F1 USB peripherals, and probably no
 * place else. Nonportable bits really need to be factored out, and
 * the result made cleaner.
 */

#include <string.h>
#include "usb_generic.h"
#include "usb_midi_device.h"
#include <MidiSpecs.h>
#include <MinSysex.h>

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

static void midiDataTxCb(void);
static void midiDataRxCb(void);

static void usbMIDIReset(void);
static RESULT usbMIDIDataSetup(uint8 request);
static RESULT usbMIDINoDataSetup(uint8 request);

#define MIDI_ENDPOINT_RX 0
#define MIDI_ENDPOINT_TX 1
#define USB_MIDI_RX_ENDP (midiEndpoints[MIDI_ENDPOINT_RX].address)
#define USB_MIDI_TX_ENDP (midiEndpoints[MIDI_ENDPOINT_TX].address)
#define USB_MIDI_RX_ADDR (midiEndpoints[MIDI_ENDPOINT_RX].pmaAddress)
#define USB_MIDI_TX_ADDR (midiEndpoints[MIDI_ENDPOINT_TX].pmaAddress)

/*
 * Descriptors
 */

typedef struct {
//    usb_descriptor_config_header       Config_Header;
    /* Control Interface */
    usb_descriptor_interface           AC_Interface;
    AC_CS_INTERFACE_DESCRIPTOR(1)      AC_CS_Interface;
    /* Control Interface */
    usb_descriptor_interface           MS_Interface;
    MS_CS_INTERFACE_DESCRIPTOR         MS_CS_Interface;
    MIDI_IN_JACK_DESCRIPTOR            MIDI_IN_JACK_1;
    MIDI_IN_JACK_DESCRIPTOR            MIDI_IN_JACK_2;
    MIDI_OUT_JACK_DESCRIPTOR(1)        MIDI_OUT_JACK_3;
    MIDI_OUT_JACK_DESCRIPTOR(1)        MIDI_OUT_JACK_4;
    usb_descriptor_endpoint            DataOutEndpoint;
    MS_CS_BULK_ENDPOINT_DESCRIPTOR(1)  MS_CS_DataOutEndpoint;
    usb_descriptor_endpoint            DataInEndpoint;
    MS_CS_BULK_ENDPOINT_DESCRIPTOR(1)  MS_CS_DataInEndpoint;
} __packed usb_descriptor_config;

static const usb_descriptor_config usbMIDIDescriptor_Config = {
    /* .Config_Header = {
        .bLength              = sizeof(usb_descriptor_config_header),
        .bDescriptorType      = USB_DESCRIPTOR_TYPE_CONFIGURATION,
        .wTotalLength         = sizeof(usb_descriptor_config),
        .bNumInterfaces       = 0x02,
        .bConfigurationValue  = 0x01,
        .iConfiguration       = 0x00,
        .bmAttributes         = (USB_CONFIG_ATTR_BUSPOWERED |
                                 USB_CONFIG_ATTR_SELF_POWERED),
        .bMaxPower            = MAX_POWER,
    }, */

    .AC_Interface = {
        .bLength            = sizeof(usb_descriptor_interface),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber   = 0x00, // PATCH
        .bAlternateSetting  = 0x00,
        .bNumEndpoints      = 0x00, 
        .bInterfaceClass    = USB_INTERFACE_CLASS_AUDIO,
        .bInterfaceSubClass = USB_INTERFACE_AUDIOCONTROL,
        .bInterfaceProtocol = 0x00,
        .iInterface         = 0x00,
    },

    .AC_CS_Interface = {
        .bLength            = AC_CS_INTERFACE_DESCRIPTOR_SIZE(1),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = 0x01,
        .bcdADC             = 0x0100,
        .wTotalLength       = AC_CS_INTERFACE_DESCRIPTOR_SIZE(1),
        .bInCollection      = 0x01,
        .baInterfaceNr      = {0x01},
    },

    .MS_Interface = {
        .bLength            = sizeof(usb_descriptor_interface),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber   = 0x01, // PATCH
        .bAlternateSetting  = 0x00,
        .bNumEndpoints      = 0x02,
        .bInterfaceClass    = USB_INTERFACE_CLASS_AUDIO,
        .bInterfaceSubClass = USB_INTERFACE_MIDISTREAMING,
        .bInterfaceProtocol = 0x00,
        .iInterface         = 0, // was 0x04
    },

    .MS_CS_Interface = {
        .bLength            = sizeof(MS_CS_INTERFACE_DESCRIPTOR),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = 0x01,
        .bcdADC             = 0x0100,
        .wTotalLength       = sizeof(MS_CS_INTERFACE_DESCRIPTOR)
                              +sizeof(MIDI_IN_JACK_DESCRIPTOR)
                              +sizeof(MIDI_IN_JACK_DESCRIPTOR)
                              +MIDI_OUT_JACK_DESCRIPTOR_SIZE(1)
                              +MIDI_OUT_JACK_DESCRIPTOR_SIZE(1)
                              +sizeof(usb_descriptor_endpoint)
                              +MS_CS_BULK_ENDPOINT_DESCRIPTOR_SIZE(1)
                              +sizeof(usb_descriptor_endpoint)
                              +MS_CS_BULK_ENDPOINT_DESCRIPTOR_SIZE(1)                             
                                 /* 0x41-4 */,
    },

    .MIDI_IN_JACK_1 = {
        .bLength            = sizeof(MIDI_IN_JACK_DESCRIPTOR),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_IN_JACK,
        .bJackType          = MIDI_JACK_EMBEDDED,
        .bJackId            = 0x01,
        .iJack              = 0x05,
    },

    .MIDI_IN_JACK_2 = {
        .bLength            = sizeof(MIDI_IN_JACK_DESCRIPTOR),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_IN_JACK,
        .bJackType          = MIDI_JACK_EXTERNAL,
        .bJackId            = 0x02,
        .iJack              = 0x00,
    },

    .MIDI_OUT_JACK_3 = {
        .bLength            = MIDI_OUT_JACK_DESCRIPTOR_SIZE(1),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_OUT_JACK,
        .bJackType          = MIDI_JACK_EMBEDDED,
        .bJackId            = 0x03,
        .bNrInputPins       = 0x01,
        .baSourceId         = {0x02},
        .baSourcePin        = {0x01},
        .iJack              = 0x00,
    },

    .MIDI_OUT_JACK_4 = {
        .bLength            = MIDI_OUT_JACK_DESCRIPTOR_SIZE(1),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_CS_INTERFACE,
        .SubType            = MIDI_OUT_JACK,
        .bJackType          = MIDI_JACK_EXTERNAL,
//        .bJackId            = 0x04,
        .bJackId            = 0x03,
        .bNrInputPins       = 0x01,
        .baSourceId         = {0x01},
        .baSourcePin        = {0x01},
        .iJack              = 0x00,
    },

    .DataOutEndpoint = {
        .bLength            = sizeof(usb_descriptor_endpoint),
        .bDescriptorType    = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress   = (USB_DESCRIPTOR_ENDPOINT_OUT |
                             MIDI_ENDPOINT_RX), // PATCH
        .bmAttributes       = USB_EP_TYPE_BULK,
        .wMaxPacketSize     = USB_MIDI_RX_EPSIZE,
        .bInterval          = 0x00,
    },

    .MS_CS_DataOutEndpoint = {
      .bLength              = MS_CS_BULK_ENDPOINT_DESCRIPTOR_SIZE(1),
      .bDescriptorType      = USB_DESCRIPTOR_TYPE_CS_ENDPOINT,
      .SubType              = 0x01,
      .bNumEmbMIDIJack      = 0x01,
      .baAssocJackID        = {0x01},
	},

    .DataInEndpoint = {
        .bLength          = sizeof(usb_descriptor_endpoint),
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_IN | MIDI_ENDPOINT_TX), // PATCH
        .bmAttributes     = USB_EP_TYPE_BULK,
        .wMaxPacketSize   = USB_MIDI_TX_EPSIZE,
        .bInterval        = 0x00,
    },

    .MS_CS_DataInEndpoint = {
      .bLength              = MS_CS_BULK_ENDPOINT_DESCRIPTOR_SIZE(1),
      .bDescriptorType      = USB_DESCRIPTOR_TYPE_CS_ENDPOINT,
      .SubType              = 0x01,
      .bNumEmbMIDIJack      = 0x01,
      .baAssocJackID        = {0x03},
	},

};

/* I/O state */

/* Received data */
static volatile uint32 midiBufferRx[USB_MIDI_RX_EPSIZE/4];
/* Read index into midiBufferRx */
static volatile uint32 rx_offset = 0;
/* Transmit data */
static volatile uint32 midiBufferTx[USB_MIDI_TX_EPSIZE/4];
/* Write index into midiBufferTx */
static volatile uint32 tx_offset = 0;
/* Number of bytes left to transmit */
static volatile uint32 n_unsent_packets = 0;
/* Are we currently sending an IN packet? */
static volatile uint8 transmitting = 0;
/* Number of unread bytes */
static volatile uint32 n_unread_packets = 0;


// eventually all of this should be in a place for settings which can be written to flash.
volatile uint8 myMidiChannel = DEFAULT_MIDI_CHANNEL;
volatile uint8 myMidiDevice = DEFAULT_MIDI_DEVICE;
volatile uint8 myMidiCable = DEFAULT_MIDI_CABLE;
volatile uint8 myMidiID[] = { LEAFLABS_MMA_VENDOR_1,LEAFLABS_MMA_VENDOR_2,LEAFLABS_MMA_VENDOR_3,0};

#define OUT_BYTE(s,v) out[(uint8*)&(s.v)-(uint8*)&s]

static void getMIDIPartDescriptor(uint8* out) {
    memcpy(out, &usbMIDIDescriptor_Config, sizeof(usbMIDIDescriptor_Config));
    // patch to reflect where the part goes in the descriptor
    OUT_BYTE(usbMIDIDescriptor_Config, AC_Interface.bInterfaceNumber) += usbMIDIPart.startInterface;
    OUT_BYTE(usbMIDIDescriptor_Config, MS_Interface.bInterfaceNumber) += usbMIDIPart.startInterface;
    OUT_BYTE(usbMIDIDescriptor_Config, AC_CS_Interface.baInterfaceNr) += usbMIDIPart.startInterface;
    OUT_BYTE(usbMIDIDescriptor_Config, DataOutEndpoint.bEndpointAddress) += usbMIDIPart.startEndpoint;
    OUT_BYTE(usbMIDIDescriptor_Config, DataInEndpoint.bEndpointAddress) += usbMIDIPart.startEndpoint;
}

static USBEndpointInfo midiEndpoints[2] = {
    {
        .callback = midiDataRxCb,
        .bufferSize = USB_MIDI_RX_EPSIZE,
        .type = USB_EP_EP_TYPE_BULK, 
        .tx = 0
    },
    {
        .callback = midiDataTxCb,
        .bufferSize = USB_MIDI_TX_EPSIZE,
        .type = USB_EP_EP_TYPE_BULK, 
        .tx = 1,
    }
};

USBCompositePart usbMIDIPart = {
    .numInterfaces = 2,
    .numEndpoints = sizeof(midiEndpoints)/sizeof(*midiEndpoints),
    .descriptorSize = sizeof(usbMIDIDescriptor_Config),
    .getPartDescriptor = getMIDIPartDescriptor,
    .usbInit = NULL,
    .usbReset = usbMIDIReset,
    .usbDataSetup = usbMIDIDataSetup,
    .usbNoDataSetup = usbMIDINoDataSetup,
    .endpoints = midiEndpoints
};

/*
 * MIDI interface
 */

/* This function is non-blocking.
 *
 * It copies data from a usercode buffer into the USB peripheral TX
 * buffer, and returns the number of bytes copied. */
uint32 usb_midi_tx(const uint32* buf, uint32 packets) {
    uint32 bytes=packets*4;
    /* Last transmission hasn't finished, so abort. */
    if (usb_midi_is_transmitting()) {
		/* Copy to TxBuffer */
		
        return 0;  /* return len */
    }

    /* We can only put USB_MIDI_TX_EPSIZE bytes in the buffer. */
    if (bytes > USB_MIDI_TX_EPSIZE) {
        bytes = USB_MIDI_TX_EPSIZE;
        packets=bytes/4;
    }

    /* Queue bytes for sending. */
    if (packets) {
        usb_copy_to_pma((uint8 *)buf, bytes, USB_MIDI_TX_ADDR);
    }
    // We still need to wait for the interrupt, even if we're sending
    // zero bytes. (Sending zero-size packets is useful for flushing
    // host-side buffers.)
    usb_set_ep_tx_count(USB_MIDI_TX_ENDP, bytes);
    n_unsent_packets = packets;
    transmitting = 1;
    usb_set_ep_tx_stat(USB_MIDI_TX_ENDP, USB_EP_STAT_TX_VALID);

    return packets;
}

uint32 usb_midi_data_available(void) {
    return n_unread_packets;
}

uint8 usb_midi_is_transmitting(void) {
    return transmitting;
}

uint16 usb_midi_get_pending(void) {
    return n_unsent_packets;
}

/* Nonblocking byte receive.
 *
 * Copies up to len bytes from our private data buffer (*NOT* the PMA)
 * into buf and deq's the FIFO. */
uint32 usb_midi_rx(uint32* buf, uint32 packets) {
    /* Copy bytes to buffer. */
    uint32 n_copied = usb_midi_peek(buf, packets);

    /* Mark bytes as read. */
    n_unread_packets -= n_copied;
    rx_offset += n_copied;

    /* If all bytes have been read, re-enable the RX endpoint, which
     * was set to NAK when the current batch of bytes was received. */
    if (n_unread_packets == 0) {
        usb_set_ep_rx_count(USB_MIDI_RX_ENDP, USB_MIDI_RX_EPSIZE);
        usb_set_ep_rx_stat(USB_MIDI_RX_ENDP, USB_EP_STAT_RX_VALID);
        rx_offset = 0;
    }

    return n_copied;
}

/* Nonblocking byte lookahead.
 *
 * Looks at unread bytes without marking them as read. */
uint32 usb_midi_peek(uint32* buf, uint32 packets) {
    uint32 i;
    if (packets > n_unread_packets) {
        packets = n_unread_packets;
    }

    for (i = 0; i < packets; i++) {
        buf[i] = midiBufferRx[i + rx_offset];
    }

    return packets;
}

/*
 * Callbacks
 */

static void midiDataTxCb(void) {
    n_unsent_packets = 0;
    transmitting = 0;
}

static void midiDataRxCb(void) {
    usb_set_ep_rx_stat(USB_MIDI_RX_ENDP, USB_EP_STAT_RX_NAK);
    n_unread_packets = usb_get_ep_rx_count(USB_MIDI_RX_ENDP) / 4;
    /* This copy won't overwrite unread bytes, since we've set the RX
     * endpoint to NAK, and will only set it to VALID when all bytes
     * have been read. */
    
    usb_copy_from_pma((uint8*)midiBufferRx, n_unread_packets * 4,
                      USB_MIDI_RX_ADDR);
    
    // discard volatile
    LglSysexHandler((uint32*)midiBufferRx,(uint32*)&rx_offset,(uint32*)&n_unread_packets);
    
    if (n_unread_packets == 0) {
        usb_set_ep_rx_count(USB_MIDI_RX_ENDP, USB_MIDI_RX_EPSIZE);
        usb_set_ep_rx_stat(USB_MIDI_RX_ENDP, USB_EP_STAT_RX_VALID);
        rx_offset = 0;
    }

}

static void usbMIDIReset(void) {
    /* Reset the RX/TX state */
    n_unread_packets = 0;
    n_unsent_packets = 0;
    rx_offset = 0;
}

static RESULT usbMIDIDataSetup(uint8 request) {
    (void)request;//unused
#if 0
    uint8* (*CopyRoutine)(uint16) = 0;

    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
    }

    if (CopyRoutine == NULL) {
        return USB_UNSUPPORT;
    }

    pInformation->Ctrl_Info.CopyData = CopyRoutine;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    (*CopyRoutine)(0);
    return USB_SUCCESS;
#endif
    return USB_UNSUPPORT;
}

static RESULT usbMIDINoDataSetup(uint8 request) {
    (void)request;//unused
#if 0    
    RESULT ret = USB_UNSUPPORT;

    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) {
    }
    return ret;
#endif    
    return USB_UNSUPPORT;
}

// .............THIS IS NOT WORKING YET................
// send debugging information to 
static uint8_t sysexbuffer[80]={CIN_SYSEX,0xF0,0x7D,0x33,CIN_SYSEX,0x33,0x00,0xf7}; // !!!bad hardcoded number foo !!!
uint8_t iSysHexLine(uint8_t rectype, uint16_t address, uint8_t *payload,uint8_t payloadlength, uint8_t *buffer);
void sendThroughSysex(char *printbuffer, int bufferlength) {
    int n;
    n = iSysHexLine(1, 0 , (uint8_t *) printbuffer, (uint8_t) bufferlength , sysexbuffer+6);
    usb_midi_tx((uint32*)sysexbuffer,n/4);
}

#define HIGHBYTE(x) ((uint8_t) (((x) >> 8) &  0x00ff) )
#define LOWBYTE(x) ((uint8_t) ((x) & 0x00ff) )
#define HIGHNIBBLE(x) ((((uint8_t)(x)) & 0xF0) >> 4)
#define LOWNIBBLE(x) (((uint8_t)(x)) & 0x0F)
#define HEXCHAR(c) ((c>9)?55+c:48+c)


uint8_t iSysHexLine(uint8_t rectype, uint16_t address, uint8_t *payload,uint8_t payloadlength, uint8_t *buffer) {
    
    int i=0; int j; int thirdone;
    uint8_t n=0;
    uint16_t checksum=0;
    //uint16_t length=0;
    
    buffer[i++]=':';
    
    checksum+=payloadlength;
    buffer[i++]=HEXCHAR(HIGHNIBBLE(payloadlength));
    buffer[i++]=HEXCHAR(LOWNIBBLE(payloadlength));
    buffer[i++]=CIN_SYSEX;
    
    n=HIGHBYTE(address);
    checksum+=n;
    buffer[i++]=HEXCHAR(HIGHNIBBLE(n));
    buffer[i++]=HEXCHAR(LOWNIBBLE(n));
    
    n=LOWBYTE(address);
    checksum+=n;
    buffer[i++]=HEXCHAR(HIGHNIBBLE(n));
    buffer[i++]=CIN_SYSEX;
    buffer[i++]=HEXCHAR(LOWNIBBLE(n));
    
    n=rectype;
    checksum+=n;
    buffer[i++]=HEXCHAR(HIGHNIBBLE(n));
    buffer[i++]=HEXCHAR(LOWNIBBLE(n));
    buffer[i++]=CIN_SYSEX;
    thirdone=0;
    for (j=0; j<payloadlength ; j++) {
        n=payload[j];
        checksum+=n;
        buffer[i++]=HEXCHAR(HIGHNIBBLE(n));
        if (++thirdone==3) {
            buffer[i++]=CIN_SYSEX;
            thirdone=0;
        }
        buffer[i++]=HEXCHAR(LOWNIBBLE(n));
        if (++thirdone==3) {
            buffer[i++]=CIN_SYSEX;
            thirdone=0;
        }
    }
    if (thirdone==0) {
        buffer[i-1]=CIN_SYSEX_ENDS_IN_3;
    } 
    n=~((uint8_t) checksum&0x00ff)+1;
    buffer[i++]=HEXCHAR(HIGHNIBBLE(n));
    if (thirdone==1) {
        buffer[i++]=CIN_SYSEX_ENDS_IN_2;
    }
    buffer[i++]=HEXCHAR(LOWNIBBLE(n));
    if (thirdone==2) {
        buffer[i++]=CIN_SYSEX_ENDS_IN_1;
    }
    buffer[i++]=0xf7;
    return i+thirdone;
}
