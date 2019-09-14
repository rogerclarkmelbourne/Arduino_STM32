#ifndef _USBCOMPOSITE_H_
#define _USBCOMPOSITE_H_

#include <boards.h>
#include "Stream.h"
#include "usb_generic.h"
//#include <libmaple/usb.h>

#include <USBCompositeSerial.h>
#include <USBHID.h>
#include <USBXBox360.h>
#include <USBMassStorage.h>
#include <USBMIDI.h>

#define USB_MAX_PRODUCT_LENGTH 32
#define USB_MAX_MANUFACTURER_LENGTH 32
#define USB_MAX_SERIAL_NUMBER_LENGTH  20


// You could use this for a serial number, but you'll be revealing the device ID to the host,
// and hence burning it for cryptographic purposes.
const char* getDeviceIDString();

#define USB_COMPOSITE_MAX_PARTS 6

class USBCompositeDevice;

#define DEFAULT_SERIAL_STRING "00000000000000000001"

typedef bool(*USBPartInitializer)(void*);
typedef void(*USBPartStopper)(void*);

class USBCompositeDevice {
private:
	bool enabled = false;
    bool haveSerialNumber = false;
    uint8_t iManufacturer[USB_DESCRIPTOR_STRING_LEN(USB_MAX_MANUFACTURER_LENGTH)];
    uint8_t iProduct[USB_DESCRIPTOR_STRING_LEN(USB_MAX_PRODUCT_LENGTH)];
    uint8_t iSerialNumber[USB_DESCRIPTOR_STRING_LEN(USB_MAX_SERIAL_NUMBER_LENGTH)]; 
    uint16 vendorId;
    uint16 productId;
    USBCompositePart* parts[USB_COMPOSITE_MAX_PARTS];
	USBPartInitializer init[USB_COMPOSITE_MAX_PARTS];
	USBPartStopper stop[USB_COMPOSITE_MAX_PARTS];
	void* plugin[USB_COMPOSITE_MAX_PARTS];
    uint32 numParts;
public:
	USBCompositeDevice(void); 
    void setVendorId(uint16 vendor=0);
    void setProductId(uint16 product=0);
    void setManufacturerString(const char* manufacturer=NULL);
    void setProductString(const char* product=NULL);
    void setSerialString(const char* serialNumber=DEFAULT_SERIAL_STRING);
    bool begin(void);
    void end(void);
    void clear();
    bool isReady() {
        return enabled && usb_is_connected(USBLIB) && usb_is_configured(USBLIB);    
    }
    bool add(USBCompositePart* part, void* plugin, USBPartInitializer init = NULL, USBPartStopper stop = NULL);
};

extern USBCompositeDevice USBComposite;
#endif
        