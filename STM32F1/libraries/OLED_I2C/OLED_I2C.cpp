/*
  OLED_I2C.cpp - Arduino/chipKit library support for 128x64 pixel SSD1306 OLEDs
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

#include "OLED_I2C.h"
#if defined(__AVR__)
	#include <avr/pgmspace.h>
	#include "hardware/avr/HW_AVR.h"
#elif defined(__PIC32MX__)
	#pragma message("Compiling for PIC32 Architecture...")
	#include "hardware/pic32/HW_PIC32.h"
/*
#elif defined(__arm__)
	#pragma message("Compiling for ARM Architecture...")
	#include "hardware/arm/HW_ARM.h"
 */
#elif defined (__STM32F1__)
	#pragma message("Compiling for STM32F1 Architecture...")
#include "hardware/arm/HW_STM32.h"
#endif

OLED::OLED(uint8_t data_pin, uint8_t sclk_pin, uint8_t rst_pin)
{ 
	_sda_pin = data_pin;
	_scl_pin = sclk_pin;
	_rst_pin = rst_pin;
}

OLED::OLED(uint8_t data_pin, uint8_t sclk_pin)
{ 
	_sda_pin = data_pin;
	_scl_pin = sclk_pin;
	_rst_pin = RST_NOT_IN_USE;
}

void OLED::begin()
{
	if (_rst_pin != RST_NOT_IN_USE)
	{
		pinMode(_rst_pin, OUTPUT);
		digitalWrite(_rst_pin, HIGH);
		delay(1);
		digitalWrite(_rst_pin, LOW);
		delay(10);
		digitalWrite(_rst_pin, HIGH);
	}

#if defined(SDA1) & defined(SCL1)
	if (((_sda_pin == SDA) and (_scl_pin == SCL)) or ((_sda_pin == SDA1) and (_scl_pin == SCL1)))
#else
	if ((_sda_pin == SDA) and (_scl_pin == SCL))
#endif
	{
		_use_hw = true;
        #pragma message("I2C HW mode enabled")
		_initTWI();
	}
	else
	{
		_use_hw = false;
		pinMode(_scl_pin, OUTPUT);
        #pragma message("SCL pinmode OUTPUT")
	}

	_sendTWIcommand(SSD1306_DISPLAY_OFF);
    _sendTWIcommand(SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO);
    _sendTWIcommand(0x80);
    _sendTWIcommand(SSD1306_SET_MULTIPLEX_RATIO);
    _sendTWIcommand(0x3F);
    _sendTWIcommand(SSD1306_SET_DISPLAY_OFFSET);
    _sendTWIcommand(0x0);
    _sendTWIcommand(SSD1306_SET_START_LINE | 0x0);
    _sendTWIcommand(SSD1306_CHARGE_PUMP);
	_sendTWIcommand(0x14);
    _sendTWIcommand(SSD1306_MEMORY_ADDR_MODE);
    _sendTWIcommand(0x00);
    _sendTWIcommand(SSD1306_SET_SEGMENT_REMAP | 0x1);
    _sendTWIcommand(SSD1306_COM_SCAN_DIR_DEC);
    _sendTWIcommand(SSD1306_SET_COM_PINS);
    _sendTWIcommand(0x12);
    _sendTWIcommand(SSD1306_SET_CONTRAST_CONTROL);
	_sendTWIcommand(0xCF);
    _sendTWIcommand(SSD1306_SET_PRECHARGE_PERIOD);
	_sendTWIcommand(0xF1);
    _sendTWIcommand(SSD1306_SET_VCOM_DESELECT);
    _sendTWIcommand(0x40);
    _sendTWIcommand(SSD1306_DISPLAY_ALL_ON_RESUME);
    _sendTWIcommand(SSD1306_NORMAL_DISPLAY);
	_sendTWIcommand(SSD1306_DISPLAY_ON);

	clrScr();
	update();
	cfont.font=0;
}

void OLED::clrScr()
{
	memset(scrbuf, 0, 1024);
}

void OLED::fillScr()
{
	memset(scrbuf, 255, 1024);
}

void OLED::setBrightness(uint8_t value)
{
	_sendTWIcommand(SSD1306_SET_CONTRAST_CONTROL);
	_sendTWIcommand(value);
}

void OLED::invert(bool mode)
{
	if (mode==true)
		_sendTWIcommand(SSD1306_INVERT_DISPLAY);
	else
		_sendTWIcommand(SSD1306_NORMAL_DISPLAY);
}

void OLED::setPixel(uint16_t x, uint16_t y)
{
	int by, bi;

	if ((x>=0) and (x<128) and (y>=0) and (y<64))
	{
		by=((y/8)*128)+x;
		bi=y % 8;

		scrbuf[by]=scrbuf[by] | (1<<bi);
	}
}

void OLED::clrPixel(uint16_t x, uint16_t y)
{
	int by, bi;

	if ((x>=0) and (x<128) and (y>=0) and (y<64))
	{
		by=((y/8)*128)+x;
		bi=y % 8;

		scrbuf[by]=scrbuf[by] & ~(1<<bi);
	}
}

void OLED::invPixel(uint16_t x, uint16_t y)
{
	int by, bi;

	if ((x>=0) and (x<128) and (y>=0) and (y<64))
	{
		by=((y/8)*128)+x;
		bi=y % 8;

		if ((scrbuf[by] & (1<<bi))==0)
			scrbuf[by]=scrbuf[by] | (1<<bi);
		else
			scrbuf[by]=scrbuf[by] & ~(1<<bi);
	}
}

void OLED::invertText(bool mode)
{
	if (mode==true)
		cfont.inverted=1;
	else
		cfont.inverted=0;
}

void OLED::print(char *st, int x, int y)
{
	unsigned char ch;
	int stl;

	stl = strlen(st);
	if (x == RIGHT)
		x = 128-(stl*cfont.x_size);
	if (x == CENTER)
		x = (128-(stl*cfont.x_size))/2;

	for (int cnt=0; cnt<stl; cnt++)
			_print_char(*st++, x + (cnt*(cfont.x_size)), y);
}

void OLED::print(String st, int x, int y)
{
	char buf[st.length()+1];

	st.toCharArray(buf, st.length()+1);
	print(buf, x, y);
}

void OLED::printNumI(long num, int x, int y, int length, char filler)
{
	char buf[25];
	char st[27];
	boolean neg=false;
	int c=0, f=0;
  
	if (num==0)
	{
		if (length!=0)
		{
			for (c=0; c<(length-1); c++)
				st[c]=filler;
			st[c]=48;
			st[c+1]=0;
		}
		else
		{
			st[0]=48;
			st[1]=0;
		}
	}
	else
	{
		if (num<0)
		{
			neg=true;
			num=-num;
		}
	  
		while (num>0)
		{
			buf[c]=48+(num % 10);
			c++;
			num=(num-(num % 10))/10;
		}
		buf[c]=0;
	  
		if (neg)
		{
			st[0]=45;
		}
	  
		if (length>(c+neg))
		{
			for (int i=0; i<(length-c-neg); i++)
			{
				st[i+neg]=filler;
				f++;
			}
		}

		for (int i=0; i<c; i++)
		{
			st[i+neg+f]=buf[c-i-1];
		}
		st[c+neg+f]=0;

	}

	print(st,x,y);
}

void OLED::printNumF(double num, byte dec, int x, int y, char divider, int length, char filler)
{
	char st[27];
	boolean neg=false;

	if (num<0)
		neg = true;

	_convert_float(st, num, length, dec);

	if (divider != '.')
	{
		for (size_t i=0; i<sizeof(st); i++)
			if (st[i]=='.')
				st[i]=divider;
	}

	if (filler != ' ')
	{
		if (neg)
		{
			st[0]='-';
			for (size_t i=1; i<sizeof(st); i++)
				if ((st[i]==' ') || (st[i]=='-'))
					st[i]=filler;
		}
		else
		{
			for (size_t i=0; i<sizeof(st); i++)
				if (st[i]==' ')
					st[i]=filler;
		}
	}

	print(st,x,y);
}

void OLED::_print_char(unsigned char c, int x, int y)
{
	if ((cfont.y_size % 8) == 0)
	{
		int font_idx = ((c - cfont.offset)*(cfont.x_size*(cfont.y_size/8)))+4;
		for (int rowcnt=0; rowcnt<(cfont.y_size/8); rowcnt++)
		{
			for(int cnt=0; cnt<cfont.x_size; cnt++)
			{
				for (int b=0; b<8; b++)
					if ((fontbyte(font_idx+cnt+(rowcnt*cfont.x_size)) & (1<<b))!=0)
						if (cfont.inverted==0)
							setPixel(x+cnt, y+(rowcnt*8)+b);
						else
							clrPixel(x+cnt, y+(rowcnt*8)+b);
					else
						if (cfont.inverted==0)
							clrPixel(x+cnt, y+(rowcnt*8)+b);
						else
							setPixel(x+cnt, y+(rowcnt*8)+b);
			}
		}
	}
	else
	{
		int font_idx = ((c - cfont.offset)*((cfont.x_size*cfont.y_size/8)))+4;
		int cbyte=fontbyte(font_idx);
		int cbit=7;
		for (int cx=0; cx<cfont.x_size; cx++)
		{
			for (int cy=0; cy<cfont.y_size; cy++)
			{
				if ((cbyte & (1<<cbit)) != 0)
					if (cfont.inverted==0)
						setPixel(x+cx, y+cy);
					else
						clrPixel(x+cx, y+cy);
				else
					if (cfont.inverted==0)
						clrPixel(x+cx, y+cy);
					else
						setPixel(x+cx, y+cy);
				cbit--;
				if (cbit<0)
				{
					cbit=7;
					font_idx++;
					cbyte=fontbyte(font_idx);
				}
			}
		}
	}
}

void OLED::setFont(uint8_t* font)
{
	cfont.font=font;
	cfont.x_size=fontbyte(0);
	cfont.y_size=fontbyte(1);
	cfont.offset=fontbyte(2);
	cfont.numchars=fontbyte(3);
	cfont.inverted=0;
}

void OLED::drawHLine(int x, int y, int l)
{
	int by, bi;

	if ((x>=0) and (x<128) and (y>=0) and (y<64))
	{
		for (int cx=0; cx<l; cx++)
		{
			by=((y/8)*128)+x;
			bi=y % 8;

			scrbuf[by+cx] |= (1<<bi);
		}
	}
}

void OLED::clrHLine(int x, int y, int l)
{
	int by, bi;

	if ((x>=0) and (x<128) and (y>=0) and (y<64))
	{
		for (int cx=0; cx<l; cx++)
		{
			by=((y/8)*128)+x;
			bi=y % 8;

			scrbuf[by+cx] &= ~(1<<bi);
		}
	}
}

void OLED::drawVLine(int x, int y, int l)
{
	int by, bi;

	if ((x>=0) and (x<128) and (y>=0) and (y<64))
	{
		for (int cy=0; cy<l; cy++)
		{
			setPixel(x, y+cy);
		}
	}
}

void OLED::clrVLine(int x, int y, int l)
{
	int by, bi;

	if ((x>=0) and (x<128) and (y>=0) and (y<64))
	{
		for (int cy=0; cy<l; cy++)
		{
			clrPixel(x, y+cy);
		}
	}
}

void OLED::drawLine(int x1, int y1, int x2, int y2)
{
	int tmp;
	double delta, tx, ty;
	double m, b, dx, dy;
	
	if (((x2-x1)<0))
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
		tmp=y1;
		y1=y2;
		y2=tmp;
	}
    if (((y2-y1)<0))
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
		tmp=y1;
		y1=y2;
		y2=tmp;
	}

	if (y1==y2)
	{
		if (x1>x2)
		{
			tmp=x1;
			x1=x2;
			x2=tmp;
		}
		drawHLine(x1, y1, x2-x1);
	}
	else if (x1==x2)
	{
		if (y1>y2)
		{
			tmp=y1;
			y1=y2;
			y2=tmp;
		}
		drawVLine(x1, y1, y2-y1);
	}
	else if (abs(x2-x1)>abs(y2-y1))
	{
		delta=(double(y2-y1)/double(x2-x1));
		ty=double(y1);
		if (x1>x2)
		{
			for (int i=x1; i>=x2; i--)
			{
				setPixel(i, int(ty+0.5));
        		ty=ty-delta;
			}
		}
		else
		{
			for (int i=x1; i<=x2; i++)
			{
				setPixel(i, int(ty+0.5));
        		ty=ty+delta;
			}
		}
	}
	else
	{
		delta=(float(x2-x1)/float(y2-y1));
		tx=float(x1);
        if (y1>y2)
        {
			for (int i=y2+1; i>y1; i--)
			{
		 		setPixel(int(tx+0.5), i);
        		tx=tx+delta;
			}
        }
        else
        {
			for (int i=y1; i<y2+1; i++)
			{
		 		setPixel(int(tx+0.5), i);
        		tx=tx+delta;
			}
        }
	}

}

void OLED::clrLine(int x1, int y1, int x2, int y2)
{
	int tmp;
	double delta, tx, ty;
	double m, b, dx, dy;
	
	if (((x2-x1)<0))
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
		tmp=y1;
		y1=y2;
		y2=tmp;
	}
    if (((y2-y1)<0))
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
		tmp=y1;
		y1=y2;
		y2=tmp;
	}

	if (y1==y2)
	{
		if (x1>x2)
		{
			tmp=x1;
			x1=x2;
			x2=tmp;
		}
		clrHLine(x1, y1, x2-x1);
	}
	else if (x1==x2)
	{
		if (y1>y2)
		{
			tmp=y1;
			y1=y2;
			y2=tmp;
		}
		clrVLine(x1, y1, y2-y1);
	}
	else if (abs(x2-x1)>abs(y2-y1))
	{
		delta=(double(y2-y1)/double(x2-x1));
		ty=double(y1);
		if (x1>x2)
		{
			for (int i=x1; i>=x2; i--)
			{
				clrPixel(i, int(ty+0.5));
        		ty=ty-delta;
			}
		}
		else
		{
			for (int i=x1; i<=x2; i++)
			{
				clrPixel(i, int(ty+0.5));
        		ty=ty+delta;
			}
		}
	}
	else
	{
		delta=(float(x2-x1)/float(y2-y1));
		tx=float(x1);
        if (y1>y2)
        {
			for (int i=y2+1; i>y1; i--)
			{
		 		clrPixel(int(tx+0.5), i);
        		tx=tx+delta;
			}
        }
        else
        {
			for (int i=y1; i<y2+1; i++)
			{
		 		clrPixel(int(tx+0.5), i);
        		tx=tx+delta;
			}
        }
	}

}

void OLED::drawRect(int x1, int y1, int x2, int y2)
{
	int tmp;

	if (x1>x2)
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
	}
	if (y1>y2)
	{
		tmp=y1;
		y1=y2;
		y2=tmp;
	}

	drawHLine(x1, y1, x2-x1);
	drawHLine(x1, y2, x2-x1);
	drawVLine(x1, y1, y2-y1);
	drawVLine(x2, y1, y2-y1+1);
}

void OLED::clrRect(int x1, int y1, int x2, int y2)
{
	int tmp;

	if (x1>x2)
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
	}
	if (y1>y2)
	{
		tmp=y1;
		y1=y2;
		y2=tmp;
	}

	clrHLine(x1, y1, x2-x1);
	clrHLine(x1, y2, x2-x1);
	clrVLine(x1, y1, y2-y1);
	clrVLine(x2, y1, y2-y1+1);
}

void OLED::drawRoundRect(int x1, int y1, int x2, int y2)
{
	int tmp;

	if (x1>x2)
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
	}
	if (y1>y2)
	{
		tmp=y1;
		y1=y2;
		y2=tmp;
	}
	if ((x2-x1)>4 && (y2-y1)>4)
	{
		setPixel(x1+1,y1+1);
		setPixel(x2-1,y1+1);
		setPixel(x1+1,y2-1);
		setPixel(x2-1,y2-1);
		drawHLine(x1+2, y1, x2-x1-3);
		drawHLine(x1+2, y2, x2-x1-3);
		drawVLine(x1, y1+2, y2-y1-3);
		drawVLine(x2, y1+2, y2-y1-3);
	}
}

void OLED::clrRoundRect(int x1, int y1, int x2, int y2)
{
	int tmp;

	if (x1>x2)
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
	}
	if (y1>y2)
	{
		tmp=y1;
		y1=y2;
		y2=tmp;
	}
	if ((x2-x1)>4 && (y2-y1)>4)
	{
		clrPixel(x1+1,y1+1);
		clrPixel(x2-1,y1+1);
		clrPixel(x1+1,y2-1);
		clrPixel(x2-1,y2-1);
		clrHLine(x1+2, y1, x2-x1-3);
		clrHLine(x1+2, y2, x2-x1-3);
		clrVLine(x1, y1+2, y2-y1-3);
		clrVLine(x2, y1+2, y2-y1-3);
	}
}

void OLED::drawCircle(int x, int y, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;
	char ch, cl;
	
	setPixel(x, y + radius);
	setPixel(x, y - radius);
	setPixel(x + radius, y);
	setPixel(x - radius, y);
 
	while(x1 < y1)
	{
		if(f >= 0) 
		{
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;    
		setPixel(x + x1, y + y1);
		setPixel(x - x1, y + y1);
		setPixel(x + x1, y - y1);
		setPixel(x - x1, y - y1);
		setPixel(x + y1, y + x1);
		setPixel(x - y1, y + x1);
		setPixel(x + y1, y - x1);
		setPixel(x - y1, y - x1);
	}
}

void OLED::clrCircle(int x, int y, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;
	char ch, cl;
	
	clrPixel(x, y + radius);
	clrPixel(x, y - radius);
	clrPixel(x + radius, y);
	clrPixel(x - radius, y);
 
	while(x1 < y1)
	{
		if(f >= 0) 
		{
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;    
		clrPixel(x + x1, y + y1);
		clrPixel(x - x1, y + y1);
		clrPixel(x + x1, y - y1);
		clrPixel(x - x1, y - y1);
		clrPixel(x + y1, y + x1);
		clrPixel(x - y1, y + x1);
		clrPixel(x + y1, y - x1);
		clrPixel(x - y1, y - x1);
	}
}

void OLED::drawBitmap(int x, int y, uint8_t* bitmap, int sx, int sy)
{
	int bit;
	byte data;

	for (int cy=0; cy<sy; cy++)
	{
		bit= cy % 8;
		for(int cx=0; cx<sx; cx++)
		{
			data=bitmapbyte(cx+((cy/8)*sx));
			if ((data & (1<<bit))>0)
				setPixel(x+cx, y+cy);
			else
				clrPixel(x+cx, y+cy);
		}
	}      
}

// Private

void OLED::_sendStart(byte addr)
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_sda_pin, HIGH);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, LOW);
	shiftOut(_sda_pin, _scl_pin, MSBFIRST, addr);
}

void OLED::_sendStop()
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_sda_pin, HIGH);
	pinMode(_sda_pin, INPUT);
}

void OLED::_sendNack()
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_scl_pin, LOW);
	digitalWrite(_sda_pin, HIGH);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_scl_pin, LOW);
	pinMode(_sda_pin, INPUT);
}

void OLED::_sendAck()
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_scl_pin, LOW);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_scl_pin, LOW);
	pinMode(_sda_pin, INPUT);
}

void OLED::_waitForAck()
{
	pinMode(_sda_pin, INPUT);
	digitalWrite(_scl_pin, HIGH);
	while (digitalRead(_sda_pin)==HIGH) {}
	digitalWrite(_scl_pin, LOW);
}

void OLED::_writeByte(uint8_t value)
{
	pinMode(_sda_pin, OUTPUT);
	shiftOut(_sda_pin, _scl_pin, MSBFIRST, value);
}

