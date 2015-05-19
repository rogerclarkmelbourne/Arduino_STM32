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

Adafruit_GFX::Adafruit_GFX(int16_t w, int16_t h):
  WIDTH(w), HEIGHT(h)
{
  _width    = WIDTH;
  _height   = HEIGHT;
  rotation  = 0;
  cursor_y  = cursor_x    = 0;
  textsize  = 1;
  textcolor = textbgcolor = 0xFFFF;
  wrap      = true;
}

// Draw a circle outline
void Adafruit_GFX::drawCircle(int16_t x0, int16_t y0, int16_t r,
    uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0  , y0+r, color);
  drawPixel(x0  , y0-r, color);
  drawPixel(x0+r, y0  , color);
  drawPixel(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void Adafruit_GFX::drawCircleHelper( int16_t x0, int16_t y0,
               int16_t r, uint8_t cornername, uint16_t color) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    } 
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void Adafruit_GFX::fillCircle(int16_t x0, int16_t y0, int16_t r,
			      uint16_t color) {
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void Adafruit_GFX::fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
    uint8_t cornername, int16_t delta, uint16_t color) {

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

// Bresenham's algorithm - thx wikpedia
void Adafruit_GFX::drawLine(int16_t x0, int16_t y0,
			    int16_t x1, int16_t y1,
			    uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
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

  for (; x0<=x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// Draw a rectangle
void Adafruit_GFX::drawRect(int16_t x, int16_t y,
			    int16_t w, int16_t h,
			    uint16_t color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}

void Adafruit_GFX::drawFastVLine(int16_t x, int16_t y,
				 int16_t h, uint16_t color) {
  // Update in subclasses if desired!
  drawLine(x, y, x, y+h-1, color);
}

void Adafruit_GFX::drawFastHLine(int16_t x, int16_t y,
				 int16_t w, uint16_t color) {
  // Update in subclasses if desired!
  drawLine(x, y, x+w-1, y, color);
}

void Adafruit_GFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
			    uint16_t color) {
  // Update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++) {
    drawFastVLine(i, y, h, color);
  }
}

void Adafruit_GFX::fillScreen(uint16_t color) {
  fillRect(0, 0, _width, _height, color);
}

// Draw a rounded rectangle
void Adafruit_GFX::drawRoundRect(int16_t x, int16_t y, int16_t w,
  int16_t h, int16_t r, uint16_t color) {
  // smarter version
  drawFastHLine(x+r  , y    , w-2*r, color); // Top
  drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  drawFastVLine(x    , y+r  , h-2*r, color); // Left
  drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r    , y+r    , r, 1, color);
  drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

// Fill a rounded rectangle
void Adafruit_GFX::fillRoundRect(int16_t x, int16_t y, int16_t w,
				 int16_t h, int16_t r, uint16_t color) {
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

// Draw a triangle
void Adafruit_GFX::drawTriangle(int16_t x0, int16_t y0,
				int16_t x1, int16_t y1,
				int16_t x2, int16_t y2, uint16_t color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void Adafruit_GFX::fillTriangle ( int16_t x0, int16_t y0,
				  int16_t x1, int16_t y1,
				  int16_t x2, int16_t y2, uint16_t color) {

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

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1,
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }
}

void Adafruit_GFX::drawBitmap(int16_t x, int16_t y,
			      const uint8_t *bitmap, int16_t w, int16_t h,
			      uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
	drawPixel(x+i, y+j, color);
      }
    }
  }
}

//#if ARDUINO >= 100
size_t Adafruit_GFX::write(uint8_t c) {
//#else
//void Adafruit_GFX::write(uint8_t c) {
//#endif
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize*6;
    if (wrap && (cursor_x > (_width - textsize*6))) {
      cursor_y += textsize*8;
      cursor_x = 0;
    }
  }
//#if ARDUINO >= 100
  return 1;
//#endif
}

// Draw a character
void Adafruit_GFX::drawChar(int16_t x, int16_t y, unsigned char c,
			    uint16_t color, uint16_t bg, uint8_t size) {
#ifdef LOAD_GLCD
  if((x >= _width)            || // Clip right
     (y >= _height)           || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;

  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5) 
      line = 0x0;
    else 
      line = pgm_read_byte(font+(c*5)+i);
    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, color);
        else {  // big size
          fillRect(x+(i*size), y+(j*size), size, size, color);
        } 
      } else if (bg != color) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, bg);
        else {  // big size
          fillRect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
#endif
}

void Adafruit_GFX::setCursor(int16_t x, int16_t y) {
  cursor_x = x;
  cursor_y = y;
}

void Adafruit_GFX::setTextSize(uint8_t s) {
  textsize = (s > 0) ? s : 1;
}

void Adafruit_GFX::setTextColor(uint16_t c) {
  // For 'transparent' background, we'll set the bg 
  // to the same as fg instead of using a flag
  textcolor = textbgcolor = c;
}

void Adafruit_GFX::setTextColor(uint16_t c, uint16_t b) {
  textcolor   = c;
  textbgcolor = b; 
}

void Adafruit_GFX::setTextWrap(boolean w) {
  wrap = w;
}

uint8_t Adafruit_GFX::getRotation(void) {
  return rotation;
}

void Adafruit_GFX::setRotation(uint8_t x) {
  rotation = (x & 3);
  switch(rotation) {
   case 0:
   case 2:
    _width  = WIDTH;
    _height = HEIGHT;
    break;
   case 1:
   case 3:
    _width  = HEIGHT;
    _height = WIDTH;
    break;
  }
}

// Return the size of the display (per current rotation)
int16_t Adafruit_GFX::width(void) {
  return _width;
}
 
int16_t Adafruit_GFX::height(void) {
  return _height;
}

void Adafruit_GFX::invertDisplay(boolean i) {
  // Do nothing, must be subclassed if supported
}

/***************************************************************************************
** Function name:           drawUnicode
** Descriptions:            draw a unicode
***************************************************************************************/
int16_t Adafruit_GFX::drawUnicode(uint16_t uniCode, int16_t x, int16_t y, int16_t size)
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
int16_t Adafruit_GFX::drawNumber(long long_num,int16_t poX, int16_t poY, int16_t size)
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
int16_t Adafruit_GFX::drawChar(char c, int16_t x, int16_t y, int16_t size)
{
    return drawUnicode(c, x, y, size);
}

/***************************************************************************************
** Function name:           drawString
** Descriptions:            draw string
***************************************************************************************/
int16_t Adafruit_GFX::drawString(char *string, int16_t poX, int16_t poY, int16_t size)
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
int16_t Adafruit_GFX::drawCentreString(char *string, int16_t dX, int16_t poY, int16_t size)
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
int16_t Adafruit_GFX::drawRightString(char *string, int16_t dX, int16_t poY, int16_t size)
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
int16_t Adafruit_GFX::drawFloat(float floatNumber, int16_t decimal, int16_t poX, int16_t poY, int16_t size)
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
