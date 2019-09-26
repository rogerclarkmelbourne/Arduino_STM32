#ifndef _USB_GENERIC_H
#define _USB_GENERIC_H
#include <libmaple/libmaple_types.h>
typedef unsigned short u16;
typedef unsigned char u8;
#include <usb_core.h>
#include <libmaple/usb.h>

#define PMA_MEMORY_SIZE 512
#define MAX_USB_DESCRIPTOR_DATA_SIZE 200

#define USB_EP0_BUFFER_SIZE       0x40
#define USB_EP0_TX_BUFFER_ADDRESS 0x40
#define USB_EP0_RX_BUFFER_ADDRESS (USB_EP0_TX_BUFFER_ADDRESS+USB_EP0_BUFFER_SIZE) 

#ifdef __cplusplus
extern "C" {
#endif

extern const usb_descriptor_string usb_generic_default_iManufacturer;
extern const usb_descriptor_string usb_generic_default_iProduct;

typedef struct USBEndpointInfo {
    void (*callback)(void);
    uint16 bufferSize;
    uint16 type; // bulk, interrupt, etc.
    uint8 tx; // 1 if TX, 0 if RX
    uint8 address;    
    uint16 pmaAddress;
} USBEndpointInfo;

typedef struct USBCompositePart {
    uint8 numInterfaces;
    uint8 numEndpoints;
	uint8 startInterface;
	uint8 startEndpoint;
    uint16 descriptorSize;
    void (*getPartDescriptor)(uint8* out);
    void (*usbInit)(void);
    void (*usbReset)(void);
    void (*usbSetConfiguration)(void);
    void (*usbClearFeature)(void);
    RESULT (*usbDataSetup)(uint8 request);
    RESULT (*usbNoDataSetup)(uint8 request);
    USBEndpointInfo* endpoints;
} USBCompositePart;

void usb_generic_set_info(uint16 idVendor, uint16 idProduct, const uint8* iManufacturer, const uint8* iProduct, const uint8* iSerialNumber);
uint8 usb_generic_set_parts(USBCompositePart** _parts, unsigned _numParts);
void usb_generic_disable(void);
void usb_generic_enable(void);
extern volatile int8 usbGenericTransmitting;
void usb_copy_from_pma(uint8 *buf, uint16 len, uint16 pma_offset);
void usb_copy_to_pma(const uint8 *buf, uint16 len, uint16 pma_offset);

#ifdef __cplusplus
}
#endif

#endif
