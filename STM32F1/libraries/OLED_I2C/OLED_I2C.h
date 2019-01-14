/*
  OLED_I2C.h - Arduino/chipKit library support for 128x64 pixel SSD1306 OLEDs
  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
  
  This library has been made to make it easy to use 128x64 pixel OLED displays
  based on the SSD1306 controller chip with an Arduino or a chipKit.

  You can always find the latest version of the library at 
  http://www.RinkyDinkElectronics.com/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.
*/

#ifndef OLED_I2C_h
#define OLED_I2C_h

#define SSD1306_ADDR		0x3C

#define LEFT	0
#define RIGHT	9999
#define CENTER	9998

#define SSD1306_COMMAND			0x00
#define SSD1306_DATA			0xC0
#define SSD1306_DATA_CONTINUE	0x40

#define RST_NOT_IN_USE	255

// SSD1306 Commandset
// ------------------
// Fundamental Commands
#define SSD1306_SET_CONTRAST_CONTROL					0x81
#define SSD1306_DISPLAY_ALL_ON_RESUME					0xA4
#define SSD1306_DISPLAY_ALL_ON							0xA5
#define SSD1306_NORMAL_DISPLAY							0xA6
#define SSD1306_INVERT_DISPLAY							0xA7
#define SSD1306_DISPLAY_OFF								0xAE
#define SSD1306_DISPLAY_ON								0xAF
#define SSD1306_NOP										0xE3
// Scrolling Commands
#define SSD1306_HORIZONTAL_SCROLL_RIGHT					0x26
#define SSD1306_HORIZONTAL_SCROLL_LEFT					0x27
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_RIGHT	0x29
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_LEFT		0x2A
#define SSD1306_DEACTIVATE_SCROLL						0x2E
#define SSD1306_ACTIVATE_SCROLL							0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA				0xA3
// Addressing Setting Commands
#define SSD1306_SET_LOWER_COLUMN						0x00
#define SSD1306_SET_HIGHER_COLUMN						0x10
#define SSD1306_MEMORY_ADDR_MODE						0x20
#define SSD1306_SET_COLUMN_ADDR							0x21
#define SSD1306_SET_PAGE_ADDR							0x22
// Hardware Configuration Commands
#define SSD1306_SET_START_LINE							0x40
#define SSD1306_SET_SEGMENT_REMAP						0xA0
#define SSD1306_SET_MULTIPLEX_RATIO						0xA8
#define SSD1306_COM_SCAN_DIR_INC						0xC0
#define SSD1306_COM_SCAN_DIR_DEC						0xC8
#define SSD1306_SET_DISPLAY_OFFSET						0xD3
#define SSD1306_SET_COM_PINS							0xDA
#define SSD1306_CHARGE_PUMP								0x8D
// Timing & Driving Scheme Setting Commands
#define SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO				0xD5
#define SSD1306_SET_PRECHARGE_PERIOD					0xD9
#define SSD1306_SET_VCOM_DESELECT						0xDB

#if defined(__AVR__)
	#include "Arduino.h"
	#include "hardware/avr/HW_AVR_defines.h"
#elif defined(__PIC32MX__)
	#include "WProgram.h"
	#include "hardware/pic32/HW_PIC32_defines.h"
/*
#elif defined(__arm__)
	#include "Arduino.h"
	#include "hardware/arm/HW_ARM_defines.h"
 */
#elif defined (__STM32F1__)
#include "Arduino.h"
//#include <Wire.h>
#include "hardware/arm/HW_STM32_defines.h"
#endif

struct _current_font
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
	uint8_t inverted;
};

class OLED
{
	public:
		OLED(uint8_t data_pin, uint8_t sclk_pin, uint8_t rst_pin);
		OLED(uint8_t data_pin, uint8_t sclk_pin);
		void	begin();
		void	update();
		void	setBrightness(uint8_t value);
		void	clrScr();
		void	fillScr();
		void	invert(bool mode);
		void	setPixel(uint16_t x, uint16_t y);
		void	clrPixel(uint16_t x, uint16_t y);
		void	invPixel(uint16_t x, uint16_t y);
		void	invertText(bool mode);
		void	print(char *st, int x, int y);
		void	print(String st, int x, int y);
		void	printNumI(long num, int x, int y, int length=0, char filler=' ');
		void	printNumF(double num, byte dec, int x, int y, char divider='.', int length=0, char filler=' ');
		void	setFont(uint8_t* font);
		void	drawBitmap(int x, int y, uint8_t* bitmap, int sx, int sy);
		void	drawLine(int x1, int y1, int x2, int y2);
		void	clrLine(int x1, int y1, int x2, int y2);
		void	drawRect(int x1, int y1, int x2, int y2);
		void	clrRect(int x1, int y1, int x2, int y2);
		void	drawRoundRect(int x1, int y1, int x2, int y2);
		void	clrRoundRect(int x1, int y1, int x2, int y2);
		void	drawCircle(int x, int y, int radius);
		void	clrCircle(int x, int y, int radius);

	protected:
		uint8_t			_scl_pin, _sda_pin, _rst_pin;
		boolean			_use_hw;
		_current_font	cfont;
		uint8_t			scrbuf[1024];

		void	_print_char(unsigned char c, int x, int row);
		void	_convert_float(char *buf, double num, int width, byte prec);
		void	drawHLine(int x, int y, int l);
		void	clrHLine(int x, int y, int l);
		void	drawVLine(int x, int y, int l);
		void	clrVLine(int x, int y, int l);

		void	_initTWI();
		void	_sendTWIstart();
		void	_sendTWIstop();
		void	_sendTWIcommand(uint8_t value);
		void	_sendStart(byte addr);
		void	_sendStop();
		void	_sendAck();
		void	_sendNack();
		void	_waitForAck();
		void	_writeByte(uint8_t value);
#if defined(__arm__)
	//	Twi		*twi;
#endif
};

#endif