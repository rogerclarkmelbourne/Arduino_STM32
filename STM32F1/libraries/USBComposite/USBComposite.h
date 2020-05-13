#ifndef _USBCOMPOSITE_H_
#define _USBCOMPOSITE_H_

#include <boards.h>
#include "Stream.h"
#include "usb_generic.h"
//#include <libmaple/usb.h>

#define USB_COMPOSITE_MAX_PARTS 6

// You could use this for a serial number, but you'll be revealing the device ID to the host,
// and hence burning it for cryptographic purposes.
char* getDeviceIDString();

class USBCompositeDevice;

#define DEFAULT_SERIAL_STRING "00000000000000000001"

typedef bool(*USBPartInitializer)(void*);
typedef void(*USBPartStopper)(void*);

class USBCompositeDevice {
private:
    const char* iManufacturer = NULL;
    const char* iProduct = NULL;
    const char* iSerialNumber = NULL;
    uint16 vendorId;
    uint16 productId;
    USBCompositePart* parts[USB_COMPOSITE_MAX_PARTS];
    USBPartInitializer init[USB_COMPOSITE_MAX_PARTS];
    USBPartStopper stop[USB_COMPOSITE_MAX_PARTS];
    void* plugin[USB_COMPOSITE_MAX_PARTS];
    uint32 numParts;
    bool enabled = false;
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
    void setDisconnectDelay(uint32 delay=500) { // in microseconds
        usb_generic_set_disconnect_delay(delay);
    }
    bool isReady() {
        return enabled && usb_is_connected(USBLIB) && usb_is_configured(USBLIB);    
    }   
    operator bool() { 
        return isReady(); 
    }
    bool add(USBCompositePart* part, void* plugin, USBPartInitializer init = NULL, USBPartStopper stop = NULL);
};

extern USBCompositeDevice USBComposite;

#include <USBCompositeSerial.h>
#include <USBHID.h>
#include <USBMassStorage.h>
#include <USBMIDI.h>
#include <USBAudio.h>
#include <USBMultiSerial.h>
#include <USBXBox360.h>
#endif
        