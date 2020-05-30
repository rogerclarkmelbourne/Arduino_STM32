#include "USBComposite.h"

//================================================================================
//================================================================================
//	Mouse

void HIDDigitizer::begin(void){
}

void HIDDigitizer::end(void){
}

void HIDDigitizer::move(uint16 x, uint16 y)
{
    report.x = x;
    report.y = y;

    sendReport();
}

void HIDDigitizer::buttons(uint8_t b)
{
	if (b != report.buttons)
	{
        report.buttons = b;
        sendReport();
	}
}

void HIDDigitizer::click(uint8_t b)
{
    press(b);
    release(b);
}

void HIDDigitizer::press(uint8_t b)
{
	buttons(report.buttons | b);
}

void HIDDigitizer::release(uint8_t b)
{
	buttons(report.buttons & ~b);
}

bool HIDDigitizer::isPressed(uint8_t b)
{
	if ((b & report.buttons) == b)
		return true;
	return false;
}
