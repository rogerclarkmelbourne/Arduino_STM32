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

class USBXBox360 {
private:
	uint8_t xbox360_Report[20] = {0,0x14};//    3,0,0,0,0,0x0F,0x20,0x80,0x00,0x02,0x08,0x20,0x80};
    bool manualReport = false;
    bool enabled;
	void safeSendReport(void);
	void sendReport(void);
    bool wait(void);
public:
	void send(void);
	static bool init(void* ignore);
	bool registerComponent();
	void stop();
    void setManualReportMode(bool manualReport);
    bool getManualReportMode();
	void begin(void);
	void end(void);
	void button(uint8_t button, bool val);
	void X(int16_t val);
	void Y(int16_t val);
	void position(int16_t x, int16_t y);
	void positionRight(int16_t x, int16_t y);
	void XRight(int16_t val);
	void YRight(int16_t val);
	void sliderLeft(uint8_t val);
	void sliderRight(uint8_t val);
	void hat(int16_t dir);
    void setLEDCallback(void (*callback)(uint8 pattern));
    void setRumbleCallback(void (*callback)(uint8 left, uint8 right));
};

#endif

