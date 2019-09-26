#include "USBComposite.h"

//================================================================================
//================================================================================
//	Mouse

void HIDAbsMouse::begin(void){
}

void HIDAbsMouse::end(void){
}

void HIDAbsMouse::click(uint8_t b)
{
    report.wheel = 0;
	report.buttons = b;
    sendReport();
	report.buttons = 0;
    sendReport();
}

void HIDAbsMouse::move(int16 x, int16 y, int8 wheel)
{
    report.x = x;
    report.y = y;
    report.wheel = wheel;

    sendReport();
}

void HIDAbsMouse::buttons(uint8_t b)
{
	if (b != report.buttons)
	{
        report.wheel = 0;
        report.buttons = b;
        sendReport();
	}
}

void HIDAbsMouse::press(uint8_t b)
{
	buttons(report.buttons | b);
}

void HIDAbsMouse::release(uint8_t b)
{
	buttons(report.buttons & ~b);
}

bool HIDAbsMouse::isPressed(uint8_t b)
{
	if ((b & report.buttons) != 0)
		return true;
	return false;
}
