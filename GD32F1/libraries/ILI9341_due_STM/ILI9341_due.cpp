/*
ILI9341_due_.cpp - Arduino Due library for interfacing with ILI9341-based TFTs

Copyright (c) 2014  Marek Buriak
Modified 03/19/2015 by Victor Perez to add STM32F1 Maple mini DMA support

This library is based on ILI9341_t3 library from Paul Stoffregen
(https://github.com/PaulStoffregen/ILI9341_t3), Adafruit_ILI9341
and Adafruit_GFX libraries from Limor Fried/Ladyada
(https://github.com/adafruit/Adafruit_ILI9341).

This file is part of the Arduino ILI9341_due library.
Sources for this library can be found at https://github.com/marekburiak/ILI9341_Due.

ILI9341_due is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

ILI9341_due is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with ILI9341_due.  If not, see <http://www.gnu.org/licenses/>.

*/

/***************************************************
This is our library for the Adafruit ILI9341 Breakout and Shield
----> http://www.adafruit.com/products/1651

Check out the links above for our tutorials and wiring diagrams
These displays use SPI to communicate, 4 or 5 pins are required to
interface (RST is optional)
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution
****************************************************/

#include "ILI9341_due.h"
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
#include <SPI.h>
#elif SPI_MODE_DMA
#include "ILI_SdSpi.h"
#include <libmaple/dma.h>
#endif
//#include "..\Streaming\Streaming.h"


static const uint8_t init_commands[] PROGMEM = {
	4, 0xEF, 0x03, 0x80, 0x02,
	4, 0xCF, 0x00, 0XC1, 0X30,
	5, 0xED, 0x64, 0x03, 0X12, 0X81,
	4, 0xE8, 0x85, 0x00, 0x78,
	6, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
	2, 0xF7, 0x20,
	3, 0xEA, 0x00, 0x00,
	2, ILI9341_PWCTR1, 0x23, // Power control
	2, ILI9341_PWCTR2, 0x10, // Power control
	3, ILI9341_VMCTR1, 0x3e, 0x28, // VCM control
	2, ILI9341_VMCTR2, 0x86, // VCM control2
	2, ILI9341_MADCTL, 0x48, // Memory Access Control
	2, ILI9341_PIXFMT, 0x55,
	3, ILI9341_FRMCTR1, 0x00, 0x18,
	4, ILI9341_DFUNCTR, 0x08, 0x82, 0x27, // Display Function Control
	2, 0xF2, 0x00, // Gamma Function Disable
	2, ILI9341_GAMMASET, 0x01, // Gamma curve selected
	16, ILI9341_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
	0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
	16, ILI9341_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
	0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
	0
};


ILI9341_due::ILI9341_due(uint8_t cs, uint8_t dc, uint8_t rst)
{
	_cs = cs;
	_dc = dc;
	_rst = rst;
	_width = ILI9341_TFTWIDTH;
	_height = ILI9341_TFTHEIGHT;
	_rotation = iliRotation0;
#ifdef FEATURE_PRINT_ENABLED
	_cursorY = _cursorX = 0;
	_textsize = 1;
	_textcolor = _textbgcolor = 0xFFFF;
	_wrap = true;
#endif
#ifdef FEATURE_ARC_ENABLED
	_arcAngleMax = ARC_ANGLE_MAX;
	_arcAngleOffset = ARC_ANGLE_OFFSET;
#endif
}


bool ILI9341_due::begin(void)
{
	if (pinIsChipSelect(_cs)) {
		pinMode(_dc, OUTPUT);
		_dcport = portOutputRegister(digitalPinToPort(_dc));
		_dcpinmask = digitalPinToBitMask(_dc);

#if SPI_MODE_NORMAL | SPI_MODE_DMA
		pinMode(_cs, OUTPUT);
		_csport = portOutputRegister(digitalPinToPort(_cs));
		_cspinmask = digitalPinToBitMask(_cs);
#endif

#if SPI_MODE_NORMAL
		SPI.begin();
		SPI.setClockDivider(ILI9341_SPI_CLKDIVIDER);
		SPI.setBitOrder(MSBFIRST);
		SPI.setDataMode(SPI_MODE0);
#ifdef __AVR__
		_SPCR = SPCR;
#endif
#elif SPI_MODE_EXTENDED
		SPI.begin(_cs);
		SPI.setClockDivider(_cs, ILI9341_SPI_CLKDIVIDER);
		SPI.setBitOrder(_cs, MSBFIRST);
		SPI.setDataMode(_cs, SPI_MODE0);
#elif SPI_MODE_DMA
		_dmaSpi.begin();
		_dmaSpi.init(ILI9341_SPI_CLKDIVIDER);
#endif

		// toggle RST low to reset
		if (_rst < 255) {
			pinMode(_rst, OUTPUT);
			digitalWrite(_rst, HIGH);
			delay(5);
			digitalWrite(_rst, LOW);
			delay(20);
			digitalWrite(_rst, HIGH);
			delay(150);
		}

		const uint8_t *addr = init_commands;
		while (1) {
			uint8_t count = pgm_read_byte(addr++);
			if (count-- == 0) break;
			writecommand_cont(pgm_read_byte(addr++));
			while (count-- > 0) {
				writedata8_cont(pgm_read_byte(addr++));
			}
		}

		writecommand_last(ILI9341_SLPOUT);    // Exit Sleep
		delay(120);
		writecommand_last(ILI9341_DISPON);    // Display on
		delay(120);
		_isInSleep = _isIdle = false;

		uint8_t x = readcommand8(ILI9341_RDMODE);
		Serial.print(F("\nDisplay Power Mode: 0x")); Serial.println(x, HEX);
		x = readcommand8(ILI9341_RDMADCTL);
		Serial.print(F("\nMADCTL Mode: 0x")); Serial.println(x, HEX);
		x = readcommand8(ILI9341_RDPIXFMT);
		Serial.print(F("\nPixel Format: 0x")); Serial.println(x, HEX);
		x = readcommand8(ILI9341_RDIMGFMT);
		Serial.print(F("\nImage Format: 0x")); Serial.println(x, HEX);
		x = readcommand8(ILI9341_RDSELFDIAG);
		Serial.print(F("\nSelf Diagnostic: 0x")); Serial.println(x, HEX);

		return true;
	}
	else {
		return false;
	}
}

bool ILI9341_due::pinIsChipSelect(uint8_t cs)
{
#if SPI_MODE_EXTENDED
	if(cs == 4 || cs == 10 || cs == 52)	// in Extended SPI mode only these pins are valid
	{
		return true;
	}
	else
	{
		Serial.print("Pin ");
		Serial.print(_cs);
		Serial.println(" is not a valid Chip Select pin for SPI Extended Mode. Valid pins are 4, 10, 52");
		return false;
	}
#elif SPI_MODE_NORMAL | SPI_MODE_DMA
	return true;
#endif
}

void ILI9341_due::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	setAddrAndRW_cont(x0, y0, x1, y1);
	disableCS();
}

void ILI9341_due::setSPIClockDivider(uint8_t divider)
{
#if SPI_MODE_NORMAL
	SPI.setClockDivider(divider);
#elif SPI_MODE_EXTENDED
	SPI.setClockDivider(_cs, divider);
#elif SPI_MODE_DMA
	_dmaSpi.init(divider);
#endif
}

void ILI9341_due::pushColor(uint16_t color)
{
	enableCS();
	writedata16_last(color);
}

void ILI9341_due::pushColors(uint16_t *colors, uint16_t offset, uint16_t len) {
	enableCS();
	setDCForData();
	colors = colors + offset * 2;
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
	for (uint16_t i = 0; i < len; i++) {
		write16_cont(colors[i]);
	}
#elif SPI_MODE_DMA
	for (uint16_t i = 0; i < (len << 1); i += 2) {
		uint16_t color = *colors;
		_scanlineBuffer[i] = highByte(color);
		_scanlineBuffer[i + 1] = lowByte(color);
		colors++;
	}
	writeScanline_cont(len);
#endif
	disableCS();
}

// pushes pixels stored in the colors array (one color is 2 bytes) 
// in big endian (high byte first)
// len should be the length of the array (so to push 320 pixels,
// you have to have a 640-byte array and len should be 640)
void ILI9341_due::pushColors565(uint8_t *colors, uint16_t offset, uint16_t len) {
	enableCS();
	setDCForData();
	colors = colors + offset;

#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
	for (uint16_t i = 0; i < len; i++) {
		write8_cont(colors[i]);
	}
#elif SPI_MODE_DMA
	write_cont(colors, len);
#endif
	disableCS();
}


void ILI9341_due::drawPixel(int16_t x, int16_t y, uint16_t color) {

	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
	writePixel_last(x, y, color);
}

void ILI9341_due::drawPixel_cont(int16_t x, int16_t y, uint16_t color) {

	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
	writePixel_cont(x, y, color);
}

void ILI9341_due::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	// Rudimentary clipping
	if ((x >= _width) || (y >= _height || h == 0)) return;
	if ((y + h - 1) >= _height) h = _height - y;

	setAddrAndRW_cont(x, y, x, y + h - 1);
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
	setDCForData();
	while (h-- > 1) {
		write16_cont(color);
	}
	write16_last(color);
#elif SPI_MODE_DMA
	fillScanline(color, h);
	writeScanline_last(h);
#endif
}

void ILI9341_due::drawFastVLine_cont_noFill(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	// Rudimentary clipping
	if ((x >= _width) || (y >= _height || h == 0)) return;
	if ((y + h - 1) >= _height) h = _height - y;

	setAddrAndRW_cont(x, y, x, y + h - 1);
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
	setDCForData();
	while (h-- > 0) {
		write16_cont(color);
	}
#elif SPI_MODE_DMA
	writeScanline_cont(h);
#endif
}

void ILI9341_due::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	// Rudimentary clipping
	if ((x >= _width) || (y >= _height || w == 0)) return;
	if ((x + w - 1) >= _width)  w = _width - x;

	setAddrAndRW_cont(x, y, x + w - 1, y);
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
	setDCForData();
	while (w-- > 1) {
		write16_cont(color);
	}
	writedata16_last(color);
#elif SPI_MODE_DMA
	fillScanline(color, w);
	writeScanline_last(w);
#endif
}

void ILI9341_due::fillScreen(uint16_t color)
{
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
	fillRect(0, 0, _width, _height, color);
#elif SPI_MODE_DMA

	fillScanline(color);

	setAddrAndRW_cont(0, 0, _width - 1, _height - 1);
	setDCForData();
	const uint16_t bytesToWrite = _width << 1;
	for (uint16_t y = 0; y < _height; y++)
	{
		write_cont(_scanlineBuffer, bytesToWrite);
	}
	disableCS();

#endif
}

// fill a rectangle
void ILI9341_due::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	//Serial << "x:" << x << " y:" << y << " w:" << x << " h:" << h << " width:" << _width << " height:" << _height <<endl;
	// rudimentary clipping (drawChar w/big text requires this)
	if ((x >= _width) || (y >= _height || w == 0 || h == 0)) return;
	if ((x + w - 1) >= _width)  w = _width - x;
	if ((y + h - 1) >= _height) h = _height - y;

	setAddrAndRW_cont(x, y, x + w - 1, y + h - 1);
#if SPI_MODE_DMA
	const uint16_t maxNumLinesInScanlineBuffer = (SCANLINE_BUFFER_SIZE >> 1) / w;
	const uint16_t numPixelsInOneGo = w*maxNumLinesInScanlineBuffer;

	fillScanline(color, numPixelsInOneGo);

	for (uint16_t p = 0; p < h / maxNumLinesInScanlineBuffer; p++)
	{
		writeScanline_cont(numPixelsInOneGo);
	}
	writeScanline_last((w*h) % numPixelsInOneGo);

#elif SPI_MODE_NORMAL | SPI_MODE_EXTENDED
	// TODO: this can result in a very long transaction time
	// should break this into multiple transactions, even though
	// it'll cost more overhead, so we don't stall other SPI libs
	//enableCS();	//setAddrAndRW_cont will enable CS 
	setDCForData();

	y=h;
	while(y--) {
		x=w;
		while(x--)
		{
			write16_cont(color);
		}
	}

	disableCS();
#endif
}


#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

void ILI9341_due::setRotation(iliRotation r)
{
	writecommand_cont(ILI9341_MADCTL);
	_rotation = r;
	switch (r) {
	case iliRotation0:
		writedata8_last(MADCTL_MX | MADCTL_BGR);
		_width = ILI9341_TFTWIDTH;
		_height = ILI9341_TFTHEIGHT;
		break;
	case iliRotation90:
		writedata8_last(MADCTL_MV | MADCTL_BGR);
		_width = ILI9341_TFTHEIGHT;
		_height = ILI9341_TFTWIDTH;
		break;
	case iliRotation180:
		writedata8_last(MADCTL_MY | MADCTL_BGR);
		_width = ILI9341_TFTWIDTH;
		_height = ILI9341_TFTHEIGHT;
		break;
	case iliRotation270:
		writedata8_last(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
		_width = ILI9341_TFTHEIGHT;
		_height = ILI9341_TFTWIDTH;
		break;
	}
}


void ILI9341_due::invertDisplay(boolean i)
{
	writecommand_last(i ? ILI9341_INVON : ILI9341_INVOFF);
}


uint8_t ILI9341_due::readcommand8(uint8_t c, uint8_t index) {
	writecommand_cont(0xD9);  // woo sekret command?
	writedata8_last(0x10 + index);
	writecommand_cont(c);

	return readdata8_last();
}

// Reads one pixel/color from the TFT's GRAM
uint16_t ILI9341_due::readPixel(int16_t x, int16_t y)
{
	setAddr_cont(x, y, x + 1, y + 1);
	writecommand_cont(ILI9341_RAMRD); // read from RAM
	readdata8_cont(); // dummy read
	uint8_t red = read8_cont();
	uint8_t green = read8_cont();
	uint8_t blue = read8_last();
	return color565(red, green, blue);
}

//void ILI9341_due::drawArc(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, uint16_t start, uint16_t end, uint16_t color) {
//	//void graphics_draw_arc(GContext *ctx, GPoint p, int radius, int thickness, int start, int end) {
//	start = start % 360;
//	end = end % 360;
//
//	while (start < 0) start += 360;
//	while (end < 0) end += 360;
//
//	if (end == 0) end = 360;
//
//	//Serial << "start: " << start << " end:" << end << endl;
//
//	// Serial <<  (float)cos_lookup(start * ARC_MAX_STEPS / 360) << " x " << (float)sin_lookup(start * ARC_MAX_STEPS / 360) << endl;
//
//	float sslope = (float)cos_lookup(start * ARC_MAX_STEPS / 360) / (float)sin_lookup(start * ARC_MAX_STEPS / 360);
//	float eslope = (float)cos_lookup(end * ARC_MAX_STEPS / 360) / (float)sin_lookup(end * ARC_MAX_STEPS / 360);
//
//	//Serial << "sslope: " << sslope << " eslope:" << eslope << endl;
//
//	if (end == 360) eslope = -1000000;
//
//	int ir2 = (radius - thickness) * (radius - thickness);
//	int or2 = radius * radius;
//
//	for (int x = -radius; x <= radius; x++)
//		for (int y = -radius; y <= radius; y++)
//		{
//			int x2 = x * x;
//			int y2 = y * y;
//
//			if (
//				(x2 + y2 < or2 && x2 + y2 >= ir2) &&
//				(
//				(y > 0 && start < 180 && x <= y * sslope) ||
//				(y < 0 && start > 180 && x >= y * sslope) ||
//				(y < 0 && start <= 180) ||
//				(y == 0 && start <= 180 && x < 0) ||
//				(y == 0 && start == 0 && x > 0)
//				) &&
//				(
//				(y > 0 && end < 180 && x >= y * eslope) ||
//				(y < 0 && end > 180 && x <= y * eslope) ||
//				(y > 0 && end >= 180) ||
//				(y == 0 && end >= 180 && x < 0) ||
//				(y == 0 && start == 0 && x > 0)
//				)
//				)
//				drawPixel_cont(cx+x, cy+y, color);
//		}
//}



#ifdef FEATURE_ARC_ENABLED
// DrawArc function thanks to Jnmattern and his Arc_2.0 (https://github.com/Jnmattern)
void ILI9341_due::drawArcOffsetted(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color) {
	int16_t xmin = 65535, xmax = -32767, ymin = 32767, ymax = -32767;
	float cosStart, sinStart, cosEnd, sinEnd;
	float r, t;
	float startAngle, endAngle;

	//Serial << "start: " << start << " end: " << end << endl;
	startAngle = (start / _arcAngleMax) * 360;	// 252
	endAngle = (end / _arcAngleMax) * 360;		// 807
	//Serial << "startAngle: " << startAngle << " endAngle: " << endAngle << endl;

	while (startAngle < 0) startAngle += 360;
	while (endAngle < 0) endAngle += 360;
	while (startAngle > 360) startAngle -= 360;
	while (endAngle > 360) endAngle -= 360;
	//Serial << "startAngleAdj: " << startAngle << " endAngleAdj: " << endAngle << endl;
	//if (endAngle == 0) endAngle = 360;

	if (startAngle > endAngle) {
		drawArcOffsetted(cx, cy, radius, thickness, ((startAngle) / (float)360) * _arcAngleMax, _arcAngleMax, color);
		drawArcOffsetted(cx, cy, radius, thickness, 0, ((endAngle) / (float)360) * _arcAngleMax, color);
	}
	else {
		// Calculate bounding box for the arc to be drawn
		cosStart = cosDegrees(startAngle);
		sinStart = sinDegrees(startAngle);
		cosEnd = cosDegrees(endAngle);
		sinEnd = sinDegrees(endAngle);

		//Serial << cosStart << " " << sinStart << " " << cosEnd << " " << sinEnd << endl;

		r = radius;
		// Point 1: radius & startAngle
		t = r * cosStart;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinStart;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		// Point 2: radius & endAngle
		t = r * cosEnd;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinEnd;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		r = radius - thickness;
		// Point 3: radius-thickness & startAngle
		t = r * cosStart;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinStart;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		// Point 4: radius-thickness & endAngle
		t = r * cosEnd;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinEnd;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;


		//Serial << xmin << " " << xmax << " " << ymin << " " << ymax << endl;
		// Corrections if arc crosses X or Y axis
		if ((startAngle < 90) && (endAngle > 90)) {
			ymax = radius;
		}

		if ((startAngle < 180) && (endAngle > 180)) {
			xmin = -radius;
		}

		if ((startAngle < 270) && (endAngle > 270)) {
			ymin = -radius;
		}

		// Slopes for the two sides of the arc
		float sslope = (float)cosStart / (float)sinStart;
		float eslope = (float)cosEnd / (float)sinEnd;

		//Serial << "sslope2: " << sslope << " eslope2:" << eslope << endl;

		if (endAngle == 360) eslope = -1000000;

		int ir2 = (radius - thickness) * (radius - thickness);
		int or2 = radius * radius;
		//Serial << "ymin: " << ymin << " ymax: " << ymax << endl;
#if SPI_MODE_DMA
		fillScanline(color, radius << 1);
#endif
		for (int x = xmin; x <= xmax; x++) {
			bool y1StartFound = false, y2StartFound = false;
			bool y1EndFound = false, y2EndSearching = false;
			int y1s = 0, y1e = 0, y2s = 0, y2e = 0;
			for (int y = ymin; y <= ymax; y++)
			{
				int x2 = x * x;
				int y2 = y * y;

				if (
					(x2 + y2 < or2 && x2 + y2 >= ir2) && (
					(y > 0 && startAngle < 180 && x <= y * sslope) ||
					(y < 0 && startAngle > 180 && x >= y * sslope) ||
					(y < 0 && startAngle <= 180) ||
					(y == 0 && startAngle <= 180 && x < 0) ||
					(y == 0 && startAngle == 0 && x > 0)
					) && (
					(y > 0 && endAngle < 180 && x >= y * eslope) ||
					(y < 0 && endAngle > 180 && x <= y * eslope) ||
					(y > 0 && endAngle >= 180) ||
					(y == 0 && endAngle >= 180 && x < 0) ||
					(y == 0 && startAngle == 0 && x > 0)))
				{
					if (!y1StartFound)	//start of the higher line found
					{
						y1StartFound = true;
						y1s = y;
					}
					else if (y1EndFound && !y2StartFound) //start of the lower line found
					{
						//Serial << "Found y2 start x: " << x << " y:" << y << endl;
						y2StartFound = true;
						//drawPixel_cont(cx+x, cy+y, ILI9341_BLUE);
						y2s = y;
						y += y1e - y1s - 1;	// calculate the most probable end of the lower line (in most cases the length of lower line is equal to length of upper line), in the next loop we will validate if the end of line is really there
						if (y > ymax - 1) // the most probable end of line 2 is beyond ymax so line 2 must be shorter, thus continue with pixel by pixel search
						{
							y = y2s;	// reset y and continue with pixel by pixel search
							y2EndSearching = true;
						}

						//Serial << "Upper line length: " << (y1e - y1s) << " Setting y to " << y << endl;
					}
					else if (y2StartFound && !y2EndSearching)
					{
						// we validated that the probable end of the lower line has a pixel, continue with pixel by pixel search, in most cases next loop with confirm the end of lower line as it will not find a valid pixel
						y2EndSearching = true;
					}
					//Serial << "x:" << x << " y:" << y << endl;
					//drawPixel_cont(cx+x, cy+y, ILI9341_BLUE);
				}
				else
				{
					if (y1StartFound && !y1EndFound) //higher line end found
					{
						y1EndFound = true;
						y1e = y - 1;
						//Serial << "line: " << y1s << " - " << y1e << endl;
						drawFastVLine_cont_noFill(cx + x, cy + y1s, y - y1s, color);
						if (y < 0)
						{
							//Serial << x << " " << y << endl;
							y = abs(y); // skip the empty middle
						}
						else
							break;
					}
					else if (y2StartFound)
					{
						if (y2EndSearching)
						{
							//Serial << "Found final end at y: " << y << endl;
							// we found the end of the lower line after pixel by pixel search
							drawFastVLine_cont_noFill(cx + x, cy + y2s, y - y2s, color);
							y2EndSearching = false;
							break;
						}
						else
						{
							//Serial << "Expected end not found" << endl;
							// the expected end of the lower line is not there so the lower line must be shorter
							y = y2s;	// put the y back to the lower line start and go pixel by pixel to find the end
							y2EndSearching = true;
						}
					}
					//else
					//drawPixel_cont(cx+x, cy+y, ILI9341_RED);
				}
				//

				//delay(75);
			}
			if (y1StartFound && !y1EndFound)
			{
				y1e = ymax;
				//Serial << "line: " << y1s << " - " << y1e << endl;
				drawFastVLine_cont_noFill(cx + x, cy + y1s, y1e - y1s + 1, color);
			}
			else if (y2StartFound && y2EndSearching)	// we found start of lower line but we are still searching for the end
			{										// which we haven't found in the loop so the last pixel in a column must be the end
				drawFastVLine_cont_noFill(cx + x, cy + y2s, ymax - y2s + 1, color);
			}
		}
		disableCS();
	}
}
#endif

void ILI9341_due::screenshotToConsole()
{
	uint16_t color565;
	uint8_t lastColor[3];
	uint8_t color[3];
	uint32_t sameColorPixelCount = 0;
	uint16_t sameColorPixelCount16 = 0;
	uint32_t sameColorStartIndex = 0;
	uint32_t totalImageDataLength = 0;

	Serial.println(F("==== PIXEL DATA START ===="));
	//uint16_t x=0;
	//uint16_t y=0;
	setAddr_cont(0, 0, _width - 1, _height - 1);
	writecommand_cont(ILI9341_RAMRD); // read from RAM
	readdata8_cont(); // dummy read, also sets DC high

#if SPI_MODE_DMA
	read_cont(color, 3);
	lastColor[0] = color[0];
	lastColor[1] = color[1];
	lastColor[2] = color[2];
#elif SPI_MODE_NORMAL | SPI_MODE_EXTENDED
	lastColor[0] = color[0] = read8_cont();
	lastColor[1] = color[1] = read8_cont();
	lastColor[2] = color[2] = read8_cont();
#endif
	printHex8(color, 3);	//write color of the first pixel
	totalImageDataLength += 6;
	sameColorStartIndex = 0;

	for (uint32_t i = 1; i < _width*_height; i++)
	{
#if SPI_MODE_DMA
		read_cont(color, 3);
#elif SPI_MODE_NORMAL | SPI_MODE_EXTENDED
		color[0] = read8_cont();
		color[1] = read8_cont();
		color[2] = read8_cont();
#endif

		if (color[0] != lastColor[0] ||
			color[1] != lastColor[1] ||
			color[2] != lastColor[2])
		{
			sameColorPixelCount = i - sameColorStartIndex;
			if (sameColorPixelCount > 65535)
			{
				sameColorPixelCount16 = 65535;
				printHex16(&sameColorPixelCount16, 1);
				printHex8(lastColor, 3);
				totalImageDataLength += 10;
				sameColorPixelCount16 = sameColorPixelCount - 65535;
			}
			else
				sameColorPixelCount16 = sameColorPixelCount;
			printHex16(&sameColorPixelCount16, 1);
			printHex8(color, 3);
			totalImageDataLength += 10;

			sameColorStartIndex = i;
			lastColor[0] = color[0];
			lastColor[1] = color[1];
			lastColor[2] = color[2];
		}
	}
	sameColorPixelCount = _width*_height - sameColorStartIndex;
	if (sameColorPixelCount > 65535)
	{
		sameColorPixelCount16 = 65535;
		printHex16(&sameColorPixelCount16, 1);
		printHex8(lastColor, 3);
		totalImageDataLength += 10;
		sameColorPixelCount16 = sameColorPixelCount - 65535;
	}
	else
		sameColorPixelCount16 = sameColorPixelCount;
	printHex16(&sameColorPixelCount16, 1);
	totalImageDataLength += 4;
	printHex32(&totalImageDataLength, 1);

	/*for(uint16_t x=0; x<_width; x++)
	{
	for(uint16_t y=0; y<_height; y++)
	{
	color[0] = read8();
	color[1] = read8();
	color[2] = read8();

	if(color[0] != lastColor[0] ||
	color[1] != lastColor[1] ||
	color[2] != lastColor[2])
	{

	}

	PrintHex8(color, 3);
	}
	}*/
	Serial.println();
	Serial.println(F("==== PIXEL DATA END ===="));
	Serial.print(F("Total Image Data Length: "));
	Serial.println(totalImageDataLength);
#if SPI_MODE_DMA
	_dmaSpi.init(ILI9341_SPI_CLKDIVIDER);
#endif
	disableCS();
}

/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to bex
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "glcdfont.c"

// Draw a circle outline
void ILI9341_due::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	drawPixel_cont(x0, y0 + r, color);
	drawPixel_cont(x0, y0 - r, color);
	drawPixel_cont(x0 + r, y0, color);
	drawPixel_cont(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawPixel_cont(x0 + x, y0 + y, color);
		drawPixel_cont(x0 - x, y0 + y, color);
		drawPixel_cont(x0 + x, y0 - y, color);
		drawPixel_cont(x0 - x, y0 - y, color);
		drawPixel_cont(x0 + y, y0 + x, color);
		drawPixel_cont(x0 - y, y0 + x, color);
		drawPixel_cont(x0 + y, y0 - x, color);
		drawPixel_cont(x0 - y, y0 - x, color);
	}
	disableCS();
}


void ILI9341_due::drawCircleHelper(int16_t x0, int16_t y0,
	int16_t r, uint8_t cornername, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4) {
			drawPixel_cont(x0 + x, y0 + y, color);
			drawPixel_cont(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2) {
			drawPixel_cont(x0 + x, y0 - y, color);
			drawPixel_cont(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8) {
			drawPixel_cont(x0 - y, y0 + x, color);
			drawPixel_cont(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1) {
			drawPixel_cont(x0 - y, y0 - x, color);
			drawPixel_cont(x0 - x, y0 - y, color);
		}
	}
	disableCS();
}

void ILI9341_due::fillCircle(int16_t x0, int16_t y0, int16_t r,
	uint16_t color)
{
	drawFastVLine(x0, y0 - r, 2 * r + 1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void ILI9341_due::fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
	uint8_t cornername, int16_t delta, uint16_t color)
{

	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

#if SPI_MODE_DMA
	fillScanline(color, 2 * max(x, y) + 1 + delta);
#endif
	enableCS();
	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if (cornername & 0x1) {
			drawFastVLine_cont_noFill(x0 + x, y0 - y, 2 * y + 1 + delta, color);
			drawFastVLine_cont_noFill(x0 + y, y0 - x, 2 * x + 1 + delta, color);
		}
		if (cornername & 0x2) {
			drawFastVLine_cont_noFill(x0 - x, y0 - y, 2 * y + 1 + delta, color);
			drawFastVLine_cont_noFill(x0 - y, y0 - x, 2 * x + 1 + delta, color);
		}
	}
	disableCS();
}

// Bresenham's algorithm - thx wikpedia
void ILI9341_due::drawLine(int16_t x0, int16_t y0,
	int16_t x1, int16_t y1, uint16_t color)
{
	if (y0 == y1) {
		if (x1 > x0) {
			drawFastHLine(x0, y0, x1 - x0 + 1, color);
		}
		else if (x1 < x0) {
			drawFastHLine(x1, y0, x0 - x1 + 1, color);
		}
		else {
			drawPixel(x0, y0, color);
		}
		return;
	}
	else if (x0 == x1) {
		if (y1 > y0) {
			drawFastVLine(x0, y0, y1 - y0 + 1, color);
		}
		else {
			drawFastVLine(x0, y1, y0 - y1 + 1, color);
		}
		return;
	}

	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	}
	else {
		ystep = -1;
	}

	int16_t xbegin = x0;
#if SPI_MODE_DMA
	fillScanline(color);
#endif
	enableCS();
	if (steep) {
		for (; x0 <= x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					writeVLine_cont_noCS_noFill(y0, xbegin, len + 1, color);
				}
				else {
					writePixel_cont_noCS(y0, x0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
			writeVLine_cont_noCS_noFill(y0, xbegin, x0 - xbegin, color);
		}

	}
	else {
		for (; x0 <= x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					writeHLine_cont_noCS_noFill(xbegin, y0, len + 1, color);
				}
				else {
					writePixel_cont_noCS(x0, y0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
			writeHLine_cont_noCS_noFill(xbegin, y0, x0 - xbegin, color);
		}
	}
	disableCS();
}


// Draw a rectangle
//void ILI9341_due::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
//{
//	writeHLine_cont(x, y, w, color);
//	writeHLine_cont(x, y+h-1, w, color);
//	writeVLine_cont(x, y, h, color);
//	writeVLine_last(x+w-1, y, h, color);
//}

void ILI9341_due::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
#if SPI_MODE_DMA
	fillScanline(color, max(w, h));
#endif
	enableCS();
	writeHLine_cont_noCS_noFill(x, y, w, color);
	writeHLine_cont_noCS_noFill(x, y + h - 1, w, color);
	writeVLine_cont_noCS_noFill(x, y, h, color);
	writeVLine_cont_noCS_noFill(x + w - 1, y, h, color);
	disableCS();
}

// Draw a rounded rectangle
void ILI9341_due::drawRoundRect(int16_t x, int16_t y, int16_t w,
	int16_t h, int16_t r, uint16_t color)
{
        if ( h <= 2 * r || w <= 2 * r) return;
#if SPI_MODE_DMA
	fillScanline(color, max(w, h));
#endif
	enableCS();
	// smarter version
	writeHLine_cont_noCS_noFill(x + r, y, w - 2 * r, color); // Top
	writeHLine_cont_noCS_noFill(x + r, y + h - 1, w - 2 * r, color); // Bottom
	writeVLine_cont_noCS_noFill(x, y + r, h - 2 * r, color); // Left
	writeVLine_cont_noCS_noFill(x + w - 1, y + r, h - 2 * r, color); // Right
	disableCS();
	// draw four corners
	drawCircleHelper(x + r, y + r, r, 1, color);
	drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
	drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}

// Fill a rounded rectangle
void ILI9341_due::fillRoundRect(int16_t x, int16_t y, int16_t w,
	int16_t h, int16_t r, uint16_t color)
{
	// smarter version
	fillRect(x + r, y, w - 2 * r, h, color);

	// draw four corners
	fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

// Draw a triangle
void ILI9341_due::drawTriangle(int16_t x0, int16_t y0,
	int16_t x1, int16_t y1,
	int16_t x2, int16_t y2, uint16_t color)
{
	drawLine(x0, y0, x1, y1, color);
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void ILI9341_due::fillTriangle(int16_t x0, int16_t y0,
	int16_t x1, int16_t y1,
	int16_t x2, int16_t y2, uint16_t color)
{

	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		swap(y0, y1); swap(x0, x1);
	}
	if (y1 > y2) {
		swap(y2, y1); swap(x2, x1);
	}
	if (y0 > y1) {
		swap(y0, y1); swap(x0, x1);
	}

	if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if (x1 < a)      a = x1;
		else if (x1 > b) b = x1;
		if (x2 < a)      a = x2;
		else if (x2 > b) b = x2;
		drawFastHLine(a, y0, b - a + 1, color);
		return;
	}

	int16_t
		dx01 = x1 - x0,
		dy01 = y1 - y0,
		dx02 = x2 - x0,
		dy02 = y2 - y0,
		dx12 = x2 - x1,
		dy12 = y2 - y1,
		sa = 0,
		sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (y1 == y2) last = y1;   // Include y1 scanline
	else         last = y1 - 1; // Skip it

#if SPI_MODE_DMA
	fillScanline(color, max(x0, max(x1, x2)) - min(x0, min(x1, x2)));	// fill scanline with the widest scanline that'll be used
#endif
	enableCS();
	for (y = y0; y <= last; y++) {
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if (a > b) swap(a, b);
		writeHLine_cont_noCS_noFill(a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++) {
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if (a > b) swap(a, b);
		writeHLine_cont_noCS_noFill(a, y, b - a + 1, color);
	}
	disableCS();
}

// draws monochrome (single color) bitmaps
void ILI9341_due::drawBitmap(int16_t x, int16_t y,
	const uint8_t *bitmap, int16_t w, int16_t h,
	uint16_t color)
{

	int16_t i, j, byteWidth = (w + 7) / 8;

#if SPI_MODE_DMA
	_hiByte = highByte(color);
	_loByte = lowByte(color);
	fillScanline(color, w);
#endif

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
				drawPixel(x+i, y+j, color);
#elif SPI_MODE_DMA
				_scanlineBuffer[i << 1] = _hiByte;
				_scanlineBuffer[(i << 1) + 1] = _loByte;
#endif
			}
		}
#if SPI_MODE_DMA
		setAddrAndRW_cont(x + i, y + j, x + w - 1, y + j);
		writeScanline_cont(w);
#endif
	}
	disableCS();
}

#ifdef FEATURE_PRINT_ENABLED
size_t ILI9341_due::write(uint8_t c) {
	if (c == '\n') {
		_cursorY += _textsize * 8;
		_cursorX = 0;
	}
	else if (c == '\r') {
		// skip em
	}
	else {
		drawChar(_cursorX, _cursorY, c, _textcolor, _textbgcolor, _textsize);
		_cursorX += _textsize * 6;
		if (_wrap && (_cursorX > (_width - _textsize * 6))) {
			_cursorY += _textsize * 8;
			_cursorX = 0;
		}
	}
	return 1;
}

// Draw a character
void ILI9341_due::drawChar(int16_t x, int16_t y, unsigned char c,
	uint16_t fgcolor, uint16_t bgcolor, uint8_t size)
{
	if ((x >= _width) || // Clip right
		(y >= _height) || // Clip bottom
		((x + 6 * size - 1) < 0) || // Clip left  TODO: is this correct?
		((y + 8 * size - 1) < 0))   // Clip top   TODO: is this correct?
		return;

	enableCS();

	if (fgcolor == bgcolor)
	{
		// This transparent approach is only about 20% faster	
		if (size == 1)
		{
			uint8_t mask = 0x01;
			int16_t xoff, yoff;
#if SPI_MODE_DMA
			fillScanline(fgcolor, 5);
#endif
			for (yoff = 0; yoff < 8; yoff++) {
				uint8_t line = 0;
				for (xoff = 0; xoff < 5; xoff++) {
					if (font[c * 5 + xoff] & mask) line |= 1;
					line <<= 1;
				}
				line >>= 1;
				xoff = 0;
				while (line) {
					if (line == 0x1F) {
						writeHLine_cont_noCS_noFill(x + xoff, y + yoff, 5, fgcolor);
						break;
					}
					else if (line == 0x1E) {
						writeHLine_cont_noCS_noFill(x + xoff, y + yoff, 4, fgcolor);
						break;
					}
					else if ((line & 0x1C) == 0x1C) {
						writeHLine_cont_noCS_noFill(x + xoff, y + yoff, 3, fgcolor);
						line <<= 4;
						xoff += 4;
					}
					else if ((line & 0x18) == 0x18) {
						writeHLine_cont_noCS_noFill(x + xoff, y + yoff, 2, fgcolor);
						line <<= 3;
						xoff += 3;
					}
					else if ((line & 0x10) == 0x10) {
						writePixel_cont_noCS(x + xoff, y + yoff, fgcolor);
						line <<= 2;
						xoff += 2;
					}
					else {
						line <<= 1;
						xoff += 1;
					}
				}
				mask = mask << 1;
			}
		}
		else {
			uint8_t mask = 0x01;
			int16_t xoff, yoff;
			for (yoff = 0; yoff < 8; yoff++) {
				uint8_t line = 0;
				for (xoff = 0; xoff < 5; xoff++) {
					if (font[c * 5 + xoff] & mask) line |= 1;
					line <<= 1;
				}
				line >>= 1;
				xoff = 0;
				while (line) {
					if (line == 0x1F) {
						fillRect(x + xoff * size, y + yoff * size,
							5 * size, size, fgcolor);
						break;
					}
					else if (line == 0x1E) {
						fillRect(x + xoff * size, y + yoff * size,
							4 * size, size, fgcolor);
						break;
					}
					else if ((line & 0x1C) == 0x1C) {
						fillRect(x + xoff * size, y + yoff * size,
							3 * size, size, fgcolor);
						line <<= 4;
						xoff += 4;
					}
					else if ((line & 0x18) == 0x18) {
						fillRect(x + xoff * size, y + yoff * size,
							2 * size, size, fgcolor);
						line <<= 3;
						xoff += 3;
					}
					else if ((line & 0x10) == 0x10) {
						fillRect(x + xoff * size, y + yoff * size,
							size, size, fgcolor);
						line <<= 2;
						xoff += 2;
					}
					else {
						line <<= 1;
						xoff += 1;
					}
				}
				mask = mask << 1;
			}
		}
	}
	else {
		// This solid background approach is about 5 time faster
		setAddrAndRW_cont(x, y, x + 6 * size - 1, y + 8 * size);
		setDCForData();
		uint8_t xr, yr;
		uint8_t mask = 0x01;
		uint16_t color;
		uint16_t scanlineId = 0;
		// for each pixel row
		for (y = 0; y < 8; y++)
		{
			//scanlineId = 0;
			for (yr = 0; yr < size; yr++)
			{
				scanlineId = 0;
				// draw 5px horizontal "bitmap" line
				for (x = 0; x < 5; x++) {
					if (font[c * 5 + x] & mask) {
						color = fgcolor;
					}
					else {
						color = bgcolor;
					}
					for (xr = 0; xr < size; xr++) {
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
						write16_cont(color);
#elif SPI_MODE_DMA
						_scanlineBuffer[scanlineId++] = highByte(color);
						_scanlineBuffer[scanlineId++] = lowByte(color);
#endif	
					}
				}
				// draw a gap between chars (1px for size of 1)
				for (xr = 0; xr < size; xr++) {
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
					write16_cont(bgcolor);
#elif SPI_MODE_DMA
					_scanlineBuffer[scanlineId++] = highByte(bgcolor);
					_scanlineBuffer[scanlineId++] = lowByte(bgcolor);
#endif	
				}
#if SPI_MODE_DMA
				writeScanline_cont(scanlineId - 1);
#endif	
			}
			mask = mask << 1;
		}
		// draw an empty line below a character
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
		uint32_t n = 6 * size * size;
		do {
			write16_cont(bgcolor);
			n--;
		} while (n > 0);
#elif SPI_MODE_DMA
		fillScanline(bgcolor, 6 * size);
		for (y = 0; y < size; y++)
		{
			writeScanline_cont(6 * size);
		}
#endif	
	}
	disableCS();
}

void ILI9341_due::setCursor(int16_t x, int16_t y) {
	_cursorX = x;
	_cursorY = y;
}

void ILI9341_due::setTextSize(uint8_t s) {
	_textsize = (s > 0) ? s : 1;
}

void ILI9341_due::setTextColor(uint16_t c) {
	// For 'transparent' background, we'll set the bg 
	// to the same as fg instead of using a flag
	_textcolor = _textbgcolor = c;
}

void ILI9341_due::setTextColor(uint16_t c, uint16_t b) {
	_textcolor = c;
	_textbgcolor = b;
}

void ILI9341_due::setTextWrap(boolean w) {
	_wrap = w;
}
#endif

uint8_t ILI9341_due::getRotation(void) {
	return _rotation;
}

// if true, tft will be blank (white), 
// display's frame buffer is unaffected
// (you can write to it without showing content on the screen)
void ILI9341_due::display(boolean d){
	writecommand_last(d ? ILI9341_DISPON : ILI9341_DISPOFF);
}

// puts display in/out of sleep mode
void ILI9341_due::sleep(boolean s)
{
	writecommand_last(s ? ILI9341_SLPIN : ILI9341_SLPOUT);
	delay(120);
}

void ILI9341_due::idle(boolean i){
	writecommand_last(i ? ILI9341_IDMON : ILI9341_IDMOFF);
}


void ILI9341_due::setPowerLevel(pwrLevel p)
{
	switch (p)
	{
	case PWRLEVEL_NORMAL:
		if (_isIdle) { idle(false); _isIdle = false; }
		if (_isInSleep) { sleep(false); _isInSleep = false; }
		break;
	case PWRLEVEL_IDLE:
		if (!_isIdle) { idle(true); _isIdle = true; }
		if (_isInSleep) { sleep(false); _isInSleep = false; }
		break;
	case PWRLEVEL_SLEEP:
		if (!_isInSleep) { sleep(true); _isInSleep = true; }
		if (_isIdle) { idle(false); _isIdle = false; }
		break;
	}
}

#ifdef FEATURE_ARC_ENABLED
void ILI9341_due::setArcParams(float arcAngleMax, int arcAngleOffset)
{
	_arcAngleMax = arcAngleMax;
	_arcAngleOffset = arcAngleOffset;
}
#endif

//uint8_t ILI9341_due::spiread(void) {
//	uint8_t r = 0;
//
//	//SPI.setClockDivider(_cs, 12); // 8-ish MHz (full! speed!)
//	//SPI.setBitOrder(_cs, MSBFIRST);
//	//SPI.setDataMode(_cs, SPI_MODE0);
//	r = SPI.transfer(_cs, 0x00);
//	Serial.print("read: 0x"); Serial.print(r, HEX);
//
//	return r;
//}
//
//void ILI9341_due::spiwrite(uint8_t c) {
//
//	//Serial.print("0x"); Serial.print(c, HEX); Serial.print(", ");
//
//
//	//SPI.setClockDivider(_cs, 12); // 8-ish MHz (full! speed!)
//	//SPI.setBitOrder(_cs, MSBFIRST);
//	//SPI.setDataMode(_cs, SPI_MODE0);
//	SPI.transfer(_cs, c);
//
//}

//void ILI9341_due::writecommand(uint8_t c) {
//	//*dcport &=  ~dcpinmask;
//	digitalWrite(_dc, LOW);
//	//*clkport &= ~clkpinmask; // clkport is a NULL pointer when hwSPI==true
//	//digitalWrite(_sclk, LOW);
//	//*csport &= ~cspinmask;
//	//digitalWrite(_cs, LOW);
//
//	spiwrite(c);
//
//	//*csport |= cspinmask;
//	//digitalWrite(_cs, HIGH);
//}
//
//
//void ILI9341_due::writedata(uint8_t c) {
//	//*dcport |=  dcpinmask;
//	digitalWrite(_dc, HIGH);
//	//*clkport &= ~clkpinmask; // clkport is a NULL pointer when hwSPI==true
//	//digitalWrite(_sclk, LOW);
//	//*csport &= ~cspinmask;
//	//digitalWrite(_cs, LOW);
//
//	spiwrite(c);
//
//	//digitalWrite(_cs, HIGH);
//	//*csport |= cspinmask;
//} 

void ILI9341_due::printHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex
{
	char tmp[length * 2 + 1];
	byte first;
	byte second;
	for (int i = 0; i < length; i++) {
		first = (data[i] >> 4) & 0x0f;
		second = data[i] & 0x0f;
		// base for converting single digit numbers to ASCII is 48
		// base for 10-16 to become upper-case characters A-F is 55
		// note: difference is 7
		tmp[i * 2] = first + 48;
		tmp[i * 2 + 1] = second + 48;
		if (first > 9) tmp[i * 2] += 7;
		if (second > 9) tmp[i * 2 + 1] += 7;
	}
	tmp[length * 2] = 0;
	Serial.print(tmp);
}


void ILI9341_due::printHex16(uint16_t *data, uint8_t length) // prints 8-bit data in hex
{
	char tmp[length * 4 + 1];
	byte first;
	byte second;
	byte third;
	byte fourth;
	for (int i = 0; i < length; i++) {
		first = (data[i] >> 12) & 0x0f;
		second = (data[i] >> 8) & 0x0f;
		third = (data[i] >> 4) & 0x0f;
		fourth = data[i] & 0x0f;
		//Serial << first << " " << second << " " << third << " " << fourth << endl;
		// base for converting single digit numbers to ASCII is 48
		// base for 10-16 to become upper-case characters A-F is 55
		// note: difference is 7
		tmp[i * 4] = first + 48;
		tmp[i * 4 + 1] = second + 48;
		tmp[i * 4 + 2] = third + 48;
		tmp[i * 4 + 3] = fourth + 48;
		//tmp[i*5+4] = 32; // add trailing space
		if (first > 9) tmp[i * 4] += 7;
		if (second > 9) tmp[i * 4 + 1] += 7;
		if (third > 9) tmp[i * 4 + 2] += 7;
		if (fourth > 9) tmp[i * 4 + 3] += 7;
	}
	tmp[length * 4] = 0;
	Serial.print(tmp);
}

void ILI9341_due::printHex32(uint32_t *data, uint8_t length) // prints 8-bit data in hex
{
	char tmp[length * 8 + 1];
	byte dataByte[8];
	for (int i = 0; i < length; i++) {
		dataByte[0] = (data[i] >> 28) & 0x0f;
		dataByte[1] = (data[i] >> 24) & 0x0f;
		dataByte[2] = (data[i] >> 20) & 0x0f;
		dataByte[3] = (data[i] >> 16) & 0x0f;
		dataByte[4] = (data[i] >> 12) & 0x0f;
		dataByte[5] = (data[i] >> 8) & 0x0f;
		dataByte[6] = (data[i] >> 4) & 0x0f;
		dataByte[7] = data[i] & 0x0f;
		//Serial << first << " " << second << " " << third << " " << fourth << endl;
		// base for converting single digit numbers to ASCII is 48
		// base for 10-16 to become upper-case characters A-F is 55
		// note: difference is 7
		tmp[i * 4] = dataByte[0] + 48;
		tmp[i * 4 + 1] = dataByte[1] + 48;
		tmp[i * 4 + 2] = dataByte[2] + 48;
		tmp[i * 4 + 3] = dataByte[3] + 48;
		tmp[i * 4 + 4] = dataByte[4] + 48;
		tmp[i * 4 + 5] = dataByte[5] + 48;
		tmp[i * 4 + 6] = dataByte[6] + 48;
		tmp[i * 4 + 7] = dataByte[7] + 48;
		//tmp[i*5+4] = 32; // add trailing space
		if (dataByte[0] > 9) tmp[i * 4] += 7;
		if (dataByte[1] > 9) tmp[i * 4 + 1] += 7;
		if (dataByte[2] > 9) tmp[i * 4 + 2] += 7;
		if (dataByte[3] > 9) tmp[i * 4 + 3] += 7;
		if (dataByte[4] > 9) tmp[i * 4 + 4] += 7;
		if (dataByte[5] > 9) tmp[i * 4 + 5] += 7;
		if (dataByte[6] > 9) tmp[i * 4 + 6] += 7;
		if (dataByte[7] > 9) tmp[i * 4 + 7] += 7;
	}
	tmp[length * 8] = 0;
	Serial.print(tmp);
}
