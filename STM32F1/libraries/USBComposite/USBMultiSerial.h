#ifndef _MULTI_SERIAL_H_
#define _MULTI_SERIAL_H_

#include <USBComposite.h>
#include "usb_multi_serial.h"

#if defined(SERIAL_USB)
void usb_multi_serial_rxHook0(unsigned, void*);
void usb_multi_serial_ifaceSetupHook0(unsigned, void*);
#endif

class USBSerialPort : public Stream {
public:
    uint32 txPacketSize = USB_MULTI_SERIAL_DEFAULT_TX_SIZE;
    uint32 rxPacketSize = USB_MULTI_SERIAL_DEFAULT_RX_SIZE; 
    uint32 port;
    virtual int available(void);// Changed to virtual

    uint32 read(uint8 * buf, uint32 len);
   // uint8  read(void);

    // Roger Clark. added functions to support Arduino 1.0 API
    virtual int peek(void);
    virtual int read(void);
    int availableForWrite(void);
    virtual void flush(void);	
    
    size_t write(uint8);
    size_t write(const char *str);
    size_t write(const uint8*, uint32);

    uint8 getRTS();
    uint8 getDTR();
    uint8 isConnected();
    uint8 pending();
    
    void setRXPacketSize(uint32 size=USB_MULTI_SERIAL_DEFAULT_RX_SIZE) {
        rxPacketSize = size;
    }

    void setTXPacketSize(uint32 size=USB_MULTI_SERIAL_DEFAULT_TX_SIZE) {
        txPacketSize = size;
    }
    
    void setPort(uint8 _port) {
        port = _port;
    }
    
    uint8 getPort(void) {
        return port;
    }    
};

template<const uint32 numPorts=3,const uint32 rxPacketSize=USB_MULTI_SERIAL_DEFAULT_RX_SIZE,const uint32 txPacketSize=USB_MULTI_SERIAL_DEFAULT_TX_SIZE>class USBMultiSerial {
private:
	bool enabled = false;
    uint8 buffers[USB_MULTI_SERIAL_BUFFERS_SIZE(numPorts)];
public:
    bool begin() {
        if (!enabled) {
            USBComposite.clear();
            if (!registerComponent())
                return false;
            USBComposite.begin();
            enabled = true;
        }
        return true;
    };

    void end() {
        if (enabled) {
            USBComposite.end();
            enabled = false;
        }
    }

    bool registerComponent() {
        return USBComposite.add(&usbMultiSerialPart, this, (USBPartInitializer)&USBMultiSerial<numPorts,txPacketSize>::init);
    }

    static bool init(USBMultiSerial<numPorts,rxPacketSize,txPacketSize>* me) {
        for (uint8 i=0; i<numPorts; i++) {
            multi_serial_setTXEPSize(i, txPacketSize);
            multi_serial_setRXEPSize(i, rxPacketSize);
        }
        multi_serial_initialize_port_data(numPorts, me->buffers);
#if defined(SERIAL_USB)
        multi_serial_set_hooks(0, USBHID_CDCACM_HOOK_RX, usb_multi_serial_rxHook0);
        multi_serial_set_hooks(0, USBHID_CDCACM_HOOK_IFACE_SETUP, usb_multi_serial_ifaceSetupHook0);
#endif
        return true;
    };

	operator bool() { return USBComposite.isReady(); } 
    
    USBSerialPort ports[numPorts];

    USBMultiSerial() {
        for (uint32 i=0;i<numPorts;i++) ports[i].setPort(i);
    }
};


#endif
