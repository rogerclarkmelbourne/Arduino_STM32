#include "TFT_ILI9163C.h"
#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include ".\SPICoOS.h"

//constructors
TFT_ILI9163C::TFT_ILI9163C(uint8_t cspin,uint8_t dcpin,uint8_t rstpin) : Adafruit_GFX(_TFTWIDTH,_TFTHEIGHT){
	_cs   = cspin;
	_rs   = dcpin;
	_rst  = rstpin;
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
	#else
	_sid  = _sclk = 0;
	#endif
}


TFT_ILI9163C::TFT_ILI9163C(uint8_t CS, uint8_t DC) : Adafruit_GFX(_TFTWIDTH, _TFTHEIGHT) {
	_cs   = CS;
	_rs   = DC;
	_rst  = 0;
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
	#else
	_sid  = _sclk = 0;
	#endif
}

//Arduino Uno, Leonardo, Mega, Teensy 2.0, etc
#ifdef __AVR__

inline void TFT_ILI9163C::spiwrite(uint8_t c){
    SPDR = c;
    while(!(SPSR & _BV(SPIF)));
}

void TFT_ILI9163C::writecommand(uint8_t c){
	#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
    #endif
	*rsport &= ~rspinmask;//low
	*csport &= ~cspinmask;//low
	spiwrite(c);
	*csport |= cspinmask;//hi
	#ifdef SPI_HAS_TRANSACTION
	SPI.endTransaction();
	#endif
}

void TFT_ILI9163C::writedata(uint8_t c){
	#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
    #endif
	*rsport |=  rspinmask;
	*csport &= ~cspinmask;
	spiwrite(c);
	*csport |= cspinmask;
	#ifdef SPI_HAS_TRANSACTION
	SPI.endTransaction();
	#endif
} 

void TFT_ILI9163C::writedata16(uint16_t d){
	#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
    #endif
	*rsport |=  rspinmask;
	*csport &= ~cspinmask;
	spiwrite(d >> 8);
	spiwrite(d);
	*csport |= cspinmask;
	#ifdef SPI_HAS_TRANSACTION
	SPI.endTransaction();
	#endif
} 

void TFT_ILI9163C::setBitrate(uint32_t n){
	#if !defined (SPI_HAS_TRANSACTION)
	if (n >= 8000000) {
		SPI.setClockDivider(SPI_CLOCK_DIV2);
	} else if (n >= 4000000) {
		SPI.setClockDivider(SPI_CLOCK_DIV4);
	} else if (n >= 2000000) {
		SPI.setClockDivider(SPI_CLOCK_DIV8);
	} else {
		SPI.setClockDivider(SPI_CLOCK_DIV16);
	}
	#endif
}
#elif defined(__SAM3X8E__)
// Arduino Due

inline void TFT_ILI9163C::spiwrite(uint8_t c){
    SPI.transfer(c);
}

void TFT_ILI9163C::writecommand(uint8_t c){
	#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
    #endif
	rsport->PIO_CODR |=  rspinmask;//LO
	csport->PIO_CODR  |=  cspinmask;//LO
	spiwrite(c);
	csport->PIO_SODR  |=  cspinmask;//HI
	#ifdef SPI_HAS_TRANSACTION
	SPI.endTransaction();
	#endif
}

void TFT_ILI9163C::writedata(uint8_t c){
	#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
    #endif
	rsport->PIO_SODR |=  rspinmask;//HI
	csport->PIO_CODR  |=  cspinmask;//LO
	spiwrite(c);
	csport->PIO_SODR  |=  cspinmask;//HI
	#ifdef SPI_HAS_TRANSACTION
	SPI.endTransaction();
	#endif
} 

void TFT_ILI9163C::writedata16(uint16_t d){
	#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
    #endif
	rsport->PIO_SODR |=  rspinmask;//HI
	csport->PIO_CODR  |=  cspinmask;//LO
	spiwrite(d >> 8);
	spiwrite(d);
	csport->PIO_SODR  |=  cspinmask;//HI
	#ifdef SPI_HAS_TRANSACTION
	SPI.endTransaction();
	#endif
}


void TFT_ILI9163C::setBitrate(uint32_t n){
	#if !defined (SPI_HAS_TRANSACTION)
	uint32_t divider=1;
	while (divider < 255) {
		if (n >= 84000000 / divider) break;
		divider = divider - 1;
	}
	SPI.setClockDivider(divider);
	#endif
}

#elif defined(__STM32F1__)
// Maple & Maple mini

inline void TFT_ILI9163C::spiwrite(uint8_t c){
    SPI.write(c);
}

void TFT_ILI9163C::writecommand(uint8_t c){
	#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
    #endif
	*rsport &= ~rspinmask;//low
	*csport &= ~cspinmask;//low
	spiwrite(c);
	*csport |= cspinmask;//hi
	#ifdef SPI_HAS_TRANSACTION
	SPI.endTransaction();
	#endif
}

void TFT_ILI9163C::writedata(uint8_t c){
	#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
    #endif
	*rsport |=  rspinmask;
	*csport &= ~cspinmask;
#if defined SPI_16BIT
	SPI.setDataSize (0);
#endif
	spiwrite(c);
#if defined SPI_16BIT
	SPI.setDataSize (SPI_CR1_DFF);
#endif
	*csport |= cspinmask;
	#ifdef SPI_HAS_TRANSACTION
	SPI.endTransaction();
	#endif
} 

void TFT_ILI9163C::writedata16(uint16_t d){
	#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
    #endif
	*rsport |=  rspinmask;
	*csport &= ~cspinmask;
#if defined SPI_16BIT
	SPI.write(d);
#else
	spiwrite(d >> 8);
	spiwrite(d);
#endif
	*csport |= cspinmask;
	#ifdef SPI_HAS_TRANSACTION
	SPI.endTransaction();
	#endif
} 

void TFT_ILI9163C::setBitrate(uint32_t n){
	#if !defined (SPI_HAS_TRANSACTION)
	if (n >= 8000000) {
		SPI.setClockDivider(SPI_CLOCK_DIV2);
	} else if (n >= 4000000) {
		SPI.setClockDivider(SPI_CLOCK_DIV4);
	} else if (n >= 2000000) {
		SPI.setClockDivider(SPI_CLOCK_DIV8);
	} else {
		SPI.setClockDivider(SPI_CLOCK_DIV16);
	}
	#endif
}

///////////////
#elif defined(__MK20DX128__) || defined(__MK20DX256__)
//Teensy 3.0 & 3.1  

void TFT_ILI9163C::setBitrate(uint32_t n){
	//nop
}

#endif //#if defined(TEENSY3.x)


void TFT_ILI9163C::begin(void) {
	sleep = 0;
#if defined (__AVR__) || (__STM32F1__)
	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);
	csport    = portOutputRegister(digitalPinToPort(_cs));
	rsport    = portOutputRegister(digitalPinToPort(_rs));
	cspinmask = digitalPinToBitMask(_cs);
	rspinmask = digitalPinToBitMask(_rs);
    SPI.begin();
	#if defined SPI_16BIT
		SPI.setDataSize (SPI_CR1_DFF);
	#endif
	#if !defined (SPI_HAS_TRANSACTION)
    SPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
	#endif
	// toggle RST low to reset; CS low so it'll listen to us
	*csport &= ~cspinmask;
#elif defined(__SAM3X8E__)
	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);
	csport    = digitalPinToPort(_cs);
	rsport    = digitalPinToPort(_rs);
	cspinmask = digitalPinToBitMask(_cs);
	rspinmask = digitalPinToBitMask(_rs);
    SPI.begin();
	#if !defined (SPI_HAS_TRANSACTION)
    SPI.setClockDivider(11); // 8 MHz
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
	#endif
	// toggle RST low to reset; CS low so it'll listen to us
	csport ->PIO_CODR  |=  cspinmask; // Set control bits to LOW (idle)
#elif defined(__MK20DX128__) || defined(__MK20DX256__)
	SPI.begin();
	if (SPI.pinIsChipSelect(_cs, _rs)) {
		pcs_data = SPI.setCS(_cs);
		pcs_command = pcs_data | SPI.setCS(_rs);
	} else {
		pcs_data = 0;
		pcs_command = 0;
		return;
	}
#endif
	if (_rst != 0) {
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, HIGH);
		delay(500);
		digitalWrite(_rst, LOW);
		delay(500);
		digitalWrite(_rst, HIGH);
		delay(500);
	}

/*
7) MY:  1(bottom to top), 0(top to bottom) 	Row Address Order
6) MX:  1(R to L),        0(L to R)        	Column Address Order
5) MV:  1(Exchanged),     0(normal)        	Row/Column exchange
4) ML:  1(bottom to top), 0(top to bottom) 	Vertical Refresh Order
3) RGB: 1(BGR), 		   0(RGB)           	Color Space
2) MH:  1(R to L),        0(L to R)        	Horizontal Refresh Order
1)
0)

     MY, MX, MV, ML,RGB, MH, D1, D0
	 0 | 0 | 0 | 0 | 1 | 0 | 0 | 0	//normal
	 1 | 0 | 0 | 0 | 1 | 0 | 0 | 0	//Y-Mirror
	 0 | 1 | 0 | 0 | 1 | 0 | 0 | 0	//X-Mirror
	 1 | 1 | 0 | 0 | 1 | 0 | 0 | 0	//X-Y-Mirror
	 0 | 0 | 1 | 0 | 1 | 0 | 0 | 0	//X-Y Exchange
	 1 | 0 | 1 | 0 | 1 | 0 | 0 | 0	//X-Y Exchange, Y-Mirror
	 0 | 1 | 1 | 0 | 1 | 0 | 0 | 0	//XY exchange
	 1 | 1 | 1 | 0 | 1 | 0 | 0 | 0
*/
  _Mactrl_Data = 0b00000000;
  _colorspaceData = __COLORSPC;//start with default data;
  chipInit();
}



void TFT_ILI9163C::chipInit() {
	uint8_t i;
	#if defined(__GAMMASET1)
	const uint8_t pGammaSet[15]= {0x36,0x29,0x12,0x22,0x1C,0x15,0x42,0xB7,0x2F,0x13,0x12,0x0A,0x11,0x0B,0x06};
	const uint8_t nGammaSet[15]= {0x09,0x16,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x34,0x39};
	#elif defined(__GAMMASET2)
	const uint8_t pGammaSet[15]= {0x3F,0x21,0x12,0x22,0x1C,0x15,0x42,0xB7,0x2F,0x13,0x02,0x0A,0x01,0x00,0x00};
	const uint8_t nGammaSet[15]= {0x09,0x18,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x24,0x29};
	#elif defined(__GAMMASET3)
	const uint8_t pGammaSet[15]= {0x3F,0x26,0x23,0x30,0x28,0x10,0x55,0xB7,0x40,0x19,0x10,0x1E,0x02,0x01,0x00};
	//&const uint8_t nGammaSet[15]= {0x00,0x19,0x1C,0x0F,0x14,0x0F,0x2A,0x48,0x3F,0x06,0x1D,0x21,0x3D,0x3F,0x3F};
	const uint8_t nGammaSet[15]= {0x09,0x18,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x24,0x29};
	#else
	const uint8_t pGammaSet[15]= {0x3F,0x25,0x1C,0x1E,0x20,0x12,0x2A,0x90,0x24,0x11,0x00,0x00,0x00,0x00,0x00};
	const uint8_t nGammaSet[15]= {0x20,0x20,0x20,0x20,0x05,0x15,0x00,0xA7,0x3D,0x18,0x25,0x2A,0x2B,0x2B,0x3A};
	#endif
	
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
	SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
	writecommand_cont(CMD_SWRESET);//software reset
	delay(500);
	writecommand_cont(CMD_SLPOUT);//exit sleep
	delay(5);
	writecommand_cont(CMD_PIXFMT);//Set Color Format 16bit   
	writedata8_cont(0x05);
	delay(5);
	writecommand_cont(CMD_GAMMASET);//default gamma curve 3
	writedata8_cont(0x08);//0x04
	delay(1);
	writecommand_cont(CMD_GAMRSEL);//Enable Gamma adj    
	writedata8_cont(0x01); 
	delay(1);
	writecommand_cont(CMD_NORML);
	
	
	writecommand_cont(CMD_DFUNCTR);
	writedata8_cont(0b11111111);//
	writedata8_cont(0b00000110);//

	writecommand_cont(CMD_PGAMMAC);//Positive Gamma Correction Setting
	for (i=0;i<15;i++){
		writedata8_cont(pGammaSet[i]);
	}
	writecommand_cont(CMD_NGAMMAC);//Negative Gamma Correction Setting
	for (i=0;i<15;i++){
		writedata8_cont(nGammaSet[i]);
	}
	
	writecommand_cont(CMD_FRMCTR1);//Frame Rate Control (In normal mode/Full colors)
	writedata8_cont(0x08);//0x0C//0x08
	writedata8_cont(0x02);//0x14//0x08
	delay(1);
	
	writecommand_cont(CMD_DINVCTR);//display inversion 
	writedata8_cont(0x07);
    delay(1);
	
	writecommand_cont(CMD_PWCTR1);//Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD   
	writedata8_cont(0x0A);//4.30 - 0x0A
	writedata8_cont(0x02);//0x05
	delay(1);
	
	writecommand_cont(CMD_PWCTR2);//Set BT[2:0] for AVDD & VCL & VGH & VGL   
	writedata8_cont(0x02);
	delay(1);
	
	writecommand_cont(CMD_VCOMCTR1);//Set VMH[6:0] & VML[6:0] for VOMH & VCOML   
	writedata8_cont(0x50);//0x50
	writedata8_cont(99);//0x5b
	delay(1);
	
	writecommand_cont(CMD_VCOMOFFS);
	writedata8_cont(0);//0x40
	delay(1);
  
	writecommand_cont(CMD_CLMADRS);//Set Column Address  
	writedata16_cont(0x00);
	writedata16_cont(_GRAMWIDTH); 
  
	writecommand_cont(CMD_PGEADRS);//Set Page Address  
	writedata16_cont(0x00);
	writedata16_cont(_GRAMHEIGH); 
	// set scroll area (thanks Masuda)
    writecommand_cont(CMD_VSCLLDEF);
    writedata16_cont(__OFFSET);
    writedata16_cont(_GRAMHEIGH - __OFFSET);
    writedata16_last(0);
		
	endProc();
	colorSpace(_colorspaceData);
	setRotation(0);
	SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
	writecommand_cont(CMD_DISPON);//display ON 
	delay(1);
	writecommand_last(CMD_RAMWR);//Memory Write
	SPI.endTransaction();
	delay(1);

	#else
	writecommand(CMD_SWRESET);//software reset
	delay(500);
	writecommand(CMD_SLPOUT);//exit sleep
	delay(5);
	writecommand(CMD_PIXFMT);//Set Color Format 16bit   
	writedata(0x05);
	delay(5);
	writecommand(CMD_GAMMASET);//default gamma curve 3
	writedata(0x04);//0x04
	delay(1);
	writecommand(CMD_GAMRSEL);//Enable Gamma adj    
	writedata(0x01); 
	delay(1);
	writecommand(CMD_NORML);
	
	writecommand(CMD_DFUNCTR);
	writedata(0b11111111);//
	writedata(0b00000110);//

	writecommand(CMD_PGAMMAC);//Positive Gamma Correction Setting
	for (i=0;i<15;i++){
		writedata(pGammaSet[i]);
	}
	writecommand(CMD_NGAMMAC);//Negative Gamma Correction Setting
	for (i=0;i<15;i++){
		writedata(nGammaSet[i]);
	}

	writecommand(CMD_FRMCTR1);//Frame Rate Control (In normal mode/Full colors)
	writedata(0x08);//0x0C//0x08
	writedata(0x02);//0x14//0x08
	delay(1);
	writecommand(CMD_DINVCTR);//display inversion 
	writedata(0x07);
    delay(1);
	writecommand(CMD_PWCTR1);//Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD   
	writedata(0x0A);//4.30 - 0x0A
	writedata(0x02);//0x05
	delay(1);
	writecommand(CMD_PWCTR2);//Set BT[2:0] for AVDD & VCL & VGH & VGL   
	writedata(0x02);
	delay(1);
	writecommand(CMD_VCOMCTR1);//Set VMH[6:0] & VML[6:0] for VOMH & VCOML   
	writedata(0x50);//0x50
	writedata(99);//0x5b
	delay(1);
	writecommand(CMD_VCOMOFFS);
	writedata(0);//0x40
	delay(1);
  
	writecommand(CMD_CLMADRS);//Set Column Address  
	writedata16(0x00); 
	writedata16(_GRAMWIDTH); 
  
	writecommand(CMD_PGEADRS);//Set Page Address  
	writedata16(0X00); 
	writedata16(_GRAMHEIGH);
	// set scroll area (thanks Masuda)
    writecommand(CMD_VSCLLDEF);
    writedata16(__OFFSET);
    writedata16(_GRAMHEIGH - __OFFSET);
    writedata16(0);
	colorSpace(_colorspaceData);
	setRotation(0);
	writecommand(CMD_DISPON);//display ON 
	delay(1);
	writecommand(CMD_RAMWR);//Memory Write

	delay(1);
	#endif
	fillScreen(BLACK);
}

/*
Colorspace selection:
0: RGB
1: GBR
*/
void TFT_ILI9163C::colorSpace(uint8_t cspace) {
	if (cspace < 1){
		bitClear(_Mactrl_Data,3);
	} else {
		bitSet(_Mactrl_Data,3);
	}
}

void TFT_ILI9163C::invertDisplay(boolean i) {
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
		writecommand_last(i ? CMD_DINVON : CMD_DINVOF);
		SPI.endTransaction();
	#else
		writecommand(i ? CMD_DINVON : CMD_DINVOF);
	#endif
}

void TFT_ILI9163C::display(boolean onOff) {
	if (onOff){
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
			writecommand_last(CMD_DISPON);
			endProc();
		#else
			writecommand(CMD_DISPON);
		#endif
	} else {
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
			writecommand_last(CMD_DISPOFF);
			endProc();
		#else
			writecommand(CMD_DISPOFF);
		#endif
	}
}

void TFT_ILI9163C::idleMode(boolean onOff) {
	if (onOff){
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
			writecommand_last(CMD_IDLEON);
			endProc();
		#else
			writecommand(CMD_IDLEON);
		#endif
	} else {
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
			writecommand_last(CMD_IDLEOF);
			endProc();
		#else
			writecommand(CMD_IDLEOF);
		#endif
	}
}

void TFT_ILI9163C::sleepMode(boolean mode) {
	if (mode){
		if (sleep == 1) return;//already sleeping
		sleep = 1;
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
			writecommand_last(CMD_SLPIN);
			endProc();
		#else
			writecommand(CMD_SLPIN);
		#endif
		delay(5);//needed
	} else {
		if (sleep == 0) return; //Already awake
		sleep = 0;
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
			writecommand_last(CMD_SLPOUT);
			endProc();
		#else
			writecommand(CMD_SLPOUT);
		#endif
		delay(120);//needed
	}
}

void TFT_ILI9163C::defineScrollArea(uint16_t tfa, uint16_t bfa){
    tfa += __OFFSET;
    int16_t vsa = _GRAMHEIGH - tfa - bfa;
    if (vsa >= 0) {
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
        writecommand_cont(CMD_VSCLLDEF);
        writedata16_cont(tfa);
        writedata16_cont(vsa);
        writedata16_last(bfa);
		endProc();
		#else
        writecommand(CMD_VSCLLDEF);
        writedata16(tfa);
        writedata16(vsa);
        writedata16(bfa);
		#endif
    }
}

void TFT_ILI9163C::scroll(uint16_t adrs) {
	if (adrs <= _GRAMHEIGH) {
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
		writecommand_cont(CMD_VSSTADRS);
		writedata16_last(adrs + __OFFSET);
		endProc();
	#else
		writecommand(CMD_VSSTADRS);
		writedata16(adrs + __OFFSET);
	#endif
	}
}


//corrected! v3
void TFT_ILI9163C::clearScreen(uint16_t color) {
	int px;
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
		//writecommand_cont(CMD_RAMWR);
		_setAddrWindow(0x00,0x00,_GRAMWIDTH,_GRAMHEIGH);
		for (px = 0;px < _GRAMSIZE; px++){
			writedata16_cont(color);
		}
		writecommand_last(CMD_NOP);
		endProc();
	#elif defined(SPI_MODE_DMA)	
		fillScanline(color, _GRAMHEIGH);
		setAddr(0x00,0x00,_GRAMWIDTH,_GRAMHEIGH);//go home
		*rsport |=  rspinmask;
		*csport &= ~cspinmask;
		for (px = 0; px < _GRAMWIDTH; px++){
			writeScanline(_GRAMHEIGH);
		}
		*csport |= cspinmask;

	#else
		//writecommand(CMD_RAMWR);
		setAddr(0x00,0x00,_GRAMWIDTH,_GRAMHEIGH);//go home
		for (px = 0;px < _GRAMSIZE; px++){
			writedata16(color);
		}
	#endif
}

void TFT_ILI9163C::startPushData(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	setAddr(x0,y0,x1,y1);
}

void TFT_ILI9163C::pushData(uint16_t color) {
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		writedata16_cont(color);
	#else
		writedata16(color);
	#endif
}


void TFT_ILI9163C::endPushData() {
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		writecommand_last(CMD_NOP);
		endProc();
	#endif
}


void TFT_ILI9163C::pushColor(uint16_t color) {
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
		writedata16_last(color);
		endProc();
	#else
		writedata16(color);
	#endif
}
	
void TFT_ILI9163C::writeScreen24(const uint32_t *bitmap,uint16_t size) {
	uint16_t color;
	int px;
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		writecommand_cont(CMD_RAMWR);
		for (px = 0;px < size; px++){//16384
			color = Color24To565(bitmap[px]);
			writedata16_cont(color);
		}
		_setAddrWindow(0x00,0x00,_GRAMWIDTH,_GRAMHEIGH);//home
		endProc();
	#else
	
		writecommand(CMD_RAMWR);
		for (px = 0;px < size; px++){
			color = Color24To565(bitmap[px]);
			writedata16(color);
		}
		homeAddress();
	#endif
}

void TFT_ILI9163C::homeAddress() {
	setAddrWindow(0x00,0x00,_GRAMWIDTH,_GRAMHEIGH);
}



void TFT_ILI9163C::setCursor(int16_t x, int16_t y) {
	if (boundaryCheck(x,y)) return;
	setAddrWindow(0x00,0x00,x,y);
	cursor_x = x;
	cursor_y = y;
}



void TFT_ILI9163C::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if (boundaryCheck(x,y)) return;
	if ((x < 0) || (y < 0)) return;
	setAddr(x,y,x+1,y+1);
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		writedata16_last(color);
		endProc();
	#else
		writedata16(color);
	#endif
}



void TFT_ILI9163C::endProc(void){
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.endTransaction();
	#endif
}


void TFT_ILI9163C::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
	// Rudimentary clipping
	if (boundaryCheck(x,y)) return;
	if (((y + h) - 1) >= _height) h = _height-y;
	if (h < 2 ) {
		drawPixel(x, y, color);
		return;
	}
	setAddr(x,y,x,(y+h)-1);
	#if !defined SPI_MODE_DMA
	while (h-- > 1) {
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
		if (h == 0){
			writedata16_last(color);
		} else {
			writedata16_cont(color);
		}
		#else
			writedata16(color);
		#endif
	}

	endProc();
	#else //SPI_MODE_DMA
	fillScanline(color, h);
	*rsport |=  rspinmask;
	*csport &= ~cspinmask;
	writeScanline(h);
	*csport |= cspinmask;
	#endif
}

bool TFT_ILI9163C::boundaryCheck(int16_t x,int16_t y){
	if ((x >= _width) || (y >= _height)) return true;
	return false;
}

void TFT_ILI9163C::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
	// Rudimentary clipping
	if (boundaryCheck(x,y)) return;
	if (((x+w) - 1) >= _width)  w = _width-x;
	if (w < 2 ) {
		drawPixel(x, y, color);
		return;
	}
	setAddr(x,y,(x+w)-1,y);
	#if !defined SPI_MODE_DMA
	while (w-- > 1) {
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
		if (w == 0){
			writedata16_last(color);
		} else {
			writedata16_cont(color);
		}
		#else
			writedata16(color);
		#endif
	}
	endProc();
	#else //SPI_MODE_DMA
	fillScanline(color, w);
	*rsport |=  rspinmask;
	*csport &= ~cspinmask;
	writeScanline(w);
	*csport |= cspinmask;
	#endif
}

inline void TFT_ILI9163C::fillScreen(uint16_t color) {
	clearScreen(color);
}

// fill a rectangle
void TFT_ILI9163C::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
	if (boundaryCheck(x,y)) return;
	if (((x + w) - 1) >= _width)  w = _width  - x;
	if (((y + h) - 1) >= _height) h = _height - y;
	if (w == 1 && h == 1) {
		drawPixel(x, y, color);
		return;
	}
	#if !defined SPI_MODE_DMA	
	setAddr(x,y,(x+w)-1,(y+h)-1);
	for (y = h;y > 0;y--) {
		for (x = w;x > 0;x--) {
			#if defined(__MK20DX128__) || defined(__MK20DX256__)
				writedata16_cont(color);
			#else
				writedata16(color);
			#endif
		}
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
		writedata16_last(color);
		#endif
	}
	endProc();
	#else //SPI_MODE_DMA
	setAddr(x,y,(x+w)-1,(y+h)-1);
	if (w < h) swap(w, h);
	fillScanline(color, w);
	*rsport |=  rspinmask;
	*csport &= ~cspinmask;
	for (y = h;y > 0;y--) {
		writeScanline(w);
	}
	*csport |= cspinmask;
	#endif
}

#if defined(__MK20DX128__) || defined(__MK20DX256__)
void TFT_ILI9163C::drawLine(int16_t x0, int16_t y0,int16_t x1, int16_t y1, uint16_t color){
	if (y0 == y1) {
		if (x1 > x0) {
			drawFastHLine(x0, y0, x1 - x0 + 1, color);
		} else if (x1 < x0) {
			drawFastHLine(x1, y0, x0 - x1 + 1, color);
		} else {
			drawPixel(x0, y0, color);
		}
		return;
	} else if (x0 == x1) {
		if (y1 > y0) {
			drawFastVLine(x0, y0, y1 - y0 + 1, color);
		} else {
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
	} else {
		ystep = -1;
	}

	SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
	int16_t xbegin = x0;
	if (steep) {
		for (; x0<=x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					VLine(y0, xbegin, len + 1, color);
				} else {
					Pixel(y0, x0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
			VLine(y0, xbegin, x0 - xbegin, color);
		}

	} else {
		for (; x0<=x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					HLine(xbegin, y0, len + 1, color);
				} else {
					Pixel(x0, y0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
			HLine(xbegin, y0, x0 - xbegin, color);
		}
	}
	writecommand_last(CMD_NOP);
	SPI.endTransaction();
}

void TFT_ILI9163C::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
	SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
	HLine(x, y, w, color);
	HLine(x, y+h-1, w, color);
	VLine(x, y, h, color);
	VLine(x+w-1, y, h, color);
	writecommand_last(CMD_NOP);
	SPI.endTransaction();
}


#endif

void TFT_ILI9163C::setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
		_setAddrWindow(x0,y0,x1,y1);
	#else
		setAddrWindow(x0,y0,x1,y1);
	#endif
}

void TFT_ILI9163C::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
		_setAddrWindow(x0,y0,x1,y1);
		SPI.endTransaction();
	#else
		writecommand(CMD_CLMADRS); // Column
		if (rotation == 0 || rotation > 1){
			writedata16(x0);
			writedata16(x1);
		} else {
			writedata16(x0 + __OFFSET);
			writedata16(x1 + __OFFSET);
		}

		writecommand(CMD_PGEADRS); // Page
		if (rotation == 0){
			writedata16(y0 + __OFFSET);
			writedata16(y1 + __OFFSET);
		} else {
			writedata16(y0);
			writedata16(y1);
		}
		writecommand(CMD_RAMWR); //Into RAM
	#endif
}

#if defined(__MK20DX128__) || defined(__MK20DX256__)
void TFT_ILI9163C::_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	writecommand_cont(CMD_CLMADRS); // Column
	if (rotation == 0 || rotation > 1){
		writedata16_cont(x0);
		writedata16_cont(x1);
	} else {
		writedata16_cont(x0 + __OFFSET);
		writedata16_cont(x1 + __OFFSET);
	}
	writecommand_cont(CMD_PGEADRS); // Page
	if (rotation == 0){
		writedata16_cont(y0 + __OFFSET);
		writedata16_cont(y1 + __OFFSET);
	} else {
		writedata16_cont(y0);
		writedata16_cont(y1);
	}
	writecommand_cont(CMD_RAMWR); //Into RAM
}
#endif

void TFT_ILI9163C::setRotation(uint8_t m) {
	rotation = m % 4; // can't be higher than 3
	switch (rotation) {
	case 0:
		_Mactrl_Data = 0b00001000;
		_width  = _TFTWIDTH;
		_height = _TFTHEIGHT;//-__OFFSET;
		break;
	case 1:
		_Mactrl_Data = 0b01101000;
		_width  = _TFTHEIGHT;//-__OFFSET;
		_height = _TFTWIDTH;
		break;
	case 2:
		_Mactrl_Data = 0b11001000;
		_width  = _TFTWIDTH;
		_height = _TFTHEIGHT;//-__OFFSET;
		break;
	case 3:
		_Mactrl_Data = 0b10101000;
		_width  = _TFTWIDTH;
		_height = _TFTHEIGHT;//-__OFFSET;
		break;
	}
	colorSpace(_colorspaceData);
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
		writecommand_cont(CMD_MADCTL);
		writedata8_last(_Mactrl_Data);
		endProc();
	#else
		writecommand(CMD_MADCTL);
		writedata(_Mactrl_Data);
	#endif
}

#if SPI_MODE_DMA
void TFT_ILI9163C::drawLine(int16_t x0, int16_t y0,int16_t x1, int16_t y1, uint16_t color)
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
	fillScanline(color, _GRAMHEIGH);
	*csport &= ~cspinmask;
	if (steep) {
		for (; x0 <= x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					//drawFastVLine (y0, xbegin, len + 1, color);
					writeVLine_cont_noCS_noFill(y0, xbegin, len + 1);
				}
				else {
					//drawPixel(x0, y0, color);
					writePixel_cont_noCS(y0, x0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
			writeVLine_cont_noCS_noFill(y0, xbegin, x0 - xbegin);
		}

	}
	else {
		for (; x0 <= x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					//drawFastVLine(xbegin, y0, len + 1, color);
					writeHLine_cont_noCS_noFill(xbegin, y0, len + 1);
				}
				else {
					//drawPixel(x0, y0, color);
					writePixel_cont_noCS(x0, y0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
			writeHLine_cont_noCS_noFill(xbegin, y0, x0 - xbegin);
		}
	}
	*csport |= cspinmask;
}

void TFT_ILI9163C::writePixel_cont_noCS(int16_t x, int16_t y, uint16_t color)
{
    if (boundaryCheck(x,y)) return;
	if ((x < 0) || (y < 0)) return;
	setAddrWindow_cont(x,y,x+1,y+1);
	#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
    #endif
	*rsport |=  rspinmask;
	#if defined SPI_16BIT
	SPI.write(color);
	#else
	spiwrite(color >> 8);
	spiwrite(color);
	#endif
	#ifdef SPI_HAS_TRANSACTION
	SPI.endTransaction();
	#endif
}

void TFT_ILI9163C::writeVLine_cont_noCS_noFill(int16_t x, int16_t y, int16_t h)
{
	// Rudimentary clipping
	if (boundaryCheck(x,y)) return;
	if ((x < 0) || (y < 0)) return;
	if (((y + h) - 1) >= _height) h = _height-y;
	setAddrWindow_cont(x,y,x,(y+h)-1);
	*rsport |=  rspinmask;
	writeScanline(h); // send 2 bytes per pixel
}

void TFT_ILI9163C::writeHLine_cont_noCS_noFill(int16_t x, int16_t y, int16_t w)
{
	// Rudimentary clipping
	if (boundaryCheck(x,y)) return;
	if ((x < 0) || (y < 0)) return;
	if (((x+w) - 1) >= _width)  w = _width-x;
	setAddrWindow_cont(x,y,(x+w)-1,y);
	*rsport |=  rspinmask;
	writeScanline(w);
}

void TFT_ILI9163C::setAddrWindow_cont(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	writecommand_cont(CMD_CLMADRS); // Column
	if (rotation == 0 || rotation > 1){
		writedata16_cont(x0);
		writedata16_cont(x1);
	} else {
		writedata16_cont(x0 + __OFFSET);
		writedata16_cont(x1 + __OFFSET);
	}
	writecommand_cont(CMD_PGEADRS); // Page
	if (rotation == 0){
		writedata16_cont(y0 + __OFFSET);
		writedata16_cont(y1 + __OFFSET);
	} else {
		writedata16_cont(y0);
		writedata16_cont(y1);
	}
	writecommand_cont(CMD_RAMWR); //Into RAM
}

void TFT_ILI9163C::writedata16_cont(uint16_t d){
	#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
    #endif
	*rsport |=  rspinmask;
#if defined SPI_16BIT
	SPI.write(d);
#else
	spiwrite(d >> 8);
	spiwrite(d);
#endif
	#ifdef SPI_HAS_TRANSACTION
	SPI.endTransaction();
	#endif
} 

void TFT_ILI9163C::writecommand_cont(uint8_t c){
	#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
    #endif
	*rsport &= ~rspinmask;//low
	spiwrite(c);
	#ifdef SPI_HAS_TRANSACTION
	SPI.endTransaction();
	#endif
}

void TFT_ILI9163C::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
	fillScanline(color, _GRAMHEIGH);
	*csport &= ~cspinmask;
	writeHLine_cont_noCS_noFill(x, y, w);
	writeHLine_cont_noCS_noFill(x, y+h-1, w);
	writeVLine_cont_noCS_noFill(x, y, h);
	writeVLine_cont_noCS_noFill(x+w-1, y, h);
	*csport |= cspinmask;
}

void TFT_ILI9163C::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;
	*csport &= ~cspinmask;
	writePixel_cont_noCS(x0, y0 + r, color);
	writePixel_cont_noCS(x0, y0 - r, color);
	writePixel_cont_noCS(x0 + r, y0, color);
	writePixel_cont_noCS(x0 - r, y0, color);
	#if SPEED_UP
	if (r<6){
	#endif
	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		writePixel_cont_noCS(x0 + x, y0 + y, color);
		writePixel_cont_noCS(x0 - x, y0 + y, color);
		writePixel_cont_noCS(x0 + x, y0 - y, color);
		writePixel_cont_noCS(x0 - x, y0 - y, color);
		writePixel_cont_noCS(x0 + y, y0 + x, color);
		writePixel_cont_noCS(x0 - y, y0 + x, color);
		writePixel_cont_noCS(x0 + y, y0 - x, color);
		writePixel_cont_noCS(x0 - y, y0 - x, color);
	}
	#if SPEED_UP
	}
	else {
	fillScanline(color, _GRAMHEIGH);
	int16_t xi = x;
	int16_t yi = y;
	int16_t w = 0;
	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (y < yi){
			w = x - xi;
			if (w > 0){
			writeHLine_cont_noCS_noFill(x0 + xi, y0 + yi, w);
			writeHLine_cont_noCS_noFill(x0 - x + 1, y0 + yi, w);
			writeHLine_cont_noCS_noFill(x0 + xi, y0 - yi, w);
			writeHLine_cont_noCS_noFill(x0 - x + 1, y0 - yi, w);
			writeVLine_cont_noCS_noFill(x0 + yi, y0 + xi, w);
			writeVLine_cont_noCS_noFill(x0 - yi, y0 + xi, w);
			writeVLine_cont_noCS_noFill(x0 + yi, y0 - x + 1, w);
			writeVLine_cont_noCS_noFill(x0 - yi, y0 - x + 1, w);
			}
			else {
			writePixel_cont_noCS(x0 + x, y0 + y, color);
			writePixel_cont_noCS(x0 - x, y0 + y, color);
			writePixel_cont_noCS(x0 + x, y0 - y, color);
			writePixel_cont_noCS(x0 - x, y0 - y, color);
			writePixel_cont_noCS(x0 + y, y0 + x, color);
			writePixel_cont_noCS(x0 - y, y0 + x, color);
			writePixel_cont_noCS(x0 + y, y0 - x, color);
			writePixel_cont_noCS(x0 - y, y0 - x, color);
			}
		yi = y;
		xi = x;
		}
	}
		writePixel_cont_noCS(x0 + x, y0 + y, color);
		writePixel_cont_noCS(x0 - x, y0 + y, color);
		writePixel_cont_noCS(x0 + x, y0 - y, color);
		writePixel_cont_noCS(x0 - x, y0 - y, color);
		writePixel_cont_noCS(x0 + y, y0 + x, color);
		writePixel_cont_noCS(x0 - y, y0 + x, color);
		writePixel_cont_noCS(x0 + y, y0 - x, color);
		writePixel_cont_noCS(x0 - y, y0 - x, color);
	}
	#endif
	*csport |= cspinmask;
}

// Sets first n pixels in scanline buffer to the specified color
//    __attribute__((always_inline))
void TFT_ILI9163C::fillScanline(uint16_t color, size_t n) {
#if defined SPI_16BIT
	//color = (color << 8) | (color >> 8); 
//    for (uint16_t i = 0; i < n; i ++)
//    {
      _scanlineBuffer16[0] = color;
//    }
#else
    _hiByte = highByte(color);
    _loByte = lowByte(color);
    for (uint16_t i = 0; i < (n << 1); i += 2)
    {
      _scanlineBuffer8[i] = _hiByte;
      _scanlineBuffer8[i + 1] = _loByte;
    }
#endif
}

// Enables CS, sets DC and writes n-bytes from the scanline buffer via DMA
// Does not disable CS
//    inline __attribute__((always_inline))
void TFT_ILI9163C::writeScanline(size_t n) {
	if (n == 0) return;
#if defined SPI_16BIT
    SPI.dmaSend(_scanlineBuffer16, n, 0);	// each pixel is one halfword. circular mode
#else
    SPI.dmaSend(_scanlineBuffer8, n << 1);	// each pixel is 2 bytes
#endif
}


#endif



