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

#ifndef _USBHID_H_
#define _USBHID_H_

#include <USBComposite.h>
#include <USBCompositeSerial.h>
#include <Print.h>
#include <boards.h>
#include "Stream.h"
#include "usb_hid.h"

#define USB_HID_MAX_PRODUCT_LENGTH 32
#define USB_HID_MAX_MANUFACTURER_LENGTH 32
#define USB_HID_MAX_SERIAL_NUMBER_LENGTH  20

#define HID_MOUSE_REPORT_ID 1
#define HID_KEYBOARD_REPORT_ID 2
#define HID_CONSUMER_REPORT_ID 3
#define HID_JOYSTICK_REPORT_ID 20

#define HID_KEYBOARD_ROLLOVER 6

#define MACRO_GET_ARGUMENT_2(x, y, ...) y
#define MACRO_GET_ARGUMENT_1_WITH_DEFAULT(default, ...) MACRO_GET_ARGUMENT_2(placeholder, ## __VA_ARGS__, default)
#define MACRO_ARGUMENT_2_TO_END(skip, ...) __VA_ARGS__

// HIDBuffer_t data buffers must have even memory length because of how PMA transfers work
#define HID_DATA_BUFFER_SIZE(n) (((n)+1)/2*2)

/* note that featureSize must be 1 less than the buffer size for the feature,
   since the latter must include the reportId */
/* this only works in a collection with a report_id */
#define HID_FEATURE_REPORT_DESCRIPTOR(dataSize) \
    0x06, 0x00, 0xFF,      /* USAGE_PAGE (Vendor Defined Page 1) */ \
    0x09, 0x01,            /* USAGE (Vendor Usage 1) */ \
    0x15, 0x00,    /* LOGICAL_MINIMUM (0) */  \
    0x26, 0xff, 0x00, /* LOGICAL_MAXIMUM (255) */ \
    0x75, 0x08,       /* REPORT_SIZE (8) */ \
    0x95, dataSize,       /* REPORT_COUNT (xx) */ \
    0xB1, 0x02,     /* FEATURE (Data,Var,Abs) */ \

#define HID_OUTPUT_REPORT_DESCRIPTOR(dataSize) \
    0x06, 0x00, 0xFF,      /* USAGE_PAGE (Vendor Defined Page 1) */ \
    0x09, 0x01,            /* USAGE (Vendor Usage 1) */ \
    0x15, 0x00,    /* LOGICAL_MINIMUM (0) */  \
    0x26, 0xff, 0x00, /* LOGICAL_MAXIMUM (255) */ \
    0x75, 0x08,       /* REPORT_SIZE (8) */ \
    0x95, dataSize,       /* REPORT_COUNT (32) */ \
    0x91, 0x02,     /* OUTPUT (Data,Var,Abs) */ \
    
#define HID_CONSUMER_REPORT_DESCRIPTOR(...) \
    0x05, 0x0C,									/* usage page (consumer device) */ \
	0x09, 0x01, 								/* usage -- consumer control */ \
	0xA1, 0x01, 								/* collection (application) */ \
    0x85, MACRO_GET_ARGUMENT_1_WITH_DEFAULT(HID_CONSUMER_REPORT_ID, ## __VA_ARGS__),  /*    REPORT_ID */ \
	0x15, 0x00, 								/* logical minimum */ \
	0x26, 0xFF, 0x03, 							/* logical maximum (3ff) */ \
	0x19, 0x00, 								/* usage minimum (0) */ \
	0x2A, 0xFF, 0x03, 							/* usage maximum (3ff) */ \
	0x75, 0x10, 								/* report size (16) */ \
	0x95, 0x01, 								/* report count (1) */ \
	0x81, 0x00, 								/* input */ \
    MACRO_ARGUMENT_2_TO_END(__VA_ARGS__)  \
	0xC0 /* end collection */    
    
#define HID_MOUSE_REPORT_DESCRIPTOR(...) \
    0x05, 0x01,						/*  USAGE_PAGE (Generic Desktop)	// 54 */ \
    0x09, 0x02,						/*  USAGE (Mouse) */ \
    0xa1, 0x01,						/*  COLLECTION (Application) */ \
    0x85, MACRO_GET_ARGUMENT_1_WITH_DEFAULT(HID_MOUSE_REPORT_ID, ## __VA_ARGS__),  /*    REPORT_ID */ \
    0x09, 0x01,						/*    USAGE (Pointer) */ \
    0xa1, 0x00,						/*    COLLECTION (Physical) */ \
    0x05, 0x09,						/*      USAGE_PAGE (Button) */ \
    0x19, 0x01,						/*      USAGE_MINIMUM (Button 1) */ \
    0x29, 0x08,						/*      USAGE_MAXIMUM (Button 8) */ \
    0x15, 0x00,						/*      LOGICAL_MINIMUM (0) */ \
    0x25, 0x01,						/*      LOGICAL_MAXIMUM (1) */ \
    0x95, 0x08,						/*      REPORT_COUNT (8) */ \
    0x75, 0x01,						/*      REPORT_SIZE (1) */ \
    0x81, 0x02,						/*      INPUT (Data,Var,Abs) */ \
    0x05, 0x01,						/*      USAGE_PAGE (Generic Desktop) */ \
    0x09, 0x30,						/*      USAGE (X) */ \
    0x09, 0x31,						/*      USAGE (Y) */ \
    0x09, 0x38,						/*      USAGE (Wheel) */ \
    0x15, 0x81,						/*      LOGICAL_MINIMUM (-127) */ \
    0x25, 0x7f,						/*      LOGICAL_MAXIMUM (127) */ \
    0x75, 0x08,						/*      REPORT_SIZE (8) */ \
    0x95, 0x03,						/*      REPORT_COUNT (3) */ \
    0x81, 0x06,						/*      INPUT (Data,Var,Rel) */ \
    0xc0,      						/*    END_COLLECTION */ \
    MACRO_ARGUMENT_2_TO_END(__VA_ARGS__)  \
    0xc0      						/*  END_COLLECTION */ 

#define HID_ABS_MOUSE_REPORT_DESCRIPTOR(...) \
    0x05, 0x01,						/*  USAGE_PAGE (Generic Desktop)	// 54 */ \
    0x09, 0x02,						/*  USAGE (Mouse) */ \
    0xa1, 0x01,						/*  COLLECTION (Application) */ \
    0x85, MACRO_GET_ARGUMENT_1_WITH_DEFAULT(HID_MOUSE_REPORT_ID, ## __VA_ARGS__),  /*    REPORT_ID */ \
    0x09, 0x01,						/*    USAGE (Pointer) */ \
    0xa1, 0x00,						/*    COLLECTION (Physical) */ \
    0x05, 0x09,						/*      USAGE_PAGE (Button) */ \
    0x19, 0x01,						/*      USAGE_MINIMUM (Button 1) */ \
    0x29, 0x08,						/*      USAGE_MAXIMUM (Button 8) */ \
    0x15, 0x00,						/*      LOGICAL_MINIMUM (0) */ \
    0x25, 0x01,						/*      LOGICAL_MAXIMUM (1) */ \
    0x95, 0x08,						/*      REPORT_COUNT (8) */ \
    0x75, 0x01,						/*      REPORT_SIZE (1) */ \
    0x81, 0x02,						/*      INPUT (Data,Var,Abs) */ \
    0x05, 0x01,						/*      USAGE_PAGE (Generic Desktop) */ \
    0x09, 0x30,						/*      USAGE (X) */ \
    0x09, 0x31,						/*      USAGE (Y) */ \
    0x16, 0x00, 0x80,						/*      LOGICAL_MINIMUM (-32768) */ \
    0x26, 0xFF, 0x7f,						/*      LOGICAL_MAXIMUM (32767) */ \
    0x75, 0x10,						/*      REPORT_SIZE (16) */ \
    0x95, 0x02,						/*      REPORT_COUNT (2) */ \
    0x81, 0x02,						/*      INPUT (Data,Var,Abs) */ \
    0x09, 0x38,						/*      USAGE (Wheel) */ \
    0x15, 0x81,						/*      LOGICAL_MINIMUM (-127) */ \
    0x25, 0x7f,						/*      LOGICAL_MAXIMUM (127) */ \
    0x75, 0x08,						/*      REPORT_SIZE (8) */ \
    0x95, 0x01,						/*      REPORT_COUNT (1) */ \
    0x81, 0x06,						/*      INPUT (Data,Var,Rel) */ \
    0xc0,     						/*  END_COLLECTION */  \
    MACRO_ARGUMENT_2_TO_END(__VA_ARGS__)  \
    0xc0      						/*  END_COLLECTION */ 

#define HID_KEYBOARD_REPORT_DESCRIPTOR(...) \
    0x05, 0x01,						/*  USAGE_PAGE (Generic Desktop)	// 47 */ \
    0x09, 0x06,						/*  USAGE (Keyboard) */ \
    0xa1, 0x01,						/*  COLLECTION (Application) */ \
    0x85, MACRO_GET_ARGUMENT_1_WITH_DEFAULT(HID_KEYBOARD_REPORT_ID, ## __VA_ARGS__),  /*    REPORT_ID */ \
    0x05, 0x07,						/*    USAGE_PAGE (Keyboard) */ \
	0x19, 0xe0,						/*    USAGE_MINIMUM (Keyboard LeftControl) */ \
    0x29, 0xe7,						/*    USAGE_MAXIMUM (Keyboard Right GUI) */ \
    0x15, 0x00,						/*    LOGICAL_MINIMUM (0) */ \
    0x25, 0x01,						/*    LOGICAL_MAXIMUM (1) */ \
    0x75, 0x01,						/*    REPORT_SIZE (1) */ \
	0x95, 0x08,						/*    REPORT_COUNT (8) */ \
    0x81, 0x02,						/*    INPUT (Data,Var,Abs) */ \
\
    0x95, 0x01,						/*    REPORT_COUNT (1) */ \
    0x75, 0x08,						/*    REPORT_SIZE (8) */ \
    0x81, 0x03,						/*    INPUT (Cnst,Var,Abs) */ \
\
	0x95, HID_KEYBOARD_ROLLOVER,						/*    REPORT_COUNT (6) */ \
    0x75, 0x08,						/*    REPORT_SIZE (8) */ \
    0x15, 0x00,						/*    LOGICAL_MINIMUM (0) */ \
    0x25, 0x65,						/*    LOGICAL_MAXIMUM (101) */ \
    0x05, 0x07,						/*    USAGE_PAGE (Keyboard) */ \
\
	0x19, 0x00,						/*    USAGE_MINIMUM (Reserved (no event indicated)) */ \
    0x29, 0x65,						/*    USAGE_MAXIMUM (Keyboard Application) */ \
    0x81, 0x00,						/*    INPUT (Data,Ary,Abs) */ \
\
	0x05, 0x08,						 /*   USAGE_PAGE (LEDs) */ \
	0x19, 0x01,						 /*   USAGE_MINIMUM (Num Lock) */ \
	0x29, 0x08,						 /*   USAGE_MAXIMUM (Kana + 3 custom)*/ \
	0x95, 0x08,						 /*   REPORT_COUNT (8) */ \
	0x75, 0x01,						 /*   REPORT_SIZE (1) */ \
	0x91, 0x02,						 /*   OUTPUT (Data,Var,Abs) */    \
    MACRO_ARGUMENT_2_TO_END(__VA_ARGS__)  \
    0xc0      						/*  END_COLLECTION */
    
#define HID_BOOT_KEYBOARD_REPORT_DESCRIPTOR(...) \
    0x05, 0x01,						/*  USAGE_PAGE (Generic Desktop)	// 47 */ \
    0x09, 0x06,						/*  USAGE (Keyboard) */ \
    0xa1, 0x01,						/*  COLLECTION (Application) */ \
    0x05, 0x07,						/*    USAGE_PAGE (Keyboard) */ \
	0x19, 0xe0,						/*    USAGE_MINIMUM (Keyboard LeftControl) */ \
    0x29, 0xe7,						/*    USAGE_MAXIMUM (Keyboard Right GUI) */ \
    0x15, 0x00,						/*    LOGICAL_MINIMUM (0) */ \
    0x25, 0x01,						/*    LOGICAL_MAXIMUM (1) */ \
    0x75, 0x01,						/*    REPORT_SIZE (1) */ \
	0x95, 0x08,						/*    REPORT_COUNT (8) */ \
    0x81, 0x02,						/*    INPUT (Data,Var,Abs) */ \
\
    0x95, 0x01,						/*    REPORT_COUNT (1) */ \
    0x75, 0x08,						/*    REPORT_SIZE (8) */ \
    0x81, 0x03,						/*    INPUT (Cnst,Var,Abs) */ \
\
	0x95, 0x06,						/*    REPORT_COUNT (6) */ \
    0x75, 0x08,						/*    REPORT_SIZE (8) */ \
    0x15, 0x00,						/*    LOGICAL_MINIMUM (0) */ \
    0x25, 0x65,						/*    LOGICAL_MAXIMUM (101) */ \
    0x05, 0x07,						/*    USAGE_PAGE (Keyboard) */ \
\
	0x19, 0x00,						/*    USAGE_MINIMUM (Reserved (no event indicated)) */ \
    0x29, 0x65,						/*    USAGE_MAXIMUM (Keyboard Application) */ \
    0x81, 0x00,						/*    INPUT (Data,Ary,Abs) */ \
\
	0x05, 0x08,						 /*   USAGE_PAGE (LEDs) */ \
	0x19, 0x01,						 /*   USAGE_MINIMUM (Num Lock) */ \
	0x29, 0x08,						 /*   USAGE_MAXIMUM (Kana + 3 custom)*/ \
	0x95, 0x08,						 /*   REPORT_COUNT (8) */ \
	0x75, 0x01,						 /*   REPORT_SIZE (1) */ \
	0x91, 0x02,						 /*   OUTPUT (Data,Var,Abs) */    \
    __VA_ARGS__  \
    0xc0      						/*  END_COLLECTION */
    
#define HID_JOYSTICK_REPORT_DESCRIPTOR(...) \
	0x05, 0x01,						/*  Usage Page (Generic Desktop) */ \
	0x09, 0x04,						/*  Usage (Joystick) */ \
	0xA1, 0x01,						/*  Collection (Application) */ \
    0x85, MACRO_GET_ARGUMENT_1_WITH_DEFAULT(HID_JOYSTICK_REPORT_ID, ## __VA_ARGS__),  /*    REPORT_ID */ \
	0x15, 0x00,						/* 	 Logical Minimum (0) */ \
	0x25, 0x01,						/*    Logical Maximum (1) */ \
	0x75, 0x01,						/*    Report Size (1) */ \
	0x95, 0x20,						/*    Report Count (32) */ \
	0x05, 0x09,						/*    Usage Page (Button) */ \
	0x19, 0x01,						/*    Usage Minimum (Button #1) */ \
	0x29, 0x20,						/*    Usage Maximum (Button #32) */ \
	0x81, 0x02,						/*    Input (variable,absolute) */ \
	0x15, 0x00,						/*    Logical Minimum (0) */ \
	0x25, 0x07,						/*    Logical Maximum (7) */ \
	0x35, 0x00,						/*    Physical Minimum (0) */ \
	0x46, 0x3B, 0x01,				/*    Physical Maximum (315) */ \
	0x75, 0x04,						/*    Report Size (4) */ \
	0x95, 0x01,						/*    Report Count (1) */ \
	0x65, 0x14,						/*    Unit (20) */ \
    0x05, 0x01,                     /*    Usage Page (Generic Desktop) */ \
	0x09, 0x39,						/*    Usage (Hat switch) */ \
	0x81, 0x42,						/*    Input (variable,absolute,null_state) */ \
    0x05, 0x01,                     /* Usage Page (Generic Desktop) */ \
	0x09, 0x01,						/* Usage (Pointer) */ \
    0xA1, 0x00,                     /* Collection () */ \
	0x15, 0x00,						/*    Logical Minimum (0) */ \
	0x26, 0xFF, 0x03,				/*    Logical Maximum (1023) */ \
	0x75, 0x0A,						/*    Report Size (10) */ \
	0x95, 0x04,						/*    Report Count (4) */ \
	0x09, 0x30,						/*    Usage (X) */ \
	0x09, 0x31,						/*    Usage (Y) */ \
	0x09, 0x33,						/*    Usage (Rx) */ \
	0x09, 0x34,						/*    Usage (Ry) */ \
	0x81, 0x02,						/*    Input (variable,absolute) */ \
    0xC0,                           /*  End Collection */ \
	0x15, 0x00,						/*  Logical Minimum (0) */ \
	0x26, 0xFF, 0x03,				/*  Logical Maximum (1023) */ \
	0x75, 0x0A,						/*  Report Size (10) */ \
	0x95, 0x02,						/*  Report Count (2) */ \
	0x09, 0x36,						/*  Usage (Slider) */ \
	0x09, 0x36,						/*  Usage (Slider) */ \
	0x81, 0x02,						/*  Input (variable,absolute) */ \
    MACRO_ARGUMENT_2_TO_END(__VA_ARGS__)  \
    0xC0

#define RAWHID_USAGE_PAGE	0xFFC0 // recommended: 0xFF00 to 0xFFFF
#define RAWHID_USAGE		0x0C00 // recommended: 0x0100 to 0xFFFF
    
#define LSB(x) ((x) & 0xFF)    
#define MSB(x) (((x) & 0xFF00) >> 8)    
// TODO: make this work for txSize > 255
#define HID_RAW_REPORT_DESCRIPTOR(txSize, rxSize) \
	0x06, LSB(RAWHID_USAGE_PAGE), MSB(RAWHID_USAGE_PAGE), \
	0x0A, LSB(RAWHID_USAGE), MSB(RAWHID_USAGE), \
	0xA1, 0x01,				/*  Collection 0x01 */ \
/*    0x85, 10,				*/		/*    REPORT_ID (1) */ \
	0x75, 0x08,				/*  report size = 8 bits */ \
	0x15, 0x00,				/*  logical minimum = 0 */ \
	0x26, 0xFF, 0x00,		/*  logical maximum = 255 */ \
\
	0x96, LSB(txSize), MSB(txSize),				/*  report count TX */ \
	0x09, 0x01,				/*  usage */ \
	0x81, 0x02,				/*  Input (array) */ \
\
	0x75, 0x08,				/*  report size = 8 bits */ \
	0x15, 0x00,				/*  logical minimum = 0 */ \
	0x26, 0xFF, 0x00,		/*  logical maximum = 255 */ \
	0x96, LSB(rxSize), MSB(rxSize),				/*  report count RX */ \
	0x09, 0x02,				/*  usage */ \
	0x91, 0x02,				/*  OUTPUT (0x91) */ \
	0xC0					/*  end collection */ 
    
typedef struct {
    uint8_t* descriptor;
    uint16_t length;    
} HIDReportDescriptor;

class USBHIDDevice {
private:
	bool enabledHID = false;
public:
	bool registerComponent();
	void setReportDescriptor(const uint8_t* report_descriptor, uint16_t report_descriptor_length);
	void setReportDescriptor(const HIDReportDescriptor* reportDescriptor);
    // All the strings are zero-terminated ASCII strings. Use NULL for defaults.
    void begin(const uint8_t* report_descriptor, uint16_t length, uint16_t idVendor=0, uint16_t idProduct=0,
        const char* manufacturer=NULL, const char* product=NULL, const char* serialNumber="00000000000000000001");
    void begin(const HIDReportDescriptor* reportDescriptor, uint16_t idVendor=0, uint16_t idProduct=0,
        const char* manufacturer=NULL, const char* product=NULL, const char* serialNumber="00000000000000000001");
    void setSerial(uint8 serialSupport=true);
    void setBuffers(uint8_t buffers, volatile HIDBuffer_t* fb=NULL, int count=0); // type = HID_REPORT_TYPE_FEATURE or HID_REPORT_TYPE_OUTPUT
    bool addBuffer(uint8_t type, volatile HIDBuffer_t* buffer);
	void clearBuffers(uint8_t type);
	void clearBuffers();
    inline bool addFeatureBuffer(volatile HIDBuffer_t* buffer) {
        return addBuffer(HID_REPORT_TYPE_FEATURE, buffer);
    }
    inline bool addOutputBuffer(volatile HIDBuffer_t* buffer) {
        return addBuffer(HID_REPORT_TYPE_OUTPUT, buffer);
    }
    inline void setFeatureBuffers(volatile HIDBuffer_t* fb=NULL, int count=0) {
        setBuffers(HID_REPORT_TYPE_FEATURE, fb, count);
    }        
    inline void setOutputBuffers(volatile HIDBuffer_t* fb=NULL, int count=0) {
        setBuffers(HID_REPORT_TYPE_OUTPUT, fb, count);
    }     
    void end(void);
};

void USBHID_begin_with_serial(const uint8_t* report_descriptor, uint16_t length, uint16_t idVendor=0, uint16_t idProduct=0,
	const char* manufacturer=NULL, const char* product=NULL, const char* serialNumber="00000000000000000001");
void USBHID_begin_with_serial(const HIDReportDescriptor* reportDescriptor, uint16_t idVendor=0, uint16_t idProduct=0,
	const char* manufacturer=NULL, const char* product=NULL, const char* serialNumber="00000000000000000001");

class HIDReporter {
    private:
        uint8_t* buffer;
        unsigned bufferSize;
        uint8_t reportID;
        
    public:
        void sendReport(); 
        
    public:
        // if you use this init function, the buffer starts with a reportID, even if the reportID is zero,
        // and bufferSize includes the reportID; if reportID is zero, sendReport() will skip the initial
        // reportID byte
        HIDReporter(uint8_t* _buffer, unsigned _size, uint8_t _reportID);
        // if you use this init function, the buffer has no reportID byte in it
        HIDReporter(uint8_t* _buffer, unsigned _size);
        uint16_t getFeature(uint8_t* out=NULL, uint8_t poll=1);
        uint16_t getOutput(uint8_t* out=NULL, uint8_t poll=1);
        uint16_t getData(uint8_t type, uint8_t* out, uint8_t poll=1); // type = HID_REPORT_TYPE_FEATURE or HID_REPORT_TYPE_OUTPUT
        void setFeature(uint8_t* feature);
};

//================================================================================
//================================================================================
//	Mouse

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

class HIDMouse : public HIDReporter {
protected:
    uint8_t _buttons;
	void buttons(uint8_t b);
    uint8_t reportBuffer[5];
public:
	HIDMouse(uint8_t reportID=HID_MOUSE_REPORT_ID) : HIDReporter(reportBuffer, sizeof(reportBuffer), reportID), _buttons(0) {}
	void begin(void);
	void end(void);
	void click(uint8_t b = MOUSE_LEFT);
	void move(signed char x, signed char y, signed char wheel = 0);
	void press(uint8_t b = MOUSE_LEFT);		// press LEFT by default
	void release(uint8_t b = MOUSE_LEFT);	// release LEFT by default
	bool isPressed(uint8_t b = MOUSE_ALL);	// check all buttons by default
};

typedef struct {
    uint8_t reportID;
    uint8_t buttons;
    int16_t x;
    int16_t y;
    uint8_t wheel;
} __packed AbsMouseReport_t;

class HIDAbsMouse : public HIDReporter {
protected:
	void buttons(uint8_t b);
    AbsMouseReport_t report;
public:
	HIDAbsMouse(uint8_t reportID=HID_MOUSE_REPORT_ID) : HIDReporter((uint8_t*)&report, sizeof(report), reportID) {
        report.buttons = 0;
        report.x = 0;
        report.y = 0;
        report.wheel = 0;
    }
	void begin(void);
	void end(void);
	void click(uint8_t b = MOUSE_LEFT);
	void move(int16_t x, int16_t y, int8_t wheel = 0);
	void press(uint8_t b = MOUSE_LEFT);		// press LEFT by default
	void release(uint8_t b = MOUSE_LEFT);	// release LEFT by default
	bool isPressed(uint8_t b = MOUSE_ALL);	// check all buttons by default
};

typedef struct {
    uint8_t reportID;
    uint16_t button;
} __packed ConsumerReport_t;

class HIDConsumer : public HIDReporter {
protected:
    ConsumerReport_t report;
public:
    enum { 
           BRIGHTNESS_UP = 0x6F, 
           BRIGHTNESS_DOWN = 0x70, 
           VOLUME_UP = 0xE9, 
           VOLUME_DOWN = 0xEA,
           MUTE = 0xE2, 
           PLAY_OR_PAUSE = 0xCD
           // see pages 75ff of http://www.usb.org/developers/hidpage/Hut1_12v2.pdf
           };
	HIDConsumer(uint8_t reportID=HID_CONSUMER_REPORT_ID) : HIDReporter((uint8_t*)&report, sizeof(report), reportID) {
        report.button = 0;
    }
	void begin(void);
	void end(void);
    void press(uint16_t button);
    void release();
};

#define KEY_LEFT_CTRL		0x80
#define KEY_LEFT_SHIFT		0x81
#define KEY_LEFT_ALT		0x82
#define KEY_LEFT_GUI		0x83
#define KEY_RIGHT_CTRL		0x84
#define KEY_RIGHT_SHIFT		0x85
#define KEY_RIGHT_ALT		0x86
#define KEY_RIGHT_GUI		0x87

#define KEY_UP_ARROW		0xDA
#define KEY_DOWN_ARROW		0xD9
#define KEY_LEFT_ARROW		0xD8
#define KEY_RIGHT_ARROW		0xD7
#define KEY_BACKSPACE		0xB2
#define KEY_TAB				0xB3
#define KEY_RETURN			0xB0
#define KEY_ESC				0xB1
#define KEY_INSERT			0xD1
#define KEY_DELETE			0xD4
#define KEY_PAGE_UP			0xD3
#define KEY_PAGE_DOWN		0xD6
#define KEY_HOME			0xD2
#define KEY_END				0xD5
#define KEY_CAPS_LOCK		0xC1
#define KEY_F1				0xC2
#define KEY_F2				0xC3
#define KEY_F3				0xC4
#define KEY_F4				0xC5
#define KEY_F5				0xC6
#define KEY_F6				0xC7
#define KEY_F7				0xC8
#define KEY_F8				0xC9
#define KEY_F9				0xCA
#define KEY_F10				0xCB
#define KEY_F11				0xCC
#define KEY_F12				0xCD

typedef struct{
    uint8_t reportID;
	uint8_t modifiers;
	uint8_t reserved;
	uint8_t keys[HID_KEYBOARD_ROLLOVER];
} __packed KeyReport_t;

class HIDKeyboard : public Print, public HIDReporter {
public:
	KeyReport_t keyReport;
    
protected:    
    uint8_t leds[HID_BUFFER_ALLOCATE_SIZE(1,1)];
    HIDBuffer_t ledData;
    uint8_t reportID;
    uint8_t getKeyCode(uint8_t k, uint8_t* modifiersP);
    bool adjustForHostCapsLock = true;

public:
	HIDKeyboard(uint8_t _reportID=HID_KEYBOARD_REPORT_ID) : 
        HIDReporter((uint8*)&keyReport, sizeof(KeyReport_t), _reportID),
        ledData(leds, HID_BUFFER_SIZE(1,_reportID), _reportID, HID_BUFFER_MODE_NO_WAIT),
        reportID(_reportID)
        {}
	void begin(void);
	void end(void);
    void setAdjustForHostCapsLock(bool state) {
        adjustForHostCapsLock = state;
    }
    inline uint8 getLEDs(void) {
        return leds[reportID != 0 ? 1 : 0];
    }
	virtual size_t write(uint8_t k);
	virtual size_t press(uint8_t k);
	virtual size_t release(uint8_t k);
	virtual void releaseAll(void);
};


//================================================================================
//================================================================================
//	Joystick

// only works for little-endian machines, but makes the code so much more
// readable
typedef struct {
    uint8_t reportID;
    uint32_t buttons;
    unsigned hat:4;
    unsigned x:10;
    unsigned y:10;
    unsigned rx:10;
    unsigned ry:10;
    unsigned sliderLeft:10;
    unsigned sliderRight:10;
} __packed JoystickReport_t;

static_assert(sizeof(JoystickReport_t)==13, "Wrong endianness/packing!");

class HIDJoystick : public HIDReporter {
protected:
	JoystickReport_t joyReport; 
    bool manualReport = false;
	void safeSendReport(void);
public:
	inline void send(void) {
        sendReport();
    }
    void setManualReportMode(bool manualReport); // in manual report mode, reports only sent when send() is called
    bool getManualReportMode();
	void begin(void);
	void end(void);
	void button(uint8_t button, bool val);
	void X(uint16_t val);
	void Y(uint16_t val);
	void position(uint16_t x, uint16_t y);
	void Xrotate(uint16_t val);
	void Yrotate(uint16_t val);
	void sliderLeft(uint16_t val);
	void sliderRight(uint16_t val);
	void slider(uint16_t val);
	void hat(int16_t dir);
	HIDJoystick(uint8_t reportID=HID_JOYSTICK_REPORT_ID) : HIDReporter((uint8_t*)&joyReport, sizeof(joyReport), reportID) {
        joyReport.buttons = 0;
        joyReport.hat = 15;
        joyReport.x = 512;
        joyReport.y = 512;
        joyReport.rx = 512;
        joyReport.ry = 512;
        joyReport.sliderLeft = 0;
        joyReport.sliderRight = 0;
    }
};

extern USBHIDDevice USBHID;

template<unsigned txSize,unsigned rxSize>class HIDRaw : public HIDReporter {
private:
    uint8_t txBuffer[txSize];
    uint8_t rxBuffer[HID_BUFFER_ALLOCATE_SIZE(rxSize,0)];
    HIDBuffer_t buf;
public:
	HIDRaw() : HIDReporter(txBuffer, sizeof(txBuffer)) {}
	void begin(void) {
        buf.buffer = rxBuffer;
        buf.bufferSize = HID_BUFFER_SIZE(rxSize,0);
        buf.reportID = 0;
        USBHID.addOutputBuffer(&buf);
    }
	void end(void);
	void send(const uint8_t* data, unsigned n=sizeof(txBuffer)) {
        memset(txBuffer, 0, sizeof(txBuffer));
        memcpy(txBuffer, data, n>sizeof(txBuffer)?sizeof(txBuffer):n);
        sendReport();
    }
};

extern HIDMouse Mouse;
extern HIDKeyboard Keyboard;
extern HIDJoystick Joystick;
extern HIDKeyboard BootKeyboard;

extern const HIDReportDescriptor* hidReportMouse;
extern const HIDReportDescriptor* hidReportKeyboard;
extern const HIDReportDescriptor* hidReportJoystick;
extern const HIDReportDescriptor* hidReportKeyboardMouse;
extern const HIDReportDescriptor* hidReportKeyboardJoystick;
extern const HIDReportDescriptor* hidReportKeyboardMouseJoystick;
extern const HIDReportDescriptor* hidReportBootKeyboard;

#define HID_MOUSE                   hidReportMouse
#define HID_KEYBOARD                hidReportKeyboard
#define HID_JOYSTICK                hidReportJoystick
#define HID_KEYBOARD_MOUSE          hidReportKeyboardMouse
#define HID_KEYBOARD_JOYSTICK       hidReportKeyboardJoystick
#define HID_KEYBOARD_MOUSE_JOYSTICK hidReportKeyboardMouseJoystick
#define HID_BOOT_KEYBOARD           hidReportBootKeyboard

#endif
        		