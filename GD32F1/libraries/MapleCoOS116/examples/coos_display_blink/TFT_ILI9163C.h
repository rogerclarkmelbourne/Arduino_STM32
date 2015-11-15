/*	Modified 04/2015 by Victor G. Perez to add support for Maple and Maple mini, STM32F103 processors
*	It can use SPI DMA transfers. To not use DMA, comment out the next define. 
*	Requires function dmaSend in SPI library. Result with 16bit SPI and DMA modes enabled in Maple Mini.
*/
#define SPI_16BIT
#define SPI_MODE_DMA 1
#define SPEED_UP 1 // Enables extra calculations in the circles routine to use fastVLine and fastHLine, only in DMA mode.


/*
	ILI9163C - A fast SPI driver for TFT that use Ilitek ILI9163C.
	
	Features:
	- Very FAST!, expecially with Teensy 3.x where uses hyper optimized SPI.
	- It uses just 4 or 5 wires.
	- Compatible at command level with Adafruit display series so it's easy to adapt existing code.
	- It uses the standard Adafruit_GFX Library (you need to install). 
	
	Background:
	I got one of those displays from a chinese ebay seller but unfortunatly I cannot get
	any working library so I decided to hack it. ILI9163C looks pretty similar to other 
	display driver but it uses it's own commands so it's tricky to work with it unlsess you
	carefully fight with his gigantic and not so clever datasheet.
	My display it's a 1.44"", 128x128 that suppose to substitute Nokia 5110 LCD and here's the 
	first confusion! Many sellers claim that it's compatible with Nokia 5110 (that use a philips
	controller) but the only similarity it's the pin names since that this one it's color and
	have totally different controller that's not compatible.
	http://www.ebay.com/itm/Replace-Nokia-5110-LCD-1-44-Red-Serial-128X128-SPI-Color-TFT-LCD-Display-Module-/141196897388
	http://www.elecrow.com/144-128x-128-tft-lcd-with-spi-interface-p-855.html
	Pay attention that   can drive different resolutions and your display can be
	160*128 or whatever, also there's a strain of this display with a black PCB that a friend of mine
	got some weeks ago and need some small changes in library to get working.
	If you look at TFT_ILI9163C.h file you can add your modifications and let me know so I
	can include for future versions.
	
	Code Optimizations:
	The purpose of this library it's SPEED. I have tried to use hardware optimized calls
	where was possible and results are quite good for most applications, actually nly filled circles
    are still a bit slow. Many SPI call has been optimized by reduce un-needed triggers to RS and CS
	lines. Of course it can be improved so feel free to add suggestions.
	-------------------------------------------------------------------------------
    Copyright (c) 2014, .S.U.M.O.T.O.Y., coded by Max MC Costa.    

    TFT_ILI9163C Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TFT_ILI9163C Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    This file needs the following Libraries:
 
    Adafruit_GFX by Adafruit:
    https://github.com/adafruit/Adafruit-GFX-Library
	Remember to update GFX library often to have more features with this library!
	From this version I'm using my version of Adafruit_GFX library:
	https://github.com/sumotoy/Adafruit-GFX-Library
	It has faster char rendering and some small little optimizations but you can
	choose one of the two freely since are both fully compatible.
	''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	Special Thanks:
	Thanks Adafruit for his Adafruit_GFX!
	Thanks to Paul Stoffregen for his beautiful Teensy3 and DMA SPI.
	
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Version:
	0.1a1: First release, compile correctly. Altrough not fully working!
	0.1a3: Better but still some addressing problems.
	0.1b1: Beta! Addressing solved, now rotation works and boundaries ok.
	0.2b1: Cleaned up.
	0.2b3: Added 2.2" Red PCB parameters
	0.2b4: Bug fixes, added colorSpace (for future send image)
	0.2b5: Cleaning
	0.3b1: Complete rework on Teensy SPI based on Paul Stoffregen work
	SPI transaction,added BLACK TAG 2.2 display
	0.3b2: Minor fix, load 24bit image, Added conversion utility
	0.4:	some improvement, new ballistic gauge example!
	0.5:	Added scroll and more commands, optimizations
	0.6:	Small fix, added SD example and subroutines
	0.6b1:  Fix clearscreen, missed a parameter.
	0.6b2:  Scroll completed. (thanks Masuda)
	0.6b3:	Clear Screen fix v2. Added Idle mode.
	0.7:    Init correction.Clear Screen fix v3 (last time?)
	0.75:   SPI transactions for arduino's (beta)
	0.8:	Compatiblke with IDE 1.0.6 (teensyduino 1.20) and IDE 1.6.x (teensyduino 1.21b)
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	BugList of the current version:
	
	Please report any!

	
Here's the speed test between 0.2b5 and 0.3b1 on Teensy3.1 (major SPI changes)
------------------------------------------------------------------------
Lines                    17024  	16115	BETTER
Horiz/Vert Lines         5360		5080	BETTER
Rectangles (outline)     4384		4217	BETTER
Rectangles (filled)      96315		91265	BETTER
Circles (filled)         16053		15829	LITTLE BETTER
Circles (outline)        11540		20320	WORST!
Triangles (outline)      5359		5143	BETTER
Triangles (filled)       19088		18741	BETTER
Rounded rects (outline)  8681		12498	LITTLE WORST
Rounded rects (filled)   105453		100213	BETTER
Done!


*/


/*
Benchmark                Time (microseconds)
Screen fill              40676
Text                     11756
Lines                    42841
Horiz/Vert Lines         4033
Rectangles (outline)     3724
Rectangles (filled)      65417
Circles (filled)         28576
Circles (outline)        22532
Triangles (outline)      14877
Triangles (filled)       34927
Rounded rects (outline)  18777
Rounded rects (filled)   77142
Done!


*/

#ifndef _TFT_ILI9163CLIB_H_
#define _TFT_ILI9163CLIB_H_


#include "Arduino.h"
#include "Print.h"
#include <Adafruit_GFX.h>



//DID YOU HAVE A RED PCB, BLACk PCB or WHAT DISPLAY TYPE???????????? ---> SELECT HERE <----
//#define __144_RED_PCB__//128x128
#define __144_BLACK_PCB__//128x128
//#define __22_RED_PCB__//240x320
//---------------------------------------

#if defined(__SAM3X8E__)
	#include <include/pio.h>
	#define PROGMEM
	#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
	#define pgm_read_word(addr) (*(const unsigned short *)(addr))
	typedef unsigned char prog_uchar;
	#define SPICLOCK 8000000
#endif
#ifdef __AVR__
	#include <avr/pgmspace.h>
	#define SPICLOCK 8000000
#endif
#if defined(__MK20DX128__) || defined(__MK20DX256__)
	#define SPICLOCK 30000000
#endif


#if defined(__144_RED_PCB__)
/*
This display:
http://www.ebay.com/itm/Replace-Nokia-5110-LCD-1-44-Red-Serial-128X128-SPI-Color-TFT-LCD-Display-Module-/271422122271
This particular display has a design error! The controller has 3 pins to configure to constrain
the memory and resolution to a fixed dimension (in that case 128x128) but they leaved those pins
configured for 128x160 so there was several pixel memory addressing problems.
I solved by setup several parameters that dinamically fix the resolution as needed so below
the parameters for this diplay. If you have a strain or a correct display (can happen with chinese)
you can copy those parameters and create setup for different displays.
*/
	#define _TFTWIDTH  		128//the REAL W resolution of the TFT
	#define _TFTHEIGHT 		128//the REAL H resolution of the TFT
	#define _GRAMWIDTH      128
	#define _GRAMHEIGH      160//160
	#define _GRAMSIZE		_GRAMWIDTH * _GRAMHEIGH//*see note 1
	#define __COLORSPC		1// 1:GBR - 0:RGB
	#define __GAMMASET3		//uncomment for another gamma
	#define __OFFSET		32//*see note 2
	//Tested!
#elif defined (__144_BLACK_PCB__)
	#define _TFTWIDTH  		128//the REAL W resolution of the TFT
	#define _TFTHEIGHT 		128//the REAL H resolution of the TFT
	#define _GRAMWIDTH      128
	#define _GRAMHEIGH      128
	#define _GRAMSIZE		_GRAMWIDTH * _GRAMHEIGH//*see note 1
	#define __COLORSPC		1// 1:GBR - 0:RGB
	#define __GAMMASET1		//uncomment for another gamma
	#define __OFFSET		0
	//not tested
#elif defined (__22_RED_PCB__)
/*
Like this one:
http://www.ebay.it/itm/2-2-Serial-SPI-TFT-LCD-Display-Module-240x320-Chip-ILI9340C-PCB-Adapter-SD-Card-/281304733556
Not tested!
*/
	#define _TFTWIDTH  		240//the REAL W resolution of the TFT
	#define _TFTHEIGHT 		320//the REAL H resolution of the TFT
	#define _GRAMWIDTH      240
	#define _GRAMHEIGH      320
	#define _GRAMSIZE		_GRAMWIDTH * _GRAMHEIGH
	#define __COLORSPC		1// 1:GBR - 0:RGB
	#define __GAMMASET1		//uncomment for another gamma
	#define __OFFSET		0
#else
	#define _TFTWIDTH  		128//128
	#define _TFTHEIGHT 		160//160
	#define _GRAMWIDTH      128
	#define _GRAMHEIGH      160
	#define _GRAMSIZE		_GRAMWIDTH * _GRAMHEIGH
	#define __COLORSPC		1// 1:GBR - 0:RGB
	#define __GAMMASET1
	#define __OFFSET		0
#endif
/*
	Note 1: The __144_RED_PCB__ display has hardware addressing of 128 x 160
	but the tft resolution it's 128 x 128 so the dram should be set correctly
	
	Note 2: This is the offset between image in RAM and TFT. In that case 160 - 128 = 32;
*/

//--------- Color definitions -------------

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF
#define TRANSPARENT     -1

//--------- Used for STM32 DMA ------------
#define SCANLINE_BUFFER_SIZE _GRAMHEIGH

//ILI9163C registers-----------------------
#define CMD_NOP     	0x00//Non operation
#define CMD_SWRESET 	0x01//Soft Reset
#define CMD_SLPIN   	0x10//Sleep ON
#define CMD_SLPOUT  	0x11//Sleep OFF
#define CMD_PTLON   	0x12//Partial Mode ON
#define CMD_NORML   	0x13//Normal Display ON
#define CMD_DINVOF  	0x20//Display Inversion OFF
#define CMD_DINVON   	0x21//Display Inversion ON
#define CMD_GAMMASET 	0x26//Gamma Set (0x01[1],0x02[2],0x04[3],0x08[4])
#define CMD_DISPOFF 	0x28//Display OFF
#define CMD_DISPON  	0x29//Display ON
#define CMD_IDLEON  	0x39//Idle Mode ON
#define CMD_IDLEOF  	0x38//Idle Mode OFF
#define CMD_CLMADRS   	0x2A//Column Address Set
#define CMD_PGEADRS   	0x2B//Page Address Set

#define CMD_RAMWR   	0x2C//Memory Write
#define CMD_RAMRD   	0x2E//Memory Read
#define CMD_CLRSPACE   	0x2D//Color Space : 4K/65K/262K
#define CMD_PARTAREA	0x30//Partial Area
#define CMD_VSCLLDEF	0x33//Vertical Scroll Definition
#define CMD_TEFXLON		0x35//Tearing Effect Line ON
#define CMD_TEFXLOF		0x34//Tearing Effect Line OFF
#define CMD_MADCTL  	0x36//Memory Access Control
#define CMD_VSSTADRS	0x37//Vertical Scrolling Start address
#define CMD_PIXFMT  	0x3A//Interface Pixel Format
#define CMD_FRMCTR1 	0xB1//Frame Rate Control (In normal mode/Full colors)
#define CMD_FRMCTR2 	0xB2//Frame Rate Control(In Idle mode/8-colors)
#define CMD_FRMCTR3 	0xB3//Frame Rate Control(In Partial mode/full colors)
#define CMD_DINVCTR		0xB4//Display Inversion Control
#define CMD_RGBBLK		0xB5//RGB Interface Blanking Porch setting
#define CMD_DFUNCTR 	0xB6//Display Fuction set 5
#define CMD_SDRVDIR 	0xB7//Source Driver Direction Control
#define CMD_GDRVDIR 	0xB8//Gate Driver Direction Control 

#define CMD_PWCTR1  	0xC0//Power_Control1
#define CMD_PWCTR2  	0xC1//Power_Control2
#define CMD_PWCTR3  	0xC2//Power_Control3
#define CMD_PWCTR4  	0xC3//Power_Control4
#define CMD_PWCTR5  	0xC4//Power_Control5
#define CMD_VCOMCTR1  	0xC5//VCOM_Control 1
#define CMD_VCOMCTR2  	0xC6//VCOM_Control 2
#define CMD_VCOMOFFS  	0xC7//VCOM Offset Control
#define CMD_PGAMMAC		0xE0//Positive Gamma Correction Setting
#define CMD_NGAMMAC		0xE1//Negative Gamma Correction Setting
#define CMD_GAMRSEL		0xF2//GAM_R_SEL


class TFT_ILI9163C : public Adafruit_GFX {

 public:

	TFT_ILI9163C(uint8_t cspin,uint8_t dcpin,uint8_t rstpin);
	TFT_ILI9163C(uint8_t CS, uint8_t DC);//connect rst pin to VDD

	void     	begin(void),
				setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1),//graphic Addressing
				setCursor(int16_t x,int16_t y),//char addressing
				pushColor(uint16_t color),
				fillScreen(uint16_t color=0x0000),
				clearScreen(uint16_t color=0x0000),//same as fillScreen
				drawPixel(int16_t x, int16_t y, uint16_t color),
				drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
				drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
				#if defined(__MK20DX128__) || defined(__MK20DX256__)//workaround to get more speed from Teensy
				drawLine(int16_t x0, int16_t y0,int16_t x1, int16_t y1, uint16_t color),
				drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
				#endif
				fillRect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color),
				setRotation(uint8_t r),
				invertDisplay(boolean i);
	void		idleMode(boolean onOff);
	void		display(boolean onOff);	
	void		sleepMode(boolean mode);
	void 		defineScrollArea(uint16_t tfa, uint16_t bfa);
	void		scroll(uint16_t adrs);
	void 		startPushData(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	void 		pushData(uint16_t color);
	void 		endPushData();
	void		writeScreen24(const uint32_t *bitmap,uint16_t size=_TFTWIDTH*_TFTHEIGHT);
	inline uint16_t Color565(uint8_t r, uint8_t g, uint8_t b) {return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);};
  //convert 24bit color into packet 16 bit one (credits for this are all mine)
	inline uint16_t Color24To565(int32_t color_) { return ((((color_ >> 16) & 0xFF) / 8) << 11) | ((((color_ >> 8) & 0xFF) / 4) << 5) | (((color_) &  0xFF) / 8);}
	void 		setBitrate(uint32_t n);	
	#if defined SPI_MODE_DMA
    uint16_t _scanlineBuffer16[SCANLINE_BUFFER_SIZE];
	uint8_t* _scanlineBuffer8 = reinterpret_cast<uint8_t *>(_scanlineBuffer16);
    uint8_t _hiByte, _loByte;
	void		fillScanline(uint16_t color, size_t n),
				writeScanline(size_t n),
				drawLine(int16_t x0, int16_t y0,int16_t x1, int16_t y1, uint16_t color),
				writePixel_cont_noCS(int16_t x, int16_t y, uint16_t color),
				writeVLine_cont_noCS_noFill(int16_t x, int16_t y, int16_t h),
				writeHLine_cont_noCS_noFill(int16_t x, int16_t y, int16_t w),
				setAddrWindow_cont(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1),
				writecommand_cont(uint8_t c),
				writedata16_cont(uint16_t d),
				drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
				drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	#endif //SPI_MODE_DMA
				
 private:
	uint8_t		_Mactrl_Data;//container for the memory access control data
	uint8_t		_colorspaceData;
	void 		colorSpace(uint8_t cspace);
	void 		setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	void 		endProc(void);
	uint8_t		sleep;


	#if defined(__MK20DX128__) || defined(__MK20DX256__)
	//
	#else
	void		writecommand(uint8_t c);
	void		writedata(uint8_t d);
	void		writedata16(uint16_t d);
	#endif
	void 		chipInit();
	bool 		boundaryCheck(int16_t x,int16_t y);
	void 		homeAddress();
	#if defined (__AVR__)
	void				spiwrite(uint8_t);
	volatile uint8_t 	*dataport, *clkport, *csport, *rsport;
	uint8_t 			_cs,_rs,_sid,_sclk,_rst;
	uint8_t  			datapinmask, clkpinmask, cspinmask, rspinmask;
	#endif //  #ifdef __AVR__
	
	#if defined (__STM32F1__)
	void				spiwrite(uint8_t);
	volatile uint32_t 	*dataport, *clkport, *csport, *rsport;
	uint8_t 			_cs,_rs,_sid,_sclk,_rst;
	uint32_t  			datapinmask, clkpinmask, cspinmask, rspinmask;
	#endif //  #ifdef __STM32F1__
	
	#if defined(__SAM3X8E__)
	void				spiwrite(uint8_t);
	Pio 				*dataport, *clkport, *csport, *rsport;
	uint8_t 			_cs,_rs,_sid,_sclk,_rst;
	uint32_t  			datapinmask, clkpinmask, cspinmask, rspinmask;
	#endif //  #if defined(__SAM3X8E__)
  
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
	//Here's Paul Stoffregen magic in action...
	uint8_t _cs, _rs, _rst;
	uint8_t pcs_data, pcs_command;
	
	void _setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);//graphic Addressing for Teensy
	
	void waitFifoNotFull(void) {
		uint32_t sr;
		uint32_t tmp __attribute__((unused));
		do {
			#if ARDUINO >= 160
				sr = KINETISK_SPI0.SR;
			#else
				sr = SPI0.SR;
			#endif
			if (sr & 0xF0) tmp = SPI0_POPR;  // drain RX FIFO
		} while ((sr & (15 << 12)) > (3 << 12));
	}

	void waitFifoEmpty(void) {
		uint32_t sr;
		uint32_t tmp __attribute__((unused));
		do {
			#if ARDUINO >= 160
				sr = KINETISK_SPI0.SR;
			#else
				sr = SPI0.SR;
			#endif
			if (sr & 0xF0) tmp = SPI0_POPR;  // drain RX FIFO
		} while ((sr & 0xF0F0) > 0);             // wait both RX & TX empty
	}
	
	void waitTransmitComplete(void) __attribute__((always_inline)) {
		uint32_t tmp __attribute__((unused));
		#if ARDUINO >= 160
		while (!(KINETISK_SPI0.SR & SPI_SR_TCF)) ; // wait until final output done
		#else
		while (!(SPI0.SR & SPI_SR_TCF)) ; // wait until final output done
		#endif
		tmp = SPI0_POPR;                  // drain the final RX FIFO word
	}
	
	void writecommand_cont(uint8_t c) __attribute__((always_inline)) {
		#if ARDUINO >= 160
		KINETISK_SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
		#else
		SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
		#endif
		waitFifoNotFull();
	}
	
	void writedata8_cont(uint8_t c) __attribute__((always_inline)) {
		#if ARDUINO >= 160
		KINETISK_SPI0.PUSHR = c | (pcs_data << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
		#else
		SPI0.PUSHR = c | (pcs_data << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
		#endif
		waitFifoNotFull();
	}
	
	void writedata16_cont(uint16_t d) __attribute__((always_inline)) {
		#if ARDUINO >= 160
		KINETISK_SPI0.PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT;
		#else
		SPI0.PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT;
		#endif
		waitFifoNotFull();
	}
	
	void writecommand_last(uint8_t c) __attribute__((always_inline)) {
		waitFifoEmpty();
		#if ARDUINO >= 160
		KINETISK_SPI0.SR = SPI_SR_TCF;
		KINETISK_SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0);
		#else
		SPI0.SR = SPI_SR_TCF;
		SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0);
		#endif
		waitTransmitComplete();
	}
	
	void writedata8_last(uint8_t c) __attribute__((always_inline)) {
		waitFifoEmpty();
		#if ARDUINO >= 160
		KINETISK_SPI0.SR = SPI_SR_TCF;
		KINETISK_SPI0.PUSHR = c | (pcs_data << 16) | SPI_PUSHR_CTAS(0);
		#else
		SPI0.SR = SPI_SR_TCF;
		SPI0.PUSHR = c | (pcs_data << 16) | SPI_PUSHR_CTAS(0);
		#endif
		waitTransmitComplete();
	}
	
	void writedata16_last(uint16_t d) __attribute__((always_inline)) {
		waitFifoEmpty();
		#if ARDUINO >= 160
		KINETISK_SPI0.SR = SPI_SR_TCF;
		KINETISK_SPI0.PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(1);
		#else
		SPI0.SR = SPI_SR_TCF;
		SPI0.PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(1);
		#endif
		waitTransmitComplete();
	}
	
	void HLine(int16_t x, int16_t y, int16_t w, uint16_t color) __attribute__((always_inline)) {
		_setAddrWindow(x, y, x+w-1, y);
		//writecommand_cont(CMD_RAMWR);//not needed
		do { writedata16_cont(color); } while (--w > 0);
	}

	void Pixel(int16_t x, int16_t y, uint16_t color) __attribute__((always_inline)) {
		_setAddrWindow(x, y, x, y);
		//writecommand_cont(CMD_RAMWR);//not needed
		writedata16_cont(color);
	}
	
	void VLine(int16_t x, int16_t y, int16_t h, uint16_t color) __attribute__((always_inline)) {
		_setAddrWindow(x, y, x, y+h-1);
		//writecommand_cont(CMD_RAMWR);//not needed
		do { writedata16_cont(color); } while (--h > 0);
	}
	#endif
	
};
#endif