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

#ifndef __USB_AUDIO_H
#define __USB_AUDIO_H

#include <libmaple/libmaple_types.h>
#include <libmaple/usb.h>
#include "usb_generic.h"

#define AUDIO_SAMPLE_FREQ_0(frq)    (uint8_t)((frq >> 16))
#define AUDIO_SAMPLE_FREQ_1(frq)    (uint8_t)((frq >> 8))
#define AUDIO_SAMPLE_FREQ_2(frq)    (uint8_t)(frq)

#define MAX_AUDIO_BUFFERS 8
#define AUDIO_BUFFER_SIZE(n,reportID) ((n)+((reportID)!=0))
#define AUDIO_BUFFER_ALLOCATE_SIZE(n,reportID) ((AUDIO_BUFFER_SIZE((n),(reportID))+1)/2*2)

#define AUDIO_BUFFER_MODE_NO_WAIT 1
#define AUDIO_BUFFER_MODE_OUTPUT  2

#define AUDIO_BUFFER_EMPTY    0
#define AUDIO_BUFFER_UNREAD   1
#define AUDIO_BUFFER_READ     2

#define AUDIO_ENDPOINT_TX 0
#define USB_AUDIO_TX_ENDP (audioEndpoints[AUDIO_ENDPOINT_TX].address)
#define USB_AUDIO_TX_ADDR (audioEndpoints[AUDIO_ENDPOINT_TX].pmaAddress)

/* Descriptor constants */

#define USB_INTERFACE_ASSOCIATION_DESCRIPTOR          0x0B
#define USB_DEVICE_CLASS_AUDIO                        0x01
#define AUDIO_SUBCLASS_AUDIOCONTROL                   0x01
#define AUDIO_SUBCLASS_AUDIOSTREAMING                 0x02
#define AUDIO_STREAMING_FORMAT_TYPE                   0x02

#define AUDIO_INTERFACE_DESCRIPTOR_TYPE               0x24
#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE                0x25

#define AUDIO_CONTROL_FEATURE_UNIT                    0x06

#define AUDIO_FORMAT_TYPE_I                           0x01

#define AUDIO_MAX_EP_BUFFER_SIZE                      128

#define AUDIO_CLASS_1                                 0x0100
#define AUDIO_CLASS_2                                 0x1000
#define MIC_MONO                                      0x01
#define MIC_STEREO                                    0x02
#define SPEAKER_MONO                                  0x03
#define SPEAKER_STEREO                                0x04

extern USBCompositePart usbAUDIOPart;

#ifdef __cplusplus
extern "C" {
#endif

/* Standard Interface Association Descriptor */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bFirstInterface;
    uint8_t  bInterfaceCount;
    uint8_t  bFunctionClass;
    uint8_t  bFunctionSubClass;
    uint8_t  bFunctionProtocol;
    uint8_t  iFunction;
} __packed audio_interface_association_descriptor;

/* Interface Header Audio Class Descriptor v1 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint16_t bcdADC;
    uint16_t wTotalLength;
    uint8_t  bInCollection;
    uint8_t  baInterfaceNr;
} __packed audio_control_descriptor;

/* Interface Header Audio Class Descriptor v2 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint16_t bcdADC;
    uint8_t  bCategory;
    uint16_t wTotalLength;
    uint8_t  bmControls;
} __packed audio_control_descriptor_2;

/* Clock Source Descriptor */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bClockID;
    uint8_t  bmAttributes;
    uint8_t  bmControls;
    uint8_t  bAssocTerminal;
    uint8_t  iClockSource;
} __packed audio_clock_source_descriptor;

/* Input Terminal Audio Class Descriptor v1 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  bNrChannels;
    uint16_t wChannelConfig;
    uint8_t  iChannelNames;
    uint8_t  iTerminal;
} __packed input_terminal_descriptor;

/* Input Terminal Audio Class Descriptor v2 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  bCSourceID;
    uint8_t  bNrChannels;
    uint32_t bmChannelConfig;
    uint8_t  iChannelNames;
    uint16_t bmControls;
    uint8_t  iTerminal;
} __packed input_terminal_descriptor_2;

/* Output Terminal Audio Class Descriptor v1 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  bSourceID;
    uint8_t  iTerminal;
} __packed output_terminal_descriptor;

/* Output Terminal Audio Class Descriptor v2 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  bSourceID;
    uint8_t  bCSourceID;
    uint16_t bmControls;
    uint8_t  iTerminal;
} __packed output_terminal_descriptor_2;

/* Feature Unit Audio Class Descriptor v1 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bUnitID;
    uint8_t  bSourceID;
    uint8_t  bControlSize;
    uint8_t  bmaControls0;
    uint8_t  bmaControls1;
    uint8_t  iFeature;
} __packed feature_unit_descriptor;

/* Feature Unit Audio Class Descriptor v2 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bUnitID;
    uint8_t  bSourceID;
    uint32_t bmaControls0;
    uint32_t bmaControls1;
    uint8_t  iFeature;
} __packed feature_unit_descriptor_2;

/* Alternate Audio Interface Descriptor */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bInterfaceNumber;
    uint8_t  bAlternateSetting;
    uint8_t  bNumEndpoints;
    uint8_t  bInterfaceClass;
    uint8_t  bInterfaceSubclass;
    uint8_t  bInterfaceProtocol;
    uint8_t  iInterface;
} __packed audio_streaming_descriptor;

/* Audio Stream Audio Class Descriptor v1 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalLink;
    uint8_t  bDelay;
    uint16_t wFormatTag;
} __packed audio_stream_audio_class_descriptor;

/* Audio Stream Audio Class Descriptor v2 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalLink;
    uint8_t  bmControls;
    uint8_t  bFormatType;
    uint32_t bmFormats;
    uint8_t  bNrChannels;
    uint32_t bmChannelConfig;
    uint8_t  iChannelNames;
} __packed audio_stream_audio_class_descriptor_2;

/* Format Type Audio Descriptor v1 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bFormatType;
    uint8_t  bNrChannels;
    uint8_t  bSubFrameSize;
    uint8_t  bBitResolution;
    uint8_t  bSamFreqType;
    uint8_t  tSamFreq2;
    uint8_t  tSamFreq1;
    uint8_t  tSamFreq0;
} __packed audio_format_type_descriptor;

/* Format Type Audio Descriptor v2 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bFormatType;
    uint8_t  bSubSlotSize;
    uint8_t  bBitResolution;
} __packed audio_format_type_descriptor_2;

/* Isochronous Endpoint Descriptor v1 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
    uint8_t  bRefresh;
    uint8_t  bSynchAddress;
} __packed audio_iso_endpoint_descriptor;

/* Isochronous Endpoint Descriptor v2 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
} __packed audio_iso_endpoint_descriptor_2;

/* Isochronous Endpoint Audio Class Descriptor v1 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bmAttributes;
    uint8_t  bLockDelayUnits;
    uint16_t wLockDelay;
} __packed audio_iso_ac_endpoint_descriptor;

/* Isochronous Endpoint Audio Class Descriptor v2 */
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bmAttributes;
    uint8_t  bmControls;
    uint8_t  bLockDelayUnits;
    uint16_t wLockDelay;
} __packed audio_iso_ac_endpoint_descriptor_2;


 /*
 * AUDIO interface
 */

uint32 usb_audio_write_tx_data(const uint8* buf, uint32 len);
uint32 usb_audio_read_rx_data(uint8* buf, uint32 len);
uint8 usb_audio_init(uint16 type, uint16 rate);
void usb_audio_setEPSize(uint32_t size);
void audio_set_packet_callback(void (*callback)(uint8));

#ifdef __cplusplus
}
#endif

#endif  /* __USB_AUDIO_H */
