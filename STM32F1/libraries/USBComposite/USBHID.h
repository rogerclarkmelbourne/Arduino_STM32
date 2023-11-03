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

#include <Print.h>
#include <boards.h>
#include "Stream.h"
#include "usb_hid.h"

#define HID_MAX_REPORT_CHUNKS 24

#define HID_REPORT_ID_NONE -1
#define HID_REPORT_ID_AUTO -2

#define HID_MOUSE_REPORT_ID 1
#define HID_KEYBOARD_REPORT_ID 2
#define HID_CONSUMER_REPORT_ID 3
#define HID_DIGITIZER_REPORT_ID 4
#define HID_JOYSTICK_REPORT_ID 20

#define HID_KEYBOARD_ROLLOVER 6

#define HID_AUTO_REPORT_ID_START 0x80

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

#define HID_DIGITIZER_REPORT_DESCRIPTOR(...) \
    /* based on https://stackoverflow.com/questions/46033581/what-is-a-correct-usbhidreportdescriptor-for-android and https://forum.pjrc.com/threads/48762-USB-mouse-touchscreen-with-Teensy3-2-for-Android */ \
    0x05, 0x0d,                    /* USAGE_PAGE (Digitizer) */ \
    /*0x09, 0x02, */                   /* USAGE (Pen) */ \
	0x09, 0x04,                    /* USAGE (Touch Screen) */ \
    0xa1, 0x01,                    /* COLLECTION (Application) */ \
    0x85, MACRO_GET_ARGUMENT_1_WITH_DEFAULT(HID_DIGITIZER_REPORT_ID, ## __VA_ARGS__),  /*    REPORT_ID */ \
    /* declare a finger collection */ \
    0x09, 0x20,                    /*   Usage (Stylus) */ \
    0xA1, 0x00,                    /*   Collection (Physical) */ \
    /* Declare a finger touch (finger up/down) */ \
    0x09, 0x42,                    /*     Usage (Tip Switch) */ \
    0x09, 0x32,                    /*     USAGE (In Range) */ \
    0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */ \
    0x25, 0x01,                    /*     LOGICAL_MAXIMUM (1) */ \
    0x75, 0x01,                    /*     REPORT_SIZE (1) */ \
    0x95, 0x08,                    /*     REPORT_COUNT (8) */ \
    0x81, 0x02,                    /*     INPUT (Data,Var,Abs) */ \
    /* Define absolute X and Y coordinates of 12 bits each */ \
    /* http://www.usb.org/developers/hidpage/Hut1_12v2.pdf */ \
    0x05, 0x01,                    /*     Usage Page (Generic Desktop) */ \
    0x09, 0x01,                    /*     Usage (Pointer) */ \
    0xA1, 0x00,                    /*     Collection (Physical) */ \
    0x09, 0x30,                    /*        Usage (X) */ \
    0x09, 0x31,                    /*        Usage (Y) */ \
    0x16, 0x00, 0x00,              /*        Logical Minimum (0) */ \
    0x26, 0xFF, 0x0F,              /*        Logical Maximum (4095) */ \
    0x36, 0x00, 0x00,              /*        Physical Minimum (0) */ \
    0x46, 0xFF, 0x0F,              /*        Physical Maximum (4095) */ \
    0x66, 0x00, 0x00,              /*        UNIT (None) */ \
    0x75, 0x0C,                    /*        Report Size (12), */ \
    0x95, 0x02,                    /*        Report Count (2), */ \
    0x81, 0x02,                    /*        Input (Data,Var,Abs) */ \
    0xc0,                          /*     END_COLLECTION */ \
    0xc0,                          /*   END_COLLECTION */ \
    MACRO_ARGUMENT_2_TO_END(__VA_ARGS__)  \
    0xc0                           /* END_COLLECTION */
// With this declaration a data packet must be sent as:
// bits 0-7  -> "touch" state          (bit 0 = pen up/down, bit 1 = In Range)
// bits 8-19 -> absolute X coordinate (0...4095)
// bits 20-31-> absolute Y coordinate (0...4095)

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
    0x26, 0xDD, 0x00,				/*    LOGICAL_MAXIMUM (261) */ \
    0x05, 0x07,						/*    USAGE_PAGE (Keyboard) */ \
\
	0x19, 0x00,						/*    USAGE_MINIMUM (Reserved (no event indicated)) */ \
    0x29, 0xDD,						/*    USAGE_MAXIMUM (Keypad Hexadecimal) */ \
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

#define HID_SWITCH_CONTROLLER_REPORT_DESCRIPTOR(...) \
  0x05, 0x01,           /*  Usage Page (Generic Desktop) */ \
  0x09, 0x05,           /*  Usage (Joystick) */ \
  0xA1, 0x01,           /*  Collection (Application) */ \
  /* buttons */ \
  0x15, 0x00,           /*  Logical Minimum (0) */ \
  0x25, 0x01,           /*  Logical Maximum (1) */ \
  0x35, 0x00,           /*  Physical Minimum (0) */ \
  0x45, 0x01,           /*  Physical Maximum (1) */ \
  0x75, 0x01,           /*  Report Size (1) */ \
  0x95, 0x0e,           /*  Report Count (16) */ \
  0x05, 0x09,           /*  Usage Page (9) */ \
  0x19, 0x01,           /*  Usage Minimum (1) */ \
  0x29, 0x0e,           /*  Usage Maximum (16) */ \
  0x81, 0x02,           /*  Input (Data,Var,Abs) */ \
  0x95, 0x02, \
  0x81, 0x01, \
  /* HAT Switch */ \
  0x05, 0x01,           /*  Usage Page (1) */ \
  0x25, 0x07,           /*  Logical Maximum (7) */ \
  0x46, 315&0xFF,315>>8, /*  Physical Maximum (315) */ \
  0x75, 0x04,           /*  Report Size (4) */ \
  0x95, 0x01,           /*  Report Count (1) */ \
  0x65, 20,             /*  Unit (20) */ \
  0x09, 57,             /*  Usage (57) */ \
  0x81, 66,             /*  Input (66) */ \
    /* additional nibble here for Switch Pro Controller? */ \
  0x65, 0,              /*  Unit (0) */ \
  0x95, 0x01,           /*  Report Count (1) */ \
  0x81, 1,              /*  Input (1) */ \
    /* sticks */ \
  0x26, 255 & 0xFF, 255 >> 8,    /*  Logical Maximum (255) */ \
  0x46, 255 & 0xFF, 255 >> 8,    /*  Physical Maximum (255) */ \
  0x09, 48,             /*  Usage (48) */ \
  0x09, 49,             /*  Usage (49) */ \
  0x09, 50,             /*  Usage (50) */ \
  0x09, 53,             /*  Usage (53) */ \
  0x75, 0x08,           /*  Report Size (8) */ \
  0x95, 0x04,           /*  Report Count (4) */ \
  0x81, 0x02,           /*  Input (Data,Var,Abs) */ \
  0x75, 0x08,           /*  Report Size (8) */ \
  0x95, 0x01,           /*  Report Count (1) */ \
  0x81, 0x01,           /*  Input (1) */ \
  __VA_ARGS__ \
  0xC0,                 /*  End Collection */
    

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
    const uint8_t* descriptor;
    uint16_t length;    
} HIDReportDescriptor;

extern const HIDReportDescriptor* hidReportMouse;
extern const HIDReportDescriptor* hidReportKeyboard;
extern const HIDReportDescriptor* hidReportJoystick;
extern const HIDReportDescriptor* hidReportKeyboardMouse;
extern const HIDReportDescriptor* hidReportKeyboardJoystick;
extern const HIDReportDescriptor* hidReportKeyboardMouseJoystick;
extern const HIDReportDescriptor* hidReportBootKeyboard;
extern const HIDReportDescriptor* hidReportAbsMouse;
extern const HIDReportDescriptor* hidReportDigitizer;
extern const HIDReportDescriptor* hidReportConsumer;
extern const HIDReportDescriptor* hidReportSwitchController;

#define HID_MOUSE                   hidReportMouse
#define HID_KEYBOARD                hidReportKeyboard
#define HID_JOYSTICK                hidReportJoystick
#define HID_KEYBOARD_MOUSE          hidReportKeyboardMouse
#define HID_KEYBOARD_JOYSTICK       hidReportKeyboardJoystick
#define HID_KEYBOARD_MOUSE_JOYSTICK hidReportKeyboardMouseJoystick
#define HID_BOOT_KEYBOARD           hidReportBootKeyboard

class HIDReporter;

class USBHID {
private:
    bool autoRegister = true;
	bool enabledHID = false;
    uint32 txPacketSize = 64;
    struct usb_chunk* chunkList = nullptr;
    // baseChunk holds any explicitly specified report descriptor that
    // overrides any report descriptors from the chain of registered profiles
    struct usb_chunk baseChunk = { 0, 0, 0 };
    HIDReporter* profiles = nullptr;
public:
	static bool init(USBHID* me);
    // add a report to the list ; if always is false, then it only works if autoRegister is true
    void addReport(HIDReporter* r, bool always=true);
    void clear();
	bool registerComponent();
	void setReportDescriptor(const uint8_t* report_descriptor, uint16_t report_descriptor_length);
	void setReportDescriptor(const HIDReportDescriptor* reportDescriptor=NULL);
    // All the strings are zero-terminated ASCII strings. Use NULL for defaults.
    void begin(const uint8_t* report_descriptor, uint16_t length);
    void begin(const HIDReportDescriptor* reportDescriptor = NULL);
    void begin(USBCompositeSerial serial, const uint8_t* report_descriptor, uint16_t length);
    void begin(USBCompositeSerial serial, const HIDReportDescriptor* reportDescriptor = NULL);
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
    void setTXPacketSize(uint32 size=64) {
        txPacketSize = size;
    }
    USBHID(bool _autoRegister=true) {
        autoRegister = _autoRegister;
    }
    void setTXInterval(uint8 t) {
        usb_hid_setTXInterval(t);
    }
    void setRXInterval(uint8 t) {
        usb_hid_setRXInterval(t);
    }
};

class HIDReporter {
    private:
        uint8_t* reportBuffer;
        uint8_t reportID;
        uint8_t userSuppliedReportID;
        bool forceUserSuppliedReportID;
        uint16_t bufferSize;
        HIDReportDescriptor reportDescriptor;
        struct usb_chunk reportChunks[3];
        class HIDReporter* next;
        friend class USBHID;

    protected:
        USBHID& HID;
        
    public:
        void sendReport(); 
        uint8_t* getReport() {
            return reportBuffer;
        }
        uint16_t getReportSize() {
            return bufferSize;
        }
        // if you use this init function, the buffer starts with a reportID, even if the reportID is zero,
        // and bufferSize includes the reportID; if reportID is zero, sendReport() will skip the initial
        // reportID byte
        HIDReporter(USBHID& _HID, const HIDReportDescriptor* r, uint8_t* _buffer, unsigned _size, uint8_t _reportID, bool forceReportID=false);
        HIDReporter(USBHID& _HID, uint8_t* _buffer, unsigned _size, uint8_t _reportID, bool forceReportID=false) : HIDReporter(_HID, NULL, _buffer, _size, _reportID, forceReportID) {}
        // if you use these init functions, the buffer has no reportID byte in it
        HIDReporter(USBHID& _HID, const HIDReportDescriptor* r, uint8_t* _buffer, unsigned _size);
        HIDReporter(USBHID& _HID, uint8_t* _buffer, unsigned _size) : HIDReporter(_HID, NULL, _buffer, _size) {}
        uint16_t getFeature(uint8_t* out=NULL, uint8_t poll=1);
        uint16_t getOutput(uint8_t* out=NULL, uint8_t poll=1);
        uint16_t getData(uint8_t type, uint8_t* out, uint8_t poll=1); // type = HID_REPORT_TYPE_FEATURE or HID_REPORT_TYPE_OUTPUT
        void setFeature(uint8_t* feature);
        void registerProfile(bool always=true);
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
	HIDMouse(USBHID& HID, uint8_t reportID=HID_MOUSE_REPORT_ID) : HIDReporter(HID, hidReportMouse, reportBuffer, sizeof(reportBuffer), reportID), _buttons(0) {}
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
	HIDAbsMouse(USBHID& HID, uint8_t reportID=HID_MOUSE_REPORT_ID) : HIDReporter(HID, hidReportAbsMouse, (uint8_t*)&report, sizeof(report), reportID) {
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

#define DIGITIZER_TOUCH 1 // for touch to be processed it could be required to be "in range" first
#define DIGITIZER_IN_RANGE 2
#define DIGITIZER_TOUCH_IN_RANGE (DIGITIZER_TOUCH | DIGITIZER_IN_RANGE)

typedef struct {
    uint8_t reportID;
    uint8_t buttons;
    uint16_t x:12;
    uint16_t y:12;
} __packed DigitizerReport_t;

class HIDDigitizer : public HIDReporter {
protected:
	void buttons(uint8_t b);
    DigitizerReport_t report;
public:
	HIDDigitizer(USBHID& HID, uint8_t reportID=HID_DIGITIZER_REPORT_ID) : HIDReporter(HID, hidReportDigitizer, (uint8_t*)&report, sizeof(report), reportID) {
        report.buttons = 0;
        report.x = 0;
        report.y = 0;
    }
	void begin(void);
	void end(void);
	void move(uint16_t x, uint16_t y); // coordinates are 0 to 16383
	void click(uint8_t b = DIGITIZER_TOUCH_IN_RANGE);
	void press(uint8_t b = DIGITIZER_TOUCH_IN_RANGE);
	void release(uint8_t b = DIGITIZER_TOUCH_IN_RANGE);
	bool isPressed(uint8_t b = DIGITIZER_TOUCH_IN_RANGE);
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
           NEXT_TRACK = 0xB5,
           PREVIOUS_TRACK = 0xB6,
           PLAY_OR_PAUSE = 0xCD,
           VOLUME_UP = 0xE9,
           VOLUME_DOWN = 0xEA,
           REWIND = 0xB4,
           FAST_FORWARD = 0xB3,
           MUTE = 0xE2,
           MENU = 0x40,
           MENU_PICK = 0x41,
           MENU_UP = 0x42,
           MENU_DOWN = 0x43,
           MENU_LEFT = 0x44,
           MENU_RIGHT = 0x45,
           MENU_ESCAPE = 0x46,
           // see pages 117 of https://www.usb.org/sites/default/files/hut1_22.pdf
           };
	HIDConsumer(USBHID& HID, uint8_t reportID=HID_CONSUMER_REPORT_ID) : HIDReporter(HID, hidReportConsumer, (uint8_t*)&report, sizeof(report), reportID) {
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

#define KEY_HID_OFFSET      0x88

// The following definitions takes their value from the document at https://www.usb.org/sites/default/files/hut1_22.pdf, starting p82
// However, their value are augmented by KEY_HID_OFFSET (for example, KEY_F12 real value is 0x45)
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
#define KEY_F13             (KEY_HID_OFFSET+0x68)
#define KEY_F14             (KEY_HID_OFFSET+0x69)
#define KEY_F15             (KEY_HID_OFFSET+0x6A)
#define KEY_KP_DOT          (KEY_HID_OFFSET+0x63)
#define KEY_KP_ASTERISK     (KEY_HID_OFFSET+0x55)
#define KEY_KP_PLUS         (KEY_HID_OFFSET+0x57)
#define KEY_NUM_LOCK        (KEY_HID_OFFSET+0x53)
#define KEY_KP_SLASH        (KEY_HID_OFFSET+0x54)
#define KEY_KP_ENTER        (KEY_HID_OFFSET+0x58)
#define KEY_KP_MINUS        (KEY_HID_OFFSET+0x56)
#define KEY_KP_EQUAL        (KEY_HID_OFFSET+0x86)
#define KEY_KP_0            (KEY_HID_OFFSET+0x62)
#define KEY_KP_1            (KEY_HID_OFFSET+0x59)
#define KEY_KP_2            (KEY_HID_OFFSET+0x5a)
#define KEY_KP_3            (KEY_HID_OFFSET+0x5b)
#define KEY_KP_4            (KEY_HID_OFFSET+0x5c)
#define KEY_KP_5            (KEY_HID_OFFSET+0x5d)
#define KEY_KP_6            (KEY_HID_OFFSET+0x5e)
#define KEY_KP_7            (KEY_HID_OFFSET+0x5f)
#define KEY_KP_8            (KEY_HID_OFFSET+0x60)
#define KEY_KP_9            (KEY_HID_OFFSET+0x61)
#define KEY_VOLUME_UP       (KEY_HID_OFFSET+0x80)
#define KEY_VOLUME_DOWN     (KEY_HID_OFFSET+0x81)
#define KEY_MUTE            (KEY_HID_OFFSET+0x7f)
#define KEY_HELP            (KEY_HID_OFFSET+0x75)
#define KEY_POWER           (KEY_HID_OFFSET+0x66)
#define KEY_102ND           (KEY_HID_OFFSET+0x64)
#define KEY_PRINT_SCREEN    (KEY_HID_OFFSET+0x46)    
#define KEY_SCROLL_LOCK     (KEY_HID_OFFSET+0x47)
#define KEY_PAUSE           (KEY_HID_OFFSET+0x48)       



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
    uint8_t getKeyCode(uint16_t k, uint8_t* modifiersP);
    bool adjustForHostCapsLock = true;

public:
	HIDKeyboard(USBHID& HID, uint8_t _reportID=HID_KEYBOARD_REPORT_ID) : 
        HIDReporter(HID, hidReportKeyboard, (uint8*)&keyReport, sizeof(KeyReport_t), _reportID),
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
	virtual size_t press(uint16_t k);
	virtual size_t release(uint16_t k);
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
    void setManualReportMode(bool manualReport); // in manual report mode, report only sent when send() is called
    bool getManualReportMode();
	void begin(void);
	void end(void);
	void button(uint8_t button, bool val);
    void buttons(uint32_t b) {
        joyReport.buttons = b;
    }
	void X(uint16_t val);
	void Y(uint16_t val);
	void position(uint16_t x, uint16_t y);
	void Xrotate(uint16_t val);
	void Yrotate(uint16_t val);
	void sliderLeft(uint16_t val);
	void sliderRight(uint16_t val);
	void slider(uint16_t val);
	void hat(int16_t dir);
	HIDJoystick(USBHID& HID, uint8_t reportID=HID_JOYSTICK_REPORT_ID) 
            : HIDReporter(HID, hidReportJoystick, (uint8_t*)&joyReport, sizeof(joyReport), reportID) {
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

typedef struct {
  uint16_t buttons;
  uint8_t dpad; 
  uint8_t leftX;
  uint8_t leftY;
  uint8_t rightX;
  uint8_t rightY;
  uint8_t pad;
} __packed SwitchControllerReport_t;

class HIDSwitchController : public HIDReporter {
protected:
  SwitchControllerReport_t report; 
  bool manualReport = false;
  
public:
  const int32_t AXIS_MAX = 255;
  const int32_t AXIS_NEUTRAL = 128;
  const int32_t AXIS_MIN = 0;

  enum {
      DPAD_TOP = 0,
      DPAD_TOPRIGHT = 1,
      DPAD_RIGHT = 2,
      DPAD_BOTRIGHT = 3,
      DPAD_BOTTOM = 4,
      DPAD_BOTLEFT = 5,
      DPAD_LEFT = 6,
      DPAD_TOPLEFT = 7,
      DPAD_NEUTRAL = 8,
  };

  enum {
      BUTTON_Y = 0,
      BUTTON_B = 1,
      BUTTON_A = 2,
      BUTTON_X = 3,
      BUTTON_L = 4,
      BUTTON_R = 5,
      BUTTON_ZL = 6,
      BUTTON_ZR = 7,
      BUTTON_MINUS = 8,
      BUTTON_PLUS = 9,
      BUTTON_LEFT_CLICK = 10,
      BUTTON_RIGHT_CLICK = 11,
      BUTTON_HOME = 12,
      BUTTON_CAPTURE = 13,
      BUTTON_EXTRA1 = 14,
      BUTTON_EXTRA2 = 15,
  };
  
  void setManualReportMode(bool m) {
      manualReport = m;
  }
  bool getManualReportMode() {
      return manualReport;
  }
  void safeSendReport() {
      if (!manualReport)
          sendReport();
  }
  void send() {
      sendReport();
  }
  

  void button(uint8_t b, bool val) {
    uint16_t mask = ((uint16_t)1 << b);

    if (val) {
          report.buttons |= mask;
    } else {
          report.buttons &= ~mask;
    }
    safeSendReport();
  }

  void buttons(uint16_t b) {
    report.buttons = b & 0x3FFF;
    safeSendReport();
  }

  void dpad(uint8_t value) {
    report.dpad = value & 0xF;
    safeSendReport();
  }

  void X(uint8_t v) {
    report.leftX = v;
    safeSendReport();
  }
  
  void Y(uint8_t v) {
    report.leftY = v;
    safeSendReport();
  }
  
  void XRight(uint8_t v) {
    report.rightX = v;
    safeSendReport();
  }
  
  void YRight(uint8_t v) {
    report.rightY = v;
    safeSendReport();
  }
  
  void begin() {
      USBComposite.setVendorId(0x0F0D);
      USBComposite.setProductId(0x00c1);
      USBComposite.setProductString("HORIPAD S");
      USBComposite.setManufacturerString("Omega Centauri Software");
      HID.begin(hidReportSwitchController);
  }
  
  void end() {
      HID.end();
  }

  HIDSwitchController(USBHID& HID) 
            : HIDReporter(HID, (uint8_t*)&report, sizeof(report)) {
        report.buttons = 0;
        report.dpad = DPAD_NEUTRAL;
        report.leftX = AXIS_NEUTRAL;
        report.leftY = AXIS_NEUTRAL;
        report.rightX = AXIS_NEUTRAL;
        report.rightY = AXIS_NEUTRAL;
        report.pad = 0;
    }
};

template<unsigned txSize,unsigned rxSize>class HIDRaw : public HIDReporter {
private:
    uint8_t txBuffer[txSize];
    uint8_t rxBuffer[HID_BUFFER_ALLOCATE_SIZE(rxSize,0)];
    HIDBuffer_t buf;
public:
	HIDRaw(USBHID& HID) : HIDReporter(HID, txBuffer, sizeof(txBuffer)) {}
	void begin(void) {
        buf.buffer = rxBuffer;
        buf.bufferSize = HID_BUFFER_SIZE(rxSize,0);
        buf.reportID = 0;
        HID.addOutputBuffer(&buf);
    }
	void end(void);
	void send(const uint8_t* data, unsigned n=sizeof(txBuffer)) {
        memset(txBuffer, 0, sizeof(txBuffer));
        memcpy(txBuffer, data, n>sizeof(txBuffer)?sizeof(txBuffer):n);
        sendReport();
    }
};



#endif
        		
