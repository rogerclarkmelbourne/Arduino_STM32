/* Copyright (c) 2011, Peter Barrett
 * Copyright (c) 2017-2018, Alexander Pruss
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

#include "USBComposite.h" 

#include <string.h>
#include <stdint.h>
#include <libmaple/nvic.h>
#include "usb_hid.h"
#include "usb_composite_serial.h"
#include "usb_generic.h"
#include <libmaple/usb.h>
#include <string.h>
#include <libmaple/iwdg.h>

/*
 * USB HID interface
 */

bool USBHID::init(USBHID* me) {
    usb_hid_setTXEPSize(me->txPacketSize);
    
    HIDReporter* r = me->profiles;
    
    if (me->baseChunk.data != NULL) {
        /* user set an explicit report for USBHID */
        while (r != NULL) {
            r->reportID = r->userSuppliedReportID;
            if (r->reportID != 0)
                r->reportBuffer[0] = r->reportID;
            r = r->next;
        }
        usb_hid_set_report_descriptor(&(me->baseChunk));
    }
    else {
        uint8 reportID = HID_AUTO_REPORT_ID_START;
        
        while (r != NULL) {
            if (r->forceUserSuppliedReportID) 
                r->reportID = r->userSuppliedReportID;
            else 
                r->reportID = reportID++;
            
            if (r->reportID != 0) 
                r->reportBuffer[0] = r->reportID;
            
            r = r->next;
        }
        usb_hid_set_report_descriptor(me->chunkList);
    }

	return true;
}

bool USBHID::registerComponent() {
	return USBComposite.add(&usbHIDPart, this, (USBPartInitializer)&USBHID::init);
}

void USBHID::setReportDescriptor(const uint8_t* report_descriptor, uint16_t report_descriptor_length) {
    baseChunk.dataLength = report_descriptor_length;
    baseChunk.data = report_descriptor;
    baseChunk.next = NULL;
}

void USBHID::clear() {
    clearBuffers();
    baseChunk.data = NULL;
    baseChunk.dataLength = 0;
    baseChunk.next = NULL;
    chunkList = NULL;
    profiles = NULL;
}

void USBHID::addReport(HIDReporter* r, bool always) {
    if (! always && ! autoRegister)
        return;
    
    r->next = NULL;
    if (profiles == NULL) {
        profiles = r;
    }
    else {
        HIDReporter* tail = profiles;
        while (tail != r && tail->next != NULL) {
            tail = tail->next;
        }
        if (tail == r)
            return;
        tail->next = r;
    }

    struct usb_chunk* chunkTail = chunkList;
    
    if (chunkTail == NULL) {
        chunkList = &(r->reportChunks[0]);
        chunkTail = chunkList;
    }
    else {
        while (chunkTail->next != NULL)
            chunkTail = chunkTail->next;
        chunkTail->next = &(r->reportChunks[0]);
        chunkTail = chunkTail->next;
    }
    chunkTail->next = &(r->reportChunks[1]);
    chunkTail = chunkTail->next;
    chunkTail->next = &(r->reportChunks[2]);
    chunkTail = chunkTail->next;
    chunkTail->next = NULL;
}

void USBHID::setReportDescriptor(const HIDReportDescriptor* report) {
    if (report == NULL)
        setReportDescriptor(NULL, 0);
    else
        setReportDescriptor(report->descriptor, report->length);
}

void USBHID::begin(const uint8_t* report_descriptor, uint16_t report_descriptor_length) {            
	if (enabledHID)
		return;
	
	setReportDescriptor(report_descriptor, report_descriptor_length);
	
	USBComposite.clear();
	registerComponent();

	USBComposite.begin(); 
	
	enabledHID = true;
}

void USBHID::begin(const HIDReportDescriptor* report) {
    if (report == NULL)
        begin(NULL, 0);
    else
        begin(report->descriptor, report->length);
}

void USBHID::setBuffers(uint8_t type, volatile HIDBuffer_t* fb, int count) {
    usb_hid_set_buffers(type, fb, count);
}

bool USBHID::addBuffer(uint8_t type, volatile HIDBuffer_t* buffer) {
    return 0 != usb_hid_add_buffer(type, buffer);
}

void USBHID::clearBuffers(uint8_t type) {
	usb_hid_clear_buffers(type);
}

void USBHID::clearBuffers() {
	clearBuffers(HID_REPORT_TYPE_OUTPUT);
	clearBuffers(HID_REPORT_TYPE_FEATURE);
}

void USBHID::end(void){
	if(enabledHID) {
		USBComposite.end();
		enabledHID = false;
	}
}

void USBHID::begin(USBCompositeSerial serial, const uint8_t* report_descriptor, uint16_t report_descriptor_length) {	
	if (enabledHID)
		return;
	
	USBComposite.clear();

	setReportDescriptor(report_descriptor, report_descriptor_length);
	registerComponent();

	serial.registerComponent();

	USBComposite.begin();
}
		
void USBHID::begin(USBCompositeSerial serial, const HIDReportDescriptor* report) {
    if (report == NULL)
        begin(serial, NULL, 0);
    else
        begin(serial, report->descriptor, report->length);
}

void HIDReporter::sendReport() {
    unsigned toSend = bufferSize;
    uint8* b = reportBuffer;
    
    while (toSend) {
        unsigned delta = usb_hid_tx(b, toSend);
        toSend -= delta;
        b += delta;
    }
    
    /* flush out to avoid having the pc wait for more data */
    /* TODO: remove? this does nothing right now! */
    usb_hid_tx(NULL, 0);
}

void HIDReporter::registerProfile(bool always) {
    for (uint32 i=0; i<3; i++) {
        reportChunks[i].data = NULL;
        reportChunks[i].dataLength = 0;
    }
    if (reportDescriptor.descriptor != NULL) {
        int32_t reportIDOffset = -1;
        if (! forceUserSuppliedReportID) {
            uint32_t i = 0;
            
            /*
             * parse bits of beginning of report looking for a single-byte report ID (0x85 ID)
             */
            while (i < reportDescriptor.length && reportIDOffset < 0) {
                if (reportDescriptor.descriptor[i]==0x85) {
                    if (i+1 < reportDescriptor.length)
                        reportIDOffset = i+1;
                }
                // the lowest 2 bit determine the data length of the tag
                if ((reportDescriptor.descriptor[i] & 3) == 3)
                    i += 5;
                else 
                    i += 1 + (reportDescriptor.descriptor[i] & 3);
            }
            if (i >= reportDescriptor.length) {
                forceUserSuppliedReportID = true;
            }
        }
        if (forceUserSuppliedReportID) {
            reportChunks[0].data = reportDescriptor.descriptor;
            reportChunks[0].dataLength = reportDescriptor.length;
        }
        else {
            reportChunks[0].data = reportDescriptor.descriptor;
            reportChunks[0].dataLength = reportIDOffset;
            reportID = reportDescriptor.descriptor[reportIDOffset];
            reportChunks[1].data = &(reportID);
            reportChunks[1].dataLength = 1;
            reportChunks[2].data = reportDescriptor.descriptor+reportIDOffset+1;
            reportChunks[2].dataLength = reportDescriptor.length - (reportIDOffset+1);
        }
    }
    HID.addReport(this, always);
}
        
HIDReporter::HIDReporter(USBHID& _HID, const HIDReportDescriptor* r, uint8_t* _buffer, unsigned _size, uint8_t _reportID, bool forceReportID) : HID(_HID) {
    if (r != NULL) {
        memcpy(&reportDescriptor, r, sizeof(reportDescriptor));
    }
    else {
        reportDescriptor.descriptor = NULL;
        reportDescriptor.length = 0;
    }

    if (_reportID == 0) {
        reportBuffer = _buffer+1;
        bufferSize = _size-1;
    }
    else {
        reportBuffer = _buffer;
        bufferSize = _size;
    }
    memset(reportBuffer, 0, bufferSize);
    userSuppliedReportID = _reportID;

    if (_size > 0 && _reportID != 0 && ! forceReportID) {
        reportBuffer[0] = _reportID;
        forceUserSuppliedReportID = false;
    }
    else {
        forceUserSuppliedReportID = true;
    }
    
    registerProfile(false);
}

HIDReporter::HIDReporter(USBHID& _HID, const HIDReportDescriptor* r, uint8_t* _buffer, unsigned _size) : HID(_HID) {
    if (r != NULL) {
        memcpy(&reportDescriptor, r, sizeof(reportDescriptor));
    }
    else {
        reportDescriptor.descriptor = NULL;
        reportDescriptor.length = 0;
    }

    reportBuffer = _buffer;
    bufferSize = _size;
    memset(_buffer, 0, _size);
    userSuppliedReportID = 0;
    forceUserSuppliedReportID = true;

    registerProfile(false);
}

void HIDReporter::setFeature(uint8_t* in) {
    return usb_hid_set_feature(reportID, in);
}

uint16_t HIDReporter::getData(uint8_t type, uint8_t* out, uint8_t poll) {
    return usb_hid_get_data(type, reportID, out, poll);
}

uint16_t HIDReporter::getFeature(uint8_t* out, uint8_t poll) {
    return usb_hid_get_data(HID_REPORT_TYPE_FEATURE, reportID, out, poll);
}

uint16_t HIDReporter::getOutput(uint8_t* out, uint8_t poll) {
    return usb_hid_get_data(HID_REPORT_TYPE_OUTPUT, reportID, out, poll);
}

