/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
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

#include "Adafruit_GFX_AS.h"

#ifdef LOAD_GLCD
  #include "glcdfont.c"
#endif

#ifdef LOAD_FONT2
  #include "Font16.h"
#endif

#ifdef LOAD_FONT4
#include "Font32.h"
#endif

#ifdef LOAD_FONT6
#include "Font64.h"
#endif

#ifdef LOAD_FONT7
  #include "Font7s.h"
#endif

#ifdef __AVR__
 #include <avr/pgmspace.h>
#else
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif

Adafruit_GFX_AS::Adafruit_GFX_AS(int16_t w, int16_t h): Adafruit_GFX(w, h)
{
}

/***************************************************************************************
** Function name:           drawUnicode
** Descriptions:            draw a unicode
***************************************************************************************/
int16_t Adafruit_GFX_AS::drawUnicode(uint16_t uniCode, int16_t x, int16_t y, int16_t size)
{
    
   if (size) uniCode -= 32;

   uint16_t width = 0;
   uint16_t height = 0;
   uint32_t flash_address = 0;
   int16_t gap = 0;

//   if (size == 1) {
//     flash_address = pgm_read_dword(&chrtbl_f8[uniCode]);
//     width = pgm_read_byte(widtbl_f8+uniCode);
//     height = chr_hgt_f8;
//     gap = 1;
//   }
#ifdef LOAD_FONT2
   if (size == 2) {
     flash_address = pgm_read_dword(&chrtbl_f16[uniCode]);
     width = pgm_read_byte(widtbl_f16+uniCode);
     height = chr_hgt_f16;
     gap = 1;
   }
#endif
//   if (size == 3) {
//     flash_address = pgm_read_dword(&chrtbl_f24[uniCode]);
//     width = pgm_read_byte(widtbl_f24+uniCode);
//     height = chr_hgt_f24;
//     gap = 0;
//   }
#ifdef LOAD_FONT4
   if (size == 4) {
     flash_address = pgm_read_dword(&chrtbl_f32[uniCode]);
     width = pgm_read_byte(widtbl_f32+uniCode);
     height = chr_hgt_f32;
     gap = -3;
   }
#endif
//   if (size == 5) {
//     flash_address = pgm_read_dword(&chrtbl_f48[uniCode]);
//     width = pgm_read_byte(widtbl_f48+uniCode);
//     height = chr_hgt_f48;
//     gap = -3;
//   }
#ifdef LOAD_FONT6
   if (size == 6) {
     flash_address = pgm_read_dword(&chrtbl_f64[uniCode]);
     width = pgm_read_byte(widtbl_f64+uniCode);
     height = chr_hgt_f64;
     gap = -3;
   }
#endif
#ifdef LOAD_FONT7
   if (size == 7) {
     flash_address = pgm_read_dword(&chrtbl_f7s[uniCode]);
     width = pgm_read_byte(widtbl_f7s+uniCode);
     height = chr_hgt_f7s;
     gap = 2;
   }
#endif

int16_t w = (width+7)/8;
int16_t pX      = 0;
int16_t pY      = y;
int16_t color   = 0;
byte line = 0;

//fillRect(x,pY,width+gap,height,textbgcolor);

for(int16_t i=0; i<height; i++)
{
  if (textcolor != textbgcolor) {
    if (textsize == 1) drawFastHLine(x, pY, width+gap, textbgcolor);
    else fillRect(x, pY, (width+gap)*textsize, textsize, textbgcolor);
  }
  for (int16_t k = 0;k < w; k++)
  { 
    line = pgm_read_byte(flash_address+w*i+k);
    if(line) {
      if (textsize==1){
        pX = x + k*8;
        if(line & 0x80) drawPixel(pX, pY, textcolor);
        if(line & 0x40) drawPixel(pX+1, pY, textcolor);
        if(line & 0x20) drawPixel(pX+2, pY, textcolor);
        if(line & 0x10) drawPixel(pX+3, pY, textcolor);
        if(line & 0x8) drawPixel(pX+4, pY, textcolor);
        if(line & 0x4) drawPixel(pX+5, pY, textcolor);
        if(line & 0x2) drawPixel(pX+6, pY, textcolor);
        if(line & 0x1) drawPixel(pX+7, pY, textcolor);
      }
       else {
        pX = x + k*8*textsize;
        if(line & 0x80) fillRect(pX, pY, textsize, textsize, textcolor);
        if(line & 0x40) fillRect(pX+textsize, pY, textsize, textsize, textcolor);
        if(line & 0x20) fillRect(pX+2*textsize, pY, textsize, textsize, textcolor);
        if(line & 0x10) fillRect(pX+3*textsize, pY, textsize, textsize, textcolor);
        if(line & 0x8) fillRect(pX+4*textsize, pY, textsize, textsize, textcolor);
        if(line & 0x4) fillRect(pX+5*textsize, pY, textsize, textsize, textcolor);
        if(line & 0x2) fillRect(pX+6*textsize, pY, textsize, textsize, textcolor);
        if(line & 0x1) fillRect(pX+7*textsize, pY, textsize, textsize, textcolor);
      }
    }
  }
  pY+=textsize;
}
return (width+gap)*textsize;        // x +
}

/***************************************************************************************
** Function name:           drawNumber unsigned with size
** Descriptions:            drawNumber
***************************************************************************************/
int16_t Adafruit_GFX_AS::drawNumber(long long_num,int16_t poX, int16_t poY, int16_t size)
{
    char tmp[10];
    if (long_num < 0) sprintf(tmp, "%li", long_num);
    else sprintf(tmp, "%lu", long_num);
    return drawString(tmp, poX, poY, size);
}

/***************************************************************************************
** Function name:           drawChar
** Descriptions:            draw char
***************************************************************************************/
int16_t Adafruit_GFX_AS::drawChar(char c, int16_t x, int16_t y, int16_t size)
{
    return drawUnicode(c, x, y, size);
}

/***************************************************************************************
** Function name:           drawString
** Descriptions:            draw string
***************************************************************************************/
int16_t Adafruit_GFX_AS::drawString(char *string, int16_t poX, int16_t poY, int16_t size)
{
    int16_t sumX = 0;

    while(*string)
    {
        int16_t xPlus = drawChar(*string, poX, poY, size);
        sumX += xPlus;
        *string++;
        poX += xPlus;                            /* Move cursor right       */
    }
    return sumX;
}

/***************************************************************************************
** Function name:           drawCentreString
** Descriptions:            draw string across centre
***************************************************************************************/
int16_t Adafruit_GFX_AS::drawCentreString(char *string, int16_t dX, int16_t poY, int16_t size)
{
    int16_t sumX = 0;
    int16_t len = 0;
    char *pointer = string;
    char ascii;

    while(*pointer)
    {
        ascii = *pointer;
        //if (size==0)len += 1+pgm_read_byte(widtbl_log+ascii);
        //if (size==1)len += 1+pgm_read_byte(widtbl_f8+ascii-32);
#ifdef LOAD_FONT2
        if (size==2)len += 1+pgm_read_byte(widtbl_f16+ascii-32);
#endif
        //if (size==3)len += 1+pgm_read_byte(widtbl_f48+ascii-32)/2;
#ifdef LOAD_FONT4
        if (size==4)len += pgm_read_byte(widtbl_f32+ascii-32)-3;
#endif
        //if (size==5) len += pgm_read_byte(widtbl_f48+ascii-32)-3;
#ifdef LOAD_FONT6
        if (size==6) len += pgm_read_byte(widtbl_f64+ascii-32)-3;
#endif
#ifdef LOAD_FONT7
        if (size==7) len += pgm_read_byte(widtbl_f7s+ascii-32)+2;
#endif
        *pointer++;
    }
    len = len*textsize;
    int16_t poX = dX - len/2;

    if (poX < 0) poX = 0;

    while(*string)
    {
        
        int16_t xPlus = drawChar(*string, poX, poY, size);
        sumX += xPlus;
        *string++;
        poX += xPlus;                  /* Move cursor right            */
    }
    
    return sumX;
}

/***************************************************************************************
** Function name:           drawRightString
** Descriptions:            draw string right justified
***************************************************************************************/
int16_t Adafruit_GFX_AS::drawRightString(char *string, int16_t dX, int16_t poY, int16_t size)
{
    int16_t sumX = 0;
    int16_t len = 0;
    char *pointer = string;
    char ascii;

    while(*pointer)
    {
        ascii = *pointer;
        //if (size==0)len += 1+pgm_read_byte(widtbl_log+ascii);
        //if (size==1)len += 1+pgm_read_byte(widtbl_f8+ascii-32);
#ifdef LOAD_FONT2
        if (size==2)len += 1+pgm_read_byte(widtbl_f16+ascii-32);
#endif
        //if (size==3)len += 1+pgm_read_byte(widtbl_f48+ascii-32)/2;
#ifdef LOAD_FONT4
        //if (size==4)len += pgm_read_byte(widtbl_f32+ascii-32)-3;
		if (size==4)len += pgm_read_byte(widtbl_f32+ascii-32);
#endif
        //if (size==5) len += pgm_read_byte(widtbl_f48+ascii-32)-3;
#ifdef LOAD_FONT6
        if (size==6) len += pgm_read_byte(widtbl_f64+ascii-32)-3;
#endif
#ifdef LOAD_FONT7
        if (size==7) len += pgm_read_byte(widtbl_f7s+ascii-32)+2;
#endif
        *pointer++;
    }
    
    len = len*textsize;
    int16_t poX = dX - len;

    if (poX < 0) poX = 0;

    while(*string)
    {
        
        int16_t xPlus = drawChar(*string, poX, poY, size);
        sumX += xPlus;
        *string++;
        poX += xPlus;          /* Move cursor right            */
    }
    
    return sumX;
}

/***************************************************************************************
** Function name:           drawFloat
** Descriptions:            drawFloat
***************************************************************************************/
int16_t Adafruit_GFX_AS::drawFloat(float floatNumber, int16_t decimal, int16_t poX, int16_t poY, int16_t size)
{
    unsigned long temp=0;
    float decy=0.0;
    float rounding = 0.5;
    
    float eep = 0.000001;
    
    int16_t sumX    = 0;
    int16_t xPlus   = 0;
    
    if(floatNumber-0.0 < eep)       // floatNumber < 0
    {
        xPlus = drawChar('-',poX, poY, size);
        floatNumber = -floatNumber;

        poX  += xPlus; 
        sumX += xPlus;
    }
    
    for (unsigned char i=0; i<decimal; ++i)
    {
        rounding /= 10.0;
    }
    
    floatNumber += rounding;

    temp = (long)floatNumber;
    
    
    xPlus = drawNumber(temp,poX, poY, size);

    poX  += xPlus; 
    sumX += xPlus;

    if(decimal>0)
    {
        xPlus = drawChar('.',poX, poY, size);
        poX += xPlus;                            /* Move cursor right            */
        sumX += xPlus;
    }
    else
    {
        return sumX;
    }
    
    decy = floatNumber - temp;
    for(unsigned char i=0; i<decimal; i++)                                      
    {
        decy *= 10;                                /* for the next decimal         */
        temp = decy;                               /* get the decimal              */
        xPlus = drawNumber(temp,poX, poY, size);
        
        poX += xPlus;                              /* Move cursor right            */
        sumX += xPlus;
        decy -= temp;
    }
    return sumX;
}
