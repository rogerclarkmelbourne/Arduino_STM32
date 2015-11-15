// Driver for 128x634 graphical LCD with ST7920 controller
// D Crocker, Escher Technologies Ltd.
// adapted for STM32-Arduino by Matthias Diro

#include "lcd7920_STM.h"
#include <pins_arduino.h>
#include <avr/interrupt.h>

// LCD basic instructions. These all take 72us to execute except LcdDisplayClear, which takes 1.6ms
const uint8_t LcdDisplayClear = 0x01;
const uint8_t LcdHome = 0x02;
const uint8_t LcdEntryModeSet = 0x06;       // move cursor right and indcement address when writing data
const uint8_t LcdDisplayOff = 0x08;
const uint8_t LcdDisplayOn = 0x0C;          // add 0x02 for cursor on and/or 0x01 for cursor blink on
const uint8_t LcdFunctionSetBasicAlpha = 0x20;
const uint8_t LcdFunctionSetBasicGraphic = 0x22;
const uint8_t LcdFunctionSetExtendedAlpha = 0x24;
const uint8_t LcdFunctionSetExtendedGraphic = 0x26;
const uint8_t LcdSetDdramAddress = 0x80;    // add the address we want to set

// LCD extended instructions
const uint8_t LcdSetGdramAddress = 0x80;

//const unsigned int LcdCommandDelayMicros = 72 - 24; // 72us required, less 24us time to send the command @ 1MHz
const unsigned int LcdCommandDelayMicros = 2;
const unsigned int LcdDataDelayMicros = 2;// 10;         // Delay between sending data bytes
const unsigned int LcdDisplayClearDelayMillis = 2;  // 1.6ms should be enough

const unsigned int numRows = 64;
const unsigned int numCols = 128;

Lcd7920::Lcd7920(uint8_t cPin, uint8_t dPin, bool spi) : clockPin(cPin), dataPin(dPin), useSpi(spi), currentFont(0), textInverted(false)
{
}

size_t Lcd7920::write(uint8_t ch)
{
  if (gfxMode)
  {
    if (currentFont == 0)
    { 
      return 0; 
    }
    const uint8_t startChar = pgm_read_byte_near(&(currentFont->startCharacter));
    const uint8_t endChar = pgm_read_byte_near(&(currentFont->endCharacter));
    
    if (ch < startChar || ch > endChar)
    {
      return 0;
    }
    
    const uint8_t fontWidth = pgm_read_byte_near(&(currentFont->width));
    const uint8_t fontHeight = pgm_read_byte_near(&(currentFont->height));
    const uint8_t bytesPerColumn = (fontHeight + 7)/8;
    const uint8_t bytesPerChar = (bytesPerColumn * fontWidth) + 1;
    PROGMEM const uint8_t  *fontPtr = (PROGMEM const uint8_t*)pgm_read_word_near(&(currentFont->ptr)) + (bytesPerChar * (ch - startChar));
    uint16_t cmask = (1 << fontHeight) - 1;
    
    uint8_t nCols = pgm_read_byte_near(fontPtr++);

    // Update dirty rectangle coordinates, except for endCol which we do at the end      
    {
      if (startRow > row) { startRow = row; }
      if (startCol > column) { startCol = column; }
      uint8_t nextRow = row + fontHeight;
      if (nextRow > numRows) { nextRow = numRows; }
      if (endRow < nextRow) { endRow = nextRow; }
    }

    // Decide whether to add a space column first (auto-kerning)
    // We don't add a space column before a space character.
    // We add a space column after a space character if we would have added one between the preceding and following characters.
    if (column != numCols)
    {
      uint16_t thisCharColData = pgm_read_word_near(fontPtr) & cmask;    // atmega328p is little-endian
      if (thisCharColData == 0)  // for characters with deliberate space row at the start, e.g. decimal point
      {
        thisCharColData = pgm_read_word_near(fontPtr + 2) & cmask;
      }
      bool wantSpace = ((thisCharColData | (thisCharColData << 1)) & (lastCharColData | (lastCharColData << 1))) != 0;
      if (wantSpace)
      {
        // Add space after character
        uint8_t mask = 0x80 >> (column & 7);
        uint8_t *p = image + ((row * (numCols/8)) + (column/8));
        for (uint8_t i = 0; i < fontHeight && p < (image + sizeof(image)); ++i)
        {
          if (textInverted)
          {
            *p |= mask;
          }
          else
          {
            *p &= ~mask;
          }
          p += (numCols/8);
        }
        ++column;
      }      
    }
    
    while (nCols != 0 && column < numCols)
    {
      uint16_t colData = pgm_read_word_near(fontPtr);
      fontPtr += bytesPerColumn;
      if (colData != 0)
      {
        lastCharColData = colData & cmask;
      }
      uint8_t mask1 = 0x80 >> (column & 7);
      uint8_t mask2 = ~mask1;
      uint8_t *p = image + ((row * (numCols/8)) + (column/8));
      const uint16_t setPixelVal = (textInverted) ? 0 : 1;
      for (uint8_t i = 0; i < fontHeight && p < (image + sizeof(image)); ++i)
      {
        if ((colData & 1) == setPixelVal)
        {
          *p |= mask1;      // set pixel
        }
        else
        {
          *p &= mask2;     // clear pixel
        }
        colData >>= 1;
        p += (numCols/8);
      }
      --nCols;
      ++column;
    }
    
    if (column > endCol) { endCol = column; }
  }
  else
  {
    // Alphanumeric mode
    if (ch == '\r')
    {
      setCursor(0, column);
    }
    else if (ch == '\n')
    {
      setCursor(0, row + 1);
    }
    else
    {
      if (column == 16)
      {
        setCursor(row + 1, 0);
      }
      sendLcdData(ch);
      ++column;
      commandDelay();        // so that we are ready to write another character immediately
    }
  }
  return 1;
}

// Select normal or inverted text (only works in graphics mode)
void Lcd7920::textInvert(bool b)
{
  if (b != textInverted)
  {
    textInverted = b;
    lastCharColData = 0xFFFF;    // always need space between inverted and non-inverted text
  }
}

// NB - if using SPI then the SS pin must be set to be an output before calling this!
void Lcd7920::begin(bool gmode)
{
  
  // Set up the SPI interface for talking to the LCD. We have to set MOSI, SCLK and SS to outputs, then enable SPI.
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  if (useSpi)
  {
      delay(1); //dummy
   // SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);  // enable SPI, master mode, clock low when idle, data sampled on rising edge, clock = f/16 (= 1MHz), send MSB first
    //SPSR = (1 << SPI2X);  // double the speed to 2MHz (optional)
  }

  gfxMode = false;
  sendLcdCommand(LcdFunctionSetBasicAlpha);
  delay(1);
  sendLcdCommand(LcdFunctionSetBasicAlpha);
  commandDelay();
  sendLcdCommand(LcdEntryModeSet);
  commandDelay();
  extendedMode = false;

  clear();    // clear alpha ram
  if (gmode)
  {
    gfxMode = true;
    clear();  // clear gfx ram
  }
  setCursor(0, 0);
  sendLcdCommand(LcdDisplayOn);
  commandDelay();
  currentFont = 0;
  textInverted = false;
}

void Lcd7920::setFont(const PROGMEM LcdFont *newFont)
{
  currentFont = newFont;
}

void Lcd7920::clear()
{
  if (gfxMode)
  {
    memset(image, 0, sizeof(image));
    // flag whole image as dirty and update
    startRow = 0;
    endRow = numRows;
    startCol = 0;
    endCol = numCols;
    flush();
  }
  else
  {
    ensureBasicMode();
    sendLcdCommand(LcdDisplayClear);
    delay(LcdDisplayClearDelayMillis);
  }
  setCursor(0, 0);
  textInverted = false;
}

// Draw a line using the Bresenham Algorithm (thanks Wikipedia)
void Lcd7920::line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, PixelMode mode)
{
  int dx = (x1 >= x0) ? x1 - x0 : x0 - x1;
  int dy = (y1 >= y0) ? y1 - y0 : y0 - y1;
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;
 
  for (;;)
  {
    setPixel(x0, y0, mode);
    if (x0 == x1 && y0 == y1) break;
    int e2 = err + err;
    if (e2 > -dy)
    { 
      err -= dy;
      x0 += sx;
    }
    if (e2 < dx)
    { 
      err += dx;
      y0 += sy;
    }
  }
}
void Lcd7920::fastHline(uint8_t x0, uint8_t y0, uint8_t x1, PixelMode mode)
{
    for (byte xx=0;xx< x1;xx++)
    {
    setPixel(x0+xx, y0, mode);
    }
}
void Lcd7920::fastVline(uint8_t x0, uint8_t y0, uint8_t y1, PixelMode mode)
{
    for (byte yy=0;yy< y1;yy++)
    {
        setPixel(x0, y0+yy, mode);
    }
}
// draw simple box - x0,y0=start, x1,y1-length in pixel
void Lcd7920::box(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, PixelMode mode)
{
    //line( x0,  y0,  x0+x1, y0,  mode);
    fastHline(x0,y0,x1,  mode);
    fastHline(x0,y0+y1,x1,  mode);
   // line( x0,  y0+y1,  x0+x1, y0+y1,  mode);
    //line( x0,  y0+1,  x0, y0+y1-1,  mode);
    fastVline(x0,y0+1,y1-1,  mode);
    fastVline(x0+x1-1,y0+1,y1-1,  mode);
    //line( x0+x1,  y0+1,  x0+x1, y0+y1-1,  mode);
    //line( x0,  y0,  x0+x1, y0+y1,  mode);
}

//draw full box
void Lcd7920::fillbox(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, PixelMode mode)
{
    for (byte x=0;x<y1;x++)
    {
     //line( x0,  y0+x,  x0+x1, y0+x,  mode);
        fastHline(x0,y0+x,x1,  mode);
    }
}

// Draw a circle using the Bresenham Algorithm (thanks Wikipedia)
void Lcd7920::circle(uint8_t x0, uint8_t y0, uint8_t radius, PixelMode mode)
{
  int f = 1 - (int)radius;
  int ddF_x = 1;
  int ddF_y = -2 * (int)radius;
  int x = 0;
  int y = radius;
 
  setPixel(x0, y0 + radius, mode);
  setPixel(x0, y0 - radius, mode);
  setPixel(x0 + radius, y0, mode);
  setPixel(x0 - radius, y0, mode);
 
  while(x < y)
  {
    // keep ddF_x == 2 * x + 1;
    // keep ddF_y == -2 * y;
    // keep f == x*x + y*y - radius*radius + 2*x - y + 1;
    if(f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;    
    setPixel(x0 + x, y0 + y, mode);
    setPixel(x0 - x, y0 + y, mode);
    setPixel(x0 + x, y0 - y, mode);
    setPixel(x0 - x, y0 - y, mode);
    setPixel(x0 + y, y0 + x, mode);
    setPixel(x0 - y, y0 + x, mode);
    setPixel(x0 + y, y0 - x, mode);
    setPixel(x0 - y, y0 - x, mode);
  }
}

// Draw a bitmap
void Lcd7920::bitmap(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, PROGMEM const uint8_t data[])
{
  for (uint8_t r = 0; r < height && r + y0 < numRows; ++r)
  {
    uint8_t *p = image + (((r + y0) * (numCols/8)) + (x0/8));
    uint16_t bitMapOffset = r * (width/8);
    for (uint8_t c = 0; c < (width/8) && c + (x0/8) < numCols/8; ++c)
    {
      *p++ = pgm_read_byte_near(bitMapOffset++);
    }
  }
  if (x0 < startCol) startCol = x0;
  if (x0 + width > endCol) endCol = x0 + width;
  if (y0 < startRow) startRow = y0;
  if (y0 + height > endRow) endRow = y0 + height;
}

// Flush the dirty part of the image to the lcd
void Lcd7920::flush()
{
  if (gfxMode && endCol > startCol && endRow > startRow)
  {
    uint8_t startColNum = startCol/16;
    uint8_t endColNum = (endCol + 15)/16;
    for (uint8_t r = startRow; r < endRow; ++r)
    {
      setGraphicsAddress(r, startColNum);
      uint8_t *ptr = image + ((16 * r) + (2 * startColNum));
      for (uint8_t i = startColNum; i < endColNum; ++i)
      {      
        sendLcdData(*ptr++);
        //commandDelay();    // don't seem to need a delay here
        sendLcdData(*ptr++);
        //commandDelay();    // don't seem to need as long a delay as this
        delayMicroseconds(LcdDataDelayMicros);
      }
    }
    startRow = numRows;
    startCol = numCols;
    endCol = endRow = 0;
  }
}

// Set the cursor position. We can only set alternate columns. The row addressing is rather odd.
void Lcd7920::setCursor(uint8_t r, uint8_t c)
{
  if (gfxMode)
  {
    row = r % numRows;
    column = c % numCols;
    lastCharColData = 0u;    // flag that we just set the cursor position, so no space before next character
  }
  else
  {
    row = r % 4;
    column = c % 16;
    ensureBasicMode();
    sendLcdCommand(LcdSetDdramAddress + ((row & 1) * 0x10) + (column/2) + (row >> 1) * 8);
    commandDelay();
  }
}

void Lcd7920::setPixel(uint8_t x, uint8_t y, PixelMode mode)
{
  if (y < numRows && x < numCols)
  {
    uint8_t *p = image + ((y * (numCols/8)) + (x/8));
    uint8_t mask = 0x80u >> (x%8);
    switch(mode)
    {
      case PixelClear:
        *p &= ~mask;
        break;
      case PixelSet:
        *p |= mask;
        break;
      case PixelFlip:
        *p ^= mask;
        break;
    }
    
    // Change the dirty rectangle to account for a pixel being dirty (we assume it was changed)
    if (startRow > y) { startRow = y; }
    if (endRow <= y)  { endRow = y + 1; }
    if (startCol > x) { startCol = x; }
    if (endCol <= x)  { endCol = x + 1; }
  }
}

bool Lcd7920::readPixel(uint8_t x, uint8_t y)
{
  if (y < numRows && x < numCols)
  {
    uint8_t *p = image + ((y * (numCols/8)) + (x/8));
    return (*p & (0x80u >> (x%8))) != 0;
  }
  return false;
}

void Lcd7920::setGraphicsAddress(unsigned int r, unsigned int c)
{
  if (gfxMode)
  {
    ensureExtendedMode();
    sendLcdCommand(LcdSetGdramAddress | (r & 31));
    //commandDelay();  // don't seem to need this one
    sendLcdCommand(LcdSetGdramAddress | c | ((r & 32) >> 2));
    commandDelay();    // we definitely need this one
  }
}

void Lcd7920::commandDelay()
{
  delayMicroseconds(LcdCommandDelayMicros);
}

// Send a command to the LCD
void Lcd7920::sendLcdCommand(uint8_t command)
{
  sendLcd(0xF8, command);
}

// Send a data byte to the LCD
void Lcd7920::sendLcdData(uint8_t data)
{
  sendLcd(0xFA, data);
}

// Send a command to the lcd. Data1 is sent as-is, data2 is split into 2 bytes, high nibble first.
void Lcd7920::sendLcd(uint8_t data1, uint8_t data2)
{
  if (useSpi)
  {
      delay(1); // dummy
      /*
    SPDR = data1;
    while ((SPSR & (1 << SPIF)) == 0) { }
    SPDR = data2 & 0xF0;
    while ((SPSR & (1 << SPIF)) == 0) { }
    SPDR = data2 << 4;
    while ((SPSR & (1 << SPIF)) == 0) { }
       */
  }
  else
  {
    sendLcdSlow(data1);
    sendLcdSlow(data2 & 0xF0);
    sendLcdSlow(data2 << 4);
  }
}

void Lcd7920::sendLcdSlow(uint8_t data)
{
    /*
#if 1

  // Fast shiftOut function
  volatile uint32_t *sclkPort = portOutputRegister(digitalPinToPort(clockPin));
  volatile uint32_t *mosiPort = portOutputRegister(digitalPinToPort(dataPin));
  uint8_t sclkMask = digitalPinToBitMask(clockPin);
  uint8_t mosiMask = digitalPinToBitMask(dataPin);
  
  uint8_t oldSREG = SREG;
  cli();
  for (uint8_t i = 0; i < 8; ++i)
  {
    if (data & 0x80)
    {
      *mosiPort |= mosiMask;
    }
    else
    {
      *mosiPort &= ~mosiMask;
    }
    *sclkPort |= sclkMask;
    *sclkPort &= ~sclkMask;
    data <<= 1;
  }
  SREG = oldSREG;
  
 #else
 */
  // really slow version, like Arduino shiftOut function
 
  for (uint8_t i = 0; i < 8; ++i)
  {
    digitalWrite(dataPin, (data & 0x80) ? HIGH : LOW);
    //  mosifast((data & 0x80) ? HIGH : LOW); // faster
   digitalWrite(clockPin, HIGH); // do not change to high speed!
   digitalWrite(clockPin, LOW); // do not change to high speed pin change!

    data <<= 1;
 
  }
/*
   
    for (uint8_t i = 0; i < 8; ++i)
    {
        mosifast((data & 0x80) ? HIGH : LOW);
       delayMicroseconds(1);
        sckfast(HIGH);
        sckfast(LOW);
        data <<= 1;
     }
   */
  
//#endif
}

void Lcd7920::ensureBasicMode()
{
  if (extendedMode)
  {
    sendLcdCommand(gfxMode ? LcdFunctionSetBasicGraphic : LcdFunctionSetBasicAlpha);
    commandDelay();
    extendedMode = false;
  }
}

void Lcd7920::ensureExtendedMode()
{
  if (!extendedMode)
  {
    sendLcdCommand(gfxMode ? LcdFunctionSetExtendedGraphic : LcdFunctionSetExtendedAlpha);
    commandDelay();
    extendedMode = true;
  }
}
