#include "USBComposite.h" 

//================================================================================
//================================================================================
//	Mouse

void HIDMouse::begin(void){
}

void HIDMouse::end(void){
}

void HIDMouse::click(uint8_t b)
{
	_buttons = b;
	move(0,0,0);
	_buttons = 0;
	move(0,0,0);
}

void HIDMouse::move(signed char x, signed char y, signed char wheel)
{
	reportBuffer[1] = _buttons;
	reportBuffer[2] = x;
	reportBuffer[3] = y;
	reportBuffer[4] = wheel;

    sendReport();
}

void HIDMouse::buttons(uint8_t b)
{
	if (b != _buttons)
	{
        _buttons = b;
		move(0,0,0);
	}
}

void HIDMouse::press(uint8_t b)
{
	buttons(_buttons | b);
}

void HIDMouse::release(uint8_t b)
{
	buttons(_buttons & ~b);
}

bool HIDMouse::isPressed(uint8_t b)
{
	if ((b & _buttons) > 0)
		return true;
	return false;
}

