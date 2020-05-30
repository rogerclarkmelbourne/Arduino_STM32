/* Copyright (c) 2019, Scott Moreau
**
** Permission to use, copy, modify, and/or distribute this software for
** any purpose with or without fee is hereby granted, provided that the
** above copyright notice and this permission notice appear in all copies.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
** SOFTWARE.
*/

/* Private headers */
#include "usb_lib_globals.h"
#include "usb_reg_map.h"
#include "libmaple/usb.h"
#include <string.h>
#include <libmaple/gpio.h>

#include "usb_audio.h"



#define CUR                        0x01
#define RANGE                      0x02
#define CLOCK_SOURCE_ID            0x10
#define AUDIO_INTERFACE_OFFSET     0x00
#define IO_BUFFER_SIZE              256
#define IO_BUFFER_SIZE_MASK         (IO_BUFFER_SIZE - 1)
//#define AUDIO_INTERFACE_NUMBER     (AUDIO_INTERFACE_OFFSET + usbAUDIOPart.startInterface)
#define AUDIO_ISO_EP_ENDPOINT_INFO (&usbAUDIOPart.endpoints[0])
#define AUDIO_ISO_EP_ADDRESS       (usbAUDIOPart.endpoints[0].address)
#define AUDIO_ISO_PMA_BUFFER_SIZE  (usbAUDIOPart.endpoints[0].pmaSize / 2)

/* Tx data */
static volatile uint8 audioBufferTx[IO_BUFFER_SIZE];
/* Write index to audioBufferTx */
static volatile uint32 audio_tx_head = 0;
/* Read index from audioBufferTx */
static volatile uint32 audio_tx_tail = 0;
/* Rx data */
static volatile uint8 audioBufferRx[IO_BUFFER_SIZE];
/* Write index to audioBufferRx */
static volatile uint32 audio_rx_head = 0;
/* Read index from audioBufferRx */
static volatile uint32 audio_rx_tail = 0;
static uint8 usbAudioReceiving;

static volatile int8 transmitting;
static uint8  clock_valid = 1;
static uint16 sample_rate;
static uint8  buffer_size;
static uint8  channels;

typedef struct {
    uint16_t wNumSubRanges;
    uint32_t min;
    uint32_t max;
    uint32_t res;
} __packed srr_data;

static srr_data sample_rate_range = {
    .wNumSubRanges = 0x0001,
    .min           = 0x00000000,
    .max           = 0x00000000,
    .res           = 0x00000000,
};

static void audioDataTxCb(void);
static void audioDataRxCb(void);
static void audioUSBReset(void);
static RESULT audioUSBDataSetup(uint8 request, uint8 interface, uint8 requestType, uint8 wValue0, uint8 wValue1, uint16 wIndex, uint16 wLength);
static void (*packet_callback)(uint8) = 0;

/*
 * Descriptor
 */

typedef struct {
    audio_interface_association_descriptor     AUDIO_IAD;
    usb_descriptor_interface                   AUDIO_Interface;
    audio_control_descriptor                   AUDIO_AC;
    input_terminal_descriptor                  AUDIO_Input;
    output_terminal_descriptor                 AUDIO_Output;
    feature_unit_descriptor                    AUDIO_FU;
    audio_streaming_descriptor                 AUDIO_Alternate0;
    audio_streaming_descriptor                 AUDIO_Alternate1;
    audio_stream_audio_class_descriptor        AUDIO_AS_AC;
    audio_format_type_descriptor               AUDIO_Format_Type;
    audio_iso_endpoint_descriptor              AUDIO_Iso_EP;
    audio_iso_ac_endpoint_descriptor           AUDIO_Iso_EP_AC;
} __packed audio_part_config;

typedef struct {
    audio_interface_association_descriptor     AUDIO_IAD;
    usb_descriptor_interface                   AUDIO_Interface;
    audio_control_descriptor_2                 AUDIO_AC;
    audio_clock_source_descriptor              AUDIO_CS;
    input_terminal_descriptor_2                AUDIO_Input;
    output_terminal_descriptor_2               AUDIO_Output;
    feature_unit_descriptor_2                  AUDIO_FU;
    audio_streaming_descriptor                 AUDIO_Alternate0;
    audio_streaming_descriptor                 AUDIO_Alternate1;
    audio_stream_audio_class_descriptor_2      AUDIO_AS_AC;
    audio_format_type_descriptor_2             AUDIO_Format_Type;
    audio_iso_endpoint_descriptor_2            AUDIO_Iso_EP;
    audio_iso_ac_endpoint_descriptor_2         AUDIO_Iso_EP_AC;
} __packed audio_part_config_2;

static const audio_part_config audioPartConfigData = {
    .AUDIO_IAD = {
        .bLength             = sizeof(audio_interface_association_descriptor),
        .bDescriptorType     = USB_INTERFACE_ASSOCIATION_DESCRIPTOR,
        .bFirstInterface     = 0x00,
        .bInterfaceCount     = 0x02,
        .bFunctionClass      = USB_DEVICE_CLASS_AUDIO,
        .bFunctionSubClass   = AUDIO_SUBCLASS_AUDIOCONTROL,
        .bFunctionProtocol   = 0x00,
        .iFunction           = 0x00,
    }, /* 8 */
    .AUDIO_Interface = {
        .bLength             = sizeof(usb_descriptor_interface),
        .bDescriptorType     = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber    = AUDIO_INTERFACE_OFFSET, /* PATCH */
        .bAlternateSetting   = 0x00,
        .bNumEndpoints       = 0x00,
        .bInterfaceClass     = USB_DEVICE_CLASS_AUDIO,
        .bInterfaceSubClass  = AUDIO_SUBCLASS_AUDIOCONTROL,
        .bInterfaceProtocol  = 0x00, /* Common AT Commands */
        .iInterface          = 0x00,
    }, /* 9 */
    .AUDIO_AC = {
        .bLength             = sizeof(audio_control_descriptor),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = 0x01, /* HEADER subtype */
        .bcdADC              = 0x0100, /* revision of class specification 1.00 (0x0100) */
        .wTotalLength        = 0x0027, /* total size of class specific descriptors (0x0027) */
        .bInCollection       = 0x01, /* 1 streaming interface */
        .baInterfaceNr       = 0x01, /* AudioStreaming interface 1 belongs to this AC interface */
    }, /* 9 */
    .AUDIO_Input = {
        .bLength             = sizeof(input_terminal_descriptor),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = 0x02, /* INPUT_TERMINAL subtype */
        .bTerminalID         = 0x01, /* ID of this terminal */
        .wTerminalType       = 0x0201, /* Generic Microphone (0x0201) - PATCH */
        .bAssocTerminal      = 0x00, /* bAssocTerminal: no association */
        .bNrChannels         = 0x02, /* dual channel */
        .wChannelConfig      = 0x0003, /* Stereo (0x0003) */
        .iChannelNames       = 0x00, /* unused */
        .iTerminal           = 0x00, /* unused */
    }, /* 12 */
    .AUDIO_Output = {
        .bLength             = sizeof(output_terminal_descriptor),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = 0x03, /* OUTPUT_TERMINAL subtype */
        .bTerminalID         = 0x02, /* ID of this terminal */
        .wTerminalType       = 0x0101, /* USB streaming (0x0101) */
        .bAssocTerminal      = 0x00, /* unused */
        .bSourceID           = 0x01, /* from input terminal */
        .iTerminal           = 0x00, /* unused */
    }, /* 9 */
    .AUDIO_FU = {
        .bLength             = sizeof(feature_unit_descriptor),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = AUDIO_CONTROL_FEATURE_UNIT, /* FEATURE_UNIT */
        .bUnitID             = 0x03, /* unique ID of this unit within the audio function */
        .bSourceID           = 0x01, /* ID of the terminal to which this FU connected */
        .bControlSize        = 0x01, /* bmaControls are one byte size */
        .bmaControls0        = 0x00, /* controls for master channel (no controls) */
        .bmaControls1        = 0x00, /* controls for channel 1 (no controls) */
        .iFeature            = 0x00, /* string descriptor of this FU, unused */
    }, /* 9 */
    .AUDIO_Alternate0 = {
        .bLength             = sizeof(audio_streaming_descriptor),
        .bDescriptorType     = USB_DESCRIPTOR_TYPE_INTERFACE, /* interface */
        .bInterfaceNumber    = 0x01, /* interface 1 (index of this interface) - PATCH */
        .bAlternateSetting   = 0x00, /* index of this alternate setting */
        .bNumEndpoints       = 0x00, /* 0 endpoints */
        .bInterfaceClass     = USB_DEVICE_CLASS_AUDIO, /* AUDIO */
        .bInterfaceSubclass  = AUDIO_SUBCLASS_AUDIOSTREAMING, /* AUDIO_STREAMING */
        .bInterfaceProtocol  = 0x00, /* unused */
        .iInterface          = 0x00, /* unused */
    }, /* 9 */
    .AUDIO_Alternate1 = {
        .bLength             = sizeof(audio_streaming_descriptor),
        .bDescriptorType     = USB_DESCRIPTOR_TYPE_INTERFACE, /* interface */
        .bInterfaceNumber    = 0x01, /* index of this interface - PATCH */
        .bAlternateSetting   = 0x01, /* index of this alternate setting */
        .bNumEndpoints       = 0x01, /* one endpoint */
        .bInterfaceClass     = USB_DEVICE_CLASS_AUDIO, /* AUDIO */
        .bInterfaceSubclass  = AUDIO_SUBCLASS_AUDIOSTREAMING, /* AUDIO_STREAMING */
        .bInterfaceProtocol  = 0x00, /* unused */
        .iInterface          = 0x00, /* unused */
    }, /* 9 */
    .AUDIO_AS_AC = {
        .bLength             = sizeof(audio_stream_audio_class_descriptor),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = 0x01, /* GENERAL subtype */
        .bTerminalLink       = 0x02, /* uint ID of the output terminal */
        .bDelay              = 0x01, /* interface delay */
        .wFormatTag          = 0x0002, /* PCM8 format (0x0002) */
    }, /* 7 */
    .AUDIO_Format_Type = {
        .bLength             = sizeof(audio_format_type_descriptor),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = AUDIO_STREAMING_FORMAT_TYPE, /* FORMAT_TYPE subtype */
        .bFormatType         = 0x01, /* FORMAT_TYPE_I */
        .bNrChannels         = 0x02, /* dual channel - PATCH */
        .bSubFrameSize       = 0x01, /* one byte per audio subframe */
        .bBitResolution      = 0x08, /* 8 bit per sample */
        .bSamFreqType        = 0x01, /* one frequency supported */
        .tSamFreq0           = 0x00, /* Byte 0 - PATCH */
        .tSamFreq1           = 0x00, /* Byte 1 - PATCH */
        .tSamFreq2           = 0x00, /* Byte 2 - PATCH */
    }, /* 11 */
    .AUDIO_Iso_EP = {
        .bLength             = sizeof(audio_iso_endpoint_descriptor),
        .bDescriptorType     = USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint */
        .bEndpointAddress    = USB_DESCRIPTOR_ENDPOINT_IN, /* IN endpoint 1 - PATCH */
        .bmAttributes        = USB_EP_TYPE_ISO, /* isochronous, not shared */
        .wMaxPacketSize      = 0x0040, /* 64 bytes per packet (0x0040) - PATCH */
        .bInterval           = 0x01, /* one packet per ms frame */
        .bRefresh            = 0x00, /* unused */
        .bSynchAddress       = 0x00, /* unused */
    }, /* 9 */
    .AUDIO_Iso_EP_AC = {
        .bLength             = sizeof(audio_iso_ac_endpoint_descriptor),
        .bDescriptorType     = AUDIO_ENDPOINT_DESCRIPTOR_TYPE, /* class-specific endpoint */
        .bDescriptorSubtype  = 0x01, /* GENERAL subtype */
        .bmAttributes        = 0x00, /* no sampling control, no pitch control, no packet padding */
        .bLockDelayUnits     = 0x00, /* unused */
        .wLockDelay          = 0x0000, /* unused (0x0000) */
    } /* 7 */
};

static const audio_part_config_2 audioPartConfigData2 = {
    .AUDIO_IAD = {
        .bLength             = sizeof(audio_interface_association_descriptor),
        .bDescriptorType     = USB_INTERFACE_ASSOCIATION_DESCRIPTOR,
        .bFirstInterface     = 0x00,
        .bInterfaceCount     = 0x02,
        .bFunctionClass      = USB_DEVICE_CLASS_AUDIO,
        .bFunctionSubClass   = AUDIO_SUBCLASS_AUDIOCONTROL,
        .bFunctionProtocol   = 0x20, /* 2.0 AF_VERSION_02_00 */
        .iFunction           = 0x00,
    }, /* 8 */
    .AUDIO_Interface = {
        .bLength             = sizeof(usb_descriptor_interface),
        .bDescriptorType     = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber    = AUDIO_INTERFACE_OFFSET, /* PATCH */
        .bAlternateSetting   = 0x00,
        .bNumEndpoints       = 0x00,
        .bInterfaceClass     = USB_DEVICE_CLASS_AUDIO,
        .bInterfaceSubClass  = AUDIO_SUBCLASS_AUDIOCONTROL,
        .bInterfaceProtocol  = 0x20, /* IP 2.0 IP_VERSION_02_00 */
        .iInterface          = 0x00, /* Not Requested */
    }, /* 9 */
    .AUDIO_AC = {
        .bLength             = sizeof(audio_control_descriptor_2),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = 0x01, /* HEADER subtype */
        .bcdADC              = 0x0200, /* 2.0 */
        .bCategory           = 0x00,
        .wTotalLength        = 0x003C, /* total size of class specific descriptors */
        .bmControls          = 0x01, /* 1 streaming interface */
    }, /* 9 */
    .AUDIO_CS = {
        .bLength             = sizeof(audio_clock_source_descriptor),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = 0x0A, /* CLOCK_SOURCE */
        .bClockID            = CLOCK_SOURCE_ID, /* CLOCK_SOURCE_ID */
        .bmAttributes        = 0x01, /* internal fixed clock */
        .bmControls          = 0x05, /* D3..2: Clock Validity Control   D1..0: Clock Frequency Control */
        .bAssocTerminal      = 0x20,
        .iClockSource        = 0x00, /* Not requested */
    }, /* 8 */
    .AUDIO_Input = {
        .bLength             = sizeof(input_terminal_descriptor_2),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = 0x02, /* INPUT_TERMINAL subtype */
        .bTerminalID         = 0x20, /* ID of this terminal */
        .wTerminalType       = 0x0201, /* Generic Microphone (0x0201) - PATCH */
        .bAssocTerminal      = 0x00, /* bAssocTerminal: no association */
        .bCSourceID          = CLOCK_SOURCE_ID, /* CLOCK_SOURCE_ID */
        .bNrChannels         = 0x01, /* single channel */
        .bmChannelConfig     = 0x00000000, /* Mono, no spatial location */
        .iChannelNames       = 0x00,
        .bmControls          = 0x0000,
        .iTerminal           = 0x00, /* Not requested */
    }, /* 17 */
    .AUDIO_Output = {
        .bLength             = sizeof(output_terminal_descriptor_2),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = 0x03, /* OUTPUT_TERMINAL subtype */
        .bTerminalID         = 0x40, /* ID of this terminal */
        .wTerminalType       = 0x0101, /* USB streaming (0x0101) */
        .bAssocTerminal      = 0x00, /* unused */
        .bSourceID           = 0x30, /* from input terminal */
        .bCSourceID          = CLOCK_SOURCE_ID, /* CLOCK_SOURCE_ID */
        .bmControls          = 0x0000,
        .iTerminal           = 0x00, /* unused */
    }, /* 12 */
    .AUDIO_FU = {
        .bLength             = sizeof(feature_unit_descriptor_2),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = AUDIO_CONTROL_FEATURE_UNIT, /* FEATURE_UNIT */
        .bUnitID             = 0x30, /* unique ID of this unit within the audio function */
        .bSourceID           = 0x20, /* ID of the terminal to which this FU connected */
        .bmaControls0        = 0x00000000, /* controls for master channel (no controls) */
        .bmaControls1        = 0x00000000, /* controls for channel 1 (no controls) */
        .iFeature            = 0x00, /* string descriptor of this FU, unused */
    }, /* 14 */
    .AUDIO_Alternate0 = {
        .bLength             = sizeof(audio_streaming_descriptor),
        .bDescriptorType     = USB_DESCRIPTOR_TYPE_INTERFACE, /* interface */
        .bInterfaceNumber    = 0x01, /* interface 1 (index of this interface) - PATCH */
        .bAlternateSetting   = 0x00, /* index of this alternate setting */
        .bNumEndpoints       = 0x00, /* 0 endpoints */
        .bInterfaceClass     = USB_DEVICE_CLASS_AUDIO, /* AUDIO */
        .bInterfaceSubclass  = AUDIO_SUBCLASS_AUDIOSTREAMING, /* AUDIO_STREAMING */
        .bInterfaceProtocol  = 0x20, /* IP 2.0 */
        .iInterface          = 0x00,
    }, /* 9 */
    .AUDIO_Alternate1 = {
        .bLength             = sizeof(audio_streaming_descriptor),
        .bDescriptorType     = USB_DESCRIPTOR_TYPE_INTERFACE, /* interface */
        .bInterfaceNumber    = 0x01, /* index of this interface - PATCH */
        .bAlternateSetting   = 0x01, /* index of this alternate setting */
        .bNumEndpoints       = 0x01, /* one endpoint */
        .bInterfaceClass     = USB_DEVICE_CLASS_AUDIO, /* AUDIO */
        .bInterfaceSubclass  = AUDIO_SUBCLASS_AUDIOSTREAMING, /* AUDIO_STREAMING */
        .bInterfaceProtocol  = 0x20, /* IP 2.0 */
        .iInterface          = 0x00,
    }, /* 9 */
    .AUDIO_AS_AC = {
        .bLength             = sizeof(audio_stream_audio_class_descriptor_2),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = 0x01, /* GENERAL subtype */
        .bTerminalLink       = 0x40, /* uint ID of the output terminal */
        .bmControls          = 0x00, /* interface delay */
        .bFormatType         = 0x01, /* FORMAT_TYPE_I */
        .bmFormats           = 0x00000002, /* PCM8 */
        .bNrChannels         = 0x02,
        .bmChannelConfig     = 0x00000003,
        .iChannelNames       = 0x00, /* None */
    }, /* 16 */
    .AUDIO_Format_Type = {
        .bLength             = sizeof(audio_format_type_descriptor_2),
        .bDescriptorType     = AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* class-specific interface */
        .bDescriptorSubtype  = AUDIO_STREAMING_FORMAT_TYPE, /* FORMAT_TYPE subtype */
        .bFormatType         = 0x01, /* FORMAT_TYPE_I */
        .bSubSlotSize        = 0x01, /* single channel */
        .bBitResolution      = 0x08, /* one byte per audio subframe */
    }, /* 6 */
    .AUDIO_Iso_EP = {
        .bLength             = sizeof(audio_iso_endpoint_descriptor_2),
        .bDescriptorType     = USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint */
        .bEndpointAddress    = USB_DESCRIPTOR_ENDPOINT_IN, /* IN endpoint 1 - PATCH */
        .bmAttributes        = USB_EP_TYPE_ISO, /* isochronous - PATCH */
        .wMaxPacketSize      = 0x0040, /* 64 bytes per packet (0x0040) - PATCH */
        .bInterval           = 0x01, /* one packet per ms frame */
    }, /* 7 */
    .AUDIO_Iso_EP_AC = {
        .bLength             = sizeof(audio_iso_ac_endpoint_descriptor_2),
        .bDescriptorType     = AUDIO_ENDPOINT_DESCRIPTOR_TYPE, /* class-specific endpoint */
        .bDescriptorSubtype  = 0x01, /* GENERAL subtype */
        .bmAttributes        = 0x00, /* MaxPacketsOnly = FALSE */
        .bmControls          = 0x00,
        .bLockDelayUnits     = 0x00,
        .wLockDelay          = 0x0000,
    } /* 8 */
};

static USBEndpointInfo audioEndpointIN[1] = {
    {
        .callback = audioDataTxCb,
        .pmaSize = AUDIO_MAX_EP_BUFFER_SIZE,
        .type = USB_GENERIC_ENDPOINT_TYPE_ISO,
        .tx = 1,
        .exclusive = 1, // TODO: check if needed?
		.doubleBuffer = 1
    }
};

static USBEndpointInfo audioEndpointOUT[1] = {
    {
        .callback = audioDataRxCb,
        .pmaSize = AUDIO_MAX_EP_BUFFER_SIZE,
        .type = USB_GENERIC_ENDPOINT_TYPE_ISO,
        .tx = 0,
        .exclusive = 1, // TODO: check if needed?
		.doubleBuffer = 1
    }
};

void usb_audio_setEPSize(uint32_t size) {
    if (size == 0 || size > buffer_size)
        size = buffer_size;

    /* only IN is double buffered */
    if (usbAUDIOPart.endpoints == audioEndpointIN)
        size *= 2;

    usbAUDIOPart.endpoints[0].pmaSize = size;
}

#define OUT_BYTE(s,v) out[(uint8*)&(s.v)-(uint8*)&s]
#define OUT_16(s,v) *(uint16_t*)&OUT_BYTE(s,v)
#define OUT_32(s,v) *(uint32_t*)&OUT_BYTE(s,v)

static void getAUDIOPartDescriptor(uint8* out) {
    memcpy(out, &audioPartConfigData, sizeof(audio_part_config));
    /* patch to reflect where the part goes in the descriptor */
    OUT_BYTE(audioPartConfigData, AUDIO_Interface.bInterfaceNumber) += usbAUDIOPart.startInterface;
    if (usbAUDIOPart.endpoints == audioEndpointOUT) {
        OUT_16(audioPartConfigData, AUDIO_Input.wTerminalType) = 0x0301 /* Generic Speaker */;
        OUT_BYTE(audioPartConfigData, AUDIO_Iso_EP.bEndpointAddress) = USB_DESCRIPTOR_ENDPOINT_OUT;
    }
    OUT_BYTE(audioPartConfigData, AUDIO_Alternate0.bInterfaceNumber) += usbAUDIOPart.startInterface;
    OUT_BYTE(audioPartConfigData, AUDIO_Alternate1.bInterfaceNumber) += usbAUDIOPart.startInterface;
    OUT_BYTE(audioPartConfigData, AUDIO_Iso_EP.bEndpointAddress) += AUDIO_ISO_EP_ADDRESS;
    OUT_BYTE(audioPartConfigData, AUDIO_Format_Type.bNrChannels) = channels;
    OUT_BYTE(audioPartConfigData, AUDIO_Format_Type.tSamFreq0) = AUDIO_SAMPLE_FREQ_0(sample_rate);
    OUT_BYTE(audioPartConfigData, AUDIO_Format_Type.tSamFreq1) = AUDIO_SAMPLE_FREQ_1(sample_rate);
    OUT_BYTE(audioPartConfigData, AUDIO_Format_Type.tSamFreq2) = AUDIO_SAMPLE_FREQ_2(sample_rate);
    OUT_BYTE(audioPartConfigData, AUDIO_Iso_EP.bmAttributes) |= 0x0C; /* synchronous */
    /* Used in conjunction with other attributes for bandwidth allocation calculation */
    OUT_16(audioPartConfigData, AUDIO_Iso_EP.wMaxPacketSize) = buffer_size;
}

static void getAUDIOPartDescriptor2(uint8* out) {
    memcpy(out, &audioPartConfigData2, sizeof(audio_part_config_2));
    /* patch to reflect where the part goes in the descriptor */
    OUT_BYTE(audioPartConfigData2, AUDIO_Interface.bInterfaceNumber) += usbAUDIOPart.startInterface;
    if (usbAUDIOPart.endpoints == audioEndpointOUT) {
        OUT_16(audioPartConfigData2, AUDIO_Input.wTerminalType) = 0x0301 /* Generic Speaker */;
        OUT_BYTE(audioPartConfigData2, AUDIO_Iso_EP.bEndpointAddress) = USB_DESCRIPTOR_ENDPOINT_OUT;
    }
    OUT_BYTE(audioPartConfigData2, AUDIO_Input.bNrChannels) = channels;
    if (channels == 2)
        OUT_32(audioPartConfigData2, AUDIO_Input.bmChannelConfig) = 0x00000003; /* Front Left, Front Right */
    OUT_BYTE(audioPartConfigData2, AUDIO_Alternate0.bInterfaceNumber) += usbAUDIOPart.startInterface;
    OUT_BYTE(audioPartConfigData2, AUDIO_Alternate1.bInterfaceNumber) += usbAUDIOPart.startInterface;
    OUT_BYTE(audioPartConfigData2, AUDIO_Iso_EP.bEndpointAddress) += AUDIO_ISO_EP_ADDRESS;
    OUT_BYTE(audioPartConfigData2, AUDIO_AS_AC.bNrChannels) = channels;
    OUT_BYTE(audioPartConfigData2, AUDIO_Iso_EP.bmAttributes) |= 0x0C; /* synchronous */
    /* Used in conjunction with other attributes for bandwidth allocation calculation */
    OUT_16(audioPartConfigData2, AUDIO_Iso_EP.wMaxPacketSize) = buffer_size;
}

USBCompositePart usbAUDIOPart = {
    .numInterfaces = 2,
    .numEndpoints = 1,
    .usbInit = NULL,
    .usbReset = audioUSBReset,
    .usbDataSetup = audioUSBDataSetup,
    .usbNoDataSetup = NULL,
    .usbClearFeature = NULL,
    .usbSetConfiguration = NULL
};


uint8 usb_audio_init(uint16 type, uint16 rate)
{
    channels = 1;

    if ((type & 0xFF) == MIC_MONO) {
        usbAUDIOPart.endpoints = audioEndpointIN;
    } else if ((type & 0xFF) == MIC_STEREO) {
        usbAUDIOPart.endpoints = audioEndpointIN;
        channels = 2;
    } else if ((type & 0xFF) == SPEAKER_MONO) {
        usbAUDIOPart.endpoints = audioEndpointOUT;
    } else if ((type & 0xFF) == SPEAKER_STEREO) {
        usbAUDIOPart.endpoints = audioEndpointOUT;
        channels = 2;
    }
    if ((type & 0xFF00) == AUDIO_CLASS_2) {
        usbAUDIOPart.descriptorSize = sizeof(audio_part_config_2);
        usbAUDIOPart.getPartDescriptor = getAUDIOPartDescriptor2;
    } else if (((type & 0xFF00) == AUDIO_CLASS_1) || !(type & 0xFF00)) {
        usbAUDIOPart.descriptorSize = sizeof(audio_part_config);
        usbAUDIOPart.getPartDescriptor = getAUDIOPartDescriptor;
    } else {
        return 0;
    }

    buffer_size = (rate / 1000) * channels;

    sample_rate = rate;

    sample_rate_range.wNumSubRanges = 0x0001;
    sample_rate_range.min           = sample_rate;
    sample_rate_range.max           = sample_rate;
    sample_rate_range.res           = 0x00000000;

    return buffer_size;
}

void audio_set_packet_callback(void (*callback)(uint8)) {
        packet_callback = callback;
}

/* This function is non-blocking.
 *
 * It copies data from a user buffer into the USB peripheral TX
 * buffer, and returns the number of bytes copied. */
uint32 usb_audio_write_tx_data(const uint8* buf, uint32 len)
{
    if (len == 0)
        return 0; /* no data to send */

    while(transmitting >= 0);

    uint32 head = audio_tx_head; /* load volatile variable */
    uint32 tx_unsent = (head - audio_tx_tail) & IO_BUFFER_SIZE_MASK;

    /* We can only put bytes in the buffer if there is place */
    if (len > (IO_BUFFER_SIZE - tx_unsent - 1) ) {
        len = (IO_BUFFER_SIZE - tx_unsent - 1);
    }

    if (len == 0)
        return 0; /* buffer full */

    /* copy data from user buffer to USB Tx buffer */
    uint16 i;
    for (i = 0; i < len; i++) {
        audioBufferTx[head] = buf[i];
        head = (head + 1) & IO_BUFFER_SIZE_MASK;
    }
    audio_tx_head = head; /* store volatile variable */

    return len;
}

/* Non-blocking byte lookahead.
 *
 * Looks at unread bytes without marking them as read. */
uint32 audio_rx_peek(uint8* buf, uint32 len)
{
    unsigned i;
    uint32 tail = audio_rx_tail;
    uint32 rx_unread = (audio_rx_head - tail) & IO_BUFFER_SIZE_MASK;

    if (len > rx_unread)
        len = rx_unread;

    for (i = 0; i < len; i++) {
        buf[i] = audioBufferRx[tail];
        tail = (tail + 1) & IO_BUFFER_SIZE_MASK;
    }

    return len;
}

/* Non-blocking byte receive.
 *
 * Copies up to len bytes from our private data buffer (*NOT* the PMA)
 * into buf and deq's the FIFO. */
uint32 usb_audio_read_rx_data(uint8* buf, uint32 len)
{
    while(usbAudioReceiving);

    /* Copy bytes to buffer. */
    uint32 n_copied = audio_rx_peek(buf, len);

    /* Mark bytes as read. */
    uint16 tail = audio_rx_tail; /* load volatile variable */
    tail = (tail + n_copied) & IO_BUFFER_SIZE_MASK;
    audio_rx_tail = tail; /* store volatile variable */

    return n_copied;
}

/* Since we're USB FS, this function called once per millisecond */
static void audioDataTxCb(void)
{
    transmitting = 1;
    usb_generic_send_from_circular_buffer_double_buffered(AUDIO_ISO_EP_ENDPOINT_INFO, audioBufferTx, IO_BUFFER_SIZE, buffer_size, &audio_tx_tail);
    transmitting = -1;

    if (packet_callback)
        packet_callback(buffer_size);
}

static void audioDataRxCb(void)
{
    usbAudioReceiving = 1;
    uint32 ep_rx_size = usb_generic_read_to_circular_buffer(usbAUDIOPart.endpoints+0, audioBufferRx, 
                        IO_BUFFER_SIZE, &audio_rx_head);
    usbAudioReceiving = 0;

    if (packet_callback)
        packet_callback(ep_rx_size);
}

static void audioUSBReset(void) {
    /* Reset the RX/TX state */
    audio_tx_head = 0;
    audio_tx_tail = 0;
    audio_rx_head = 0;
    audio_rx_tail = 0;
    usbAudioReceiving = 0;
    transmitting = -1;

    if (usbAUDIOPart.endpoints == audioEndpointIN) {
        /* Setup IN endpoint */
        usb_generic_enable_tx(AUDIO_ISO_EP_ENDPOINT_INFO);
    } else if (usbAUDIOPart.endpoints == audioEndpointOUT) {
        /* Setup OUT endpoint */
        usb_generic_enable_rx(AUDIO_ISO_EP_ENDPOINT_INFO);
    }
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
static RESULT audioUSBDataSetup(uint8 request, uint8 interface, uint8 requestType, uint8 wValue0, uint8 wValue1, uint16 wIndex, uint16 wLength) {
	switch (requestType) {
        case 0x21:
            usb_generic_control_rx_setup(NULL, wLength, NULL);
            return USB_SUCCESS;

        case 0xA1:
            if ((wIndex >> 8) == CLOCK_SOURCE_ID) {
                switch(request) {
                    case CUR:
                        switch(wLength) {
                            case 1:
                                usb_generic_control_tx_setup(&clock_valid, 1, NULL);
                                return USB_SUCCESS;
                            case 2:
                                usb_generic_control_tx_setup(&sample_rate, 2, NULL);
                                return USB_SUCCESS;
                            case 4:
                                usb_generic_control_tx_setup(&sample_rate_range.min, 4, NULL);
                                return USB_SUCCESS;
                        }
                        break;
                    case RANGE:
                        usb_generic_control_tx_setup(&sample_rate_range, sizeof(sample_rate_range), NULL);
                        return USB_SUCCESS;
                }
            }
            usb_generic_control_tx_setup(NULL, wLength, NULL);
            return USB_SUCCESS;
	}

	return USB_UNSUPPORT;
}

