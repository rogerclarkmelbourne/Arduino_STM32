/* Copyright (c) 2011, Peter Barrett  
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

#ifndef _USBXBox360_H
#define _USBXBox360_H

#include <Print.h>
#include <boards.h>
#include "USBComposite.h"
#include "usb_generic.h"
#include "usb_x360_generic.h"
#include "usb_x360w.h"
#include "usb_multi_x360.h"

typedef struct {
    uint8_t header[4];
    uint8_t reportID;
    uint8_t length;
    uint16_t buttons;
    uint8_t sliderLeft;
    uint8_t sliderRight;
    int16_t x;
    int16_t y;
    int16_t rx;
    int16_t ry;
    uint8 unused[11];
} __packed XBox360WReport_t;

static_assert(sizeof(XBox360WReport_t)==29, "Wrong endianness/packing!");

class USBXBox360Reporter {
protected:
    uint32 controller;
    bool wait(void);
public:
    bool manualReport = false;
    void setController(uint32 c) {
        controller = c;
    }
    void setManualReportMode(bool manualReport);
    bool getManualReportMode();
    bool sendData(const void* data, uint32 length);
    
    USBXBox360Reporter(uint32 _controller=0) {
        controller = _controller;
    }
};

class USBXBox360WController : public USBXBox360Reporter {
private:
    XBox360WReport_t report = {{0},0,19,0,0,0,0,0,0,0,{0}};
	void safeSendReport(void);
    bool connected = false;
public:
    bool connect(bool state);
    bool isConnected(void) {
        return connected; 
    }
	void send(void);
    uint8* getReport(void);
    uint16_t getReportSize(void);
	void stop(void);
	void button(uint8_t button, bool val);
    void buttons(uint16_t b);
	void X(int16_t val);
	void Y(int16_t val);
	void position(int16_t x, int16_t y);
	void positionRight(int16_t x, int16_t y);
	void XRight(int16_t val);
	void YRight(int16_t val);
	void sliderLeft(uint8_t val);
	void sliderRight(uint8_t val);
    void setLEDCallback(void (*callback)(uint8 pattern));
    void setRumbleCallback(void (*callback)(uint8 left, uint8 right));
    
    USBXBox360WController(uint32 _controller=0) : USBXBox360Reporter(_controller) {
    }
};

template<const uint32 numControllers=4>class USBXBox360W {
private:
    bool enabled = false;
    uint8 buffers[USB_X360_BUFFER_SIZE_PER_CONTROLLER * numControllers];
    uint16 vendorId;
    uint16 productId;
    
public:    
    static bool init(USBXBox360W<numControllers>* me) {
        x360w_initialize_controller_data(numControllers, me->buffers);
        USBComposite.setVendorId(me->vendorId);
        USBComposite.setProductId(me->productId);

        return true;
    };

    bool registerComponent() {
        return USBComposite.add(&usbX360Part, this, (USBPartInitializer)&USBXBox360W<numControllers>::init);
    };

    void begin(void){
        if(!enabled){
            USBComposite.clear();
            registerComponent();
            USBComposite.begin();

            enabled = true;
        }
    };

    void end() {
        if (enabled) {
            enabled = false;
            USBComposite.end();
        }
    };    
    
    USBXBox360WController controllers[numControllers];

    USBXBox360W(uint16 _vendorId=0x045e, uint16 _productId=0x0719) : vendorId(_vendorId), productId(_productId) {
        for (uint8 i=0;i<numControllers;i++) controllers[i].setController(i);
    }
};

typedef struct {
    uint8_t reportID;
    uint8_t length;
    uint16_t buttons;
    uint8_t sliderLeft;
    uint8_t sliderRight;
    int16_t x;
    int16_t y;
    int16_t rx;
    int16_t ry;
    uint8 unused[6];
} __packed XBox360Report_t;

static_assert(sizeof(XBox360Report_t)==20, "Wrong endianness/packing!");

class USBXBox360Controller : public USBXBox360Reporter {
private:
    XBox360Report_t report = {0,20,0,0,0,0,0,0,0,{0}};
	void safeSendReport(void);
public:
	void send(void);
    uint8* getReport(void);
    uint16_t getReportSize(void);
	void stop(void);
	void button(uint8_t button, bool val);
    void buttons(uint16_t b);
	void X(int16_t val);
	void Y(int16_t val);
	void position(int16_t x, int16_t y);
	void positionRight(int16_t x, int16_t y);
	void XRight(int16_t val);
	void YRight(int16_t val);
	void sliderLeft(uint8_t val);
	void sliderRight(uint8_t val);
    void setLEDCallback(void (*callback)(uint8 pattern));
    void setRumbleCallback(void (*callback)(uint8 left, uint8 right));
    
    USBXBox360Controller(uint32 _controller=0) : USBXBox360Reporter(_controller) {
    }
};

template<const uint32 numControllers=4>class USBMultiXBox360 {
private:
    bool enabled = false;
    uint8 buffers[USB_X360_BUFFER_SIZE_PER_CONTROLLER * numControllers];
    uint16 vendorId;
    uint16 productId;
    
public:    
    static bool init(USBMultiXBox360<numControllers>* me) {
        usb_multi_x360_initialize_controller_data(numControllers, me->buffers);
        USBComposite.setVendorId(me->vendorId);
        USBComposite.setProductId(me->productId);
        return true;
    };

    bool registerComponent() {
        return USBComposite.add(&usbX360Part, this, (USBPartInitializer)&USBMultiXBox360<numControllers>::init);
    };

    void begin(void){
        if(!enabled){
            USBComposite.clear();
            registerComponent();
            USBComposite.begin();

            enabled = true;
        }
    };

    void end() {
        if (enabled) {
            enabled = false;
            USBComposite.end();
        }
    };    
    
    USBXBox360Controller controllers[numControllers];

    USBMultiXBox360(uint16 _vendorId=0x045e, uint16 _productId=0x028e) : vendorId(_vendorId), productId(_productId) {
        for (uint8 i=0;i<numControllers;i++) controllers[i].setController(i);
    }
};

class USBXBox360 : public USBXBox360Controller {
private:
    bool enabled = false;
    uint8 buffers[USB_X360_BUFFER_SIZE_PER_CONTROLLER];
    uint16 vendorId;
    uint16 productId;
    
public:    
    static bool init(USBXBox360* me) {
        usb_multi_x360_initialize_controller_data(1, me->buffers);
        USBComposite.setVendorId(me->vendorId);
        USBComposite.setProductId(me->productId);
        return true;
    };

    bool registerComponent() {
        return USBComposite.add(&usbX360Part, this, (USBPartInitializer)&USBXBox360::init);
    };

    void begin(void){
        if(!enabled){
            USBComposite.clear();
            registerComponent();
            USBComposite.begin();

            enabled = true;
        }
    };

    void end() {
        if (enabled) {
            enabled = false;
            USBComposite.end();
        }
    };    
    
    USBXBox360(uint16 _vendorId=0x045e, uint16 _productId=0x028e) : USBXBox360Controller(0), vendorId(_vendorId), productId(_productId) {
    }
};

#define XBOX_A 13
#define XBOX_B 14
#define XBOX_X 15
#define XBOX_Y 16
#define XBOX_DUP 1
#define XBOX_DDOWN 2
#define XBOX_DLEFT 3
#define XBOX_DRIGHT 4
#define XBOX_START 5
#define XBOX_BACK 6
#define XBOX_L3 7
#define XBOX_R3 8
#define XBOX_LSHOULDER 9
#define XBOX_RSHOULDER 10
#define XBOX_GUIDE  11

#endif
