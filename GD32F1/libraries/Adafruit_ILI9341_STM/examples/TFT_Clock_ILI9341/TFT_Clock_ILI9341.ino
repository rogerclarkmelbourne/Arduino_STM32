/*
  An example analogue clock using a TFT LCD screen to show the time
 use of some of the drawing commands with the modified Adafruit_ILI9341_AS library.
 For a more accurate clock, it would be better to use the RTClib library.
 But this is just a demo. 
 
 This examples uses the hardware SPI only. Non-hardware SPI
 is just too slow (~8 times slower!)
 
 Gilchrist 6/2/2014 1.0
 Updated by Alan Senior 18/1/2015
 */

//#define sclk 6  // Don't change
//#define mosi 4  // Don't change
#define cs   PB4 
#define dc   PA15
#define rst  PB3  
// you can also connect this to the Arduino reset

#define TFT_CS         PB4                  //10          pin 3          10
#define TFT_DC         PA15                 // 9          pin 5           9
#define TFT_RST        PB3                  // 8          pin 4           8

#include <Adafruit_GFX_AS.h>    // Core graphics library, with extra fonts.
#include <Adafruit_ILI9341_STM.h> // STM32 DMA Hardware-specific library
#include <SPI.h>

#define ILI9341_GREY 0x5AEB

Adafruit_ILI9341_STM tft = Adafruit_ILI9341_STM(cs, dc, rst);       // Invoke custom library

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=120, osy=120, omx=120, omy=120, ohx=120, ohy=120;  // Saved H, M, S x & y coords
uint16_t x00=0, x11=0, y00=0, y11=0;
uint32_t targetTime = 0;                    // for next 1 second timeout
uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time
boolean initial = 1;

void setup(void) {
  tft.begin();
  tft.setRotation(2);
  
  //tft.fillScreen(ILI9341_BLACK);
  //tft.fillScreen(ILI9341_RED);
  //tft.fillScreen(ILI9341_GREEN);
  //tft.fillScreen(ILI9341_BLUE);
  //tft.fillScreen(ILI9341_BLACK);
  tft.fillScreen(ILI9341_GREY);
  
  tft.setTextColor(ILI9341_WHITE, ILI9341_GREY);  // Adding a background colour erases previous text automatically
  
  // Draw clock face
  tft.fillCircle(120, 120, 118, ILI9341_GREEN);
  tft.fillCircle(120, 120, 110, ILI9341_BLACK);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x00 = sx*114+120;
    y00 = sy*114+120;
    x11 = sx*100+120;
    y11 = sy*100+120;

    tft.drawLine(x00, y00, x11, y11, ILI9341_GREEN);
  }

  // Draw 60 dots
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x00 = sx*102+120;
    y00 = sy*102+120;
    // Draw minute markers
    tft.drawPixel(x00, y00, ILI9341_WHITE);
    
    // Draw main quadrant dots
    if(i==0 || i==180) tft.fillCircle(x00, y00, 2, ILI9341_WHITE);
    if(i==90 || i==270) tft.fillCircle(x00, y00, 2, ILI9341_WHITE);
  }

  tft.fillCircle(120, 121, 3, ILI9341_WHITE);

  // Draw text at position 120,260 using fonts 4
  // Only font numbers 2,4,6,7 are valid. Font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : . a p m
  // Font 7 is a 7 segment font and only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : .
  tft.drawCentreString("Time flies",120,260,4);

  targetTime = millis() + 1000; 
}

void loop() {
  if (targetTime < millis()) {
    targetTime = millis()+1000;
    ss++;              // Advance second
    if (ss==60) {
      ss=0;
      mm++;            // Advance minute
      if(mm>59) {
        mm=0;
        hh++;          // Advance hour
        if (hh>23) {
          hh=0;
        }
      }
    }

    // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = ss*6;                  // 0-59 -> 0-354
    mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
    hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
    hx = cos((hdeg-90)*0.0174532925);    
    hy = sin((hdeg-90)*0.0174532925);
    mx = cos((mdeg-90)*0.0174532925);    
    my = sin((mdeg-90)*0.0174532925);
    sx = cos((sdeg-90)*0.0174532925);    
    sy = sin((sdeg-90)*0.0174532925);

    if (ss==0 || initial) {
      initial = 0;
      // Erase hour and minute hand positions every minute
      tft.drawLine(ohx, ohy, 120, 121, ILI9341_BLACK);
      ohx = hx*62+121;    
      ohy = hy*62+121;
      tft.drawLine(omx, omy, 120, 121, ILI9341_BLACK);
      omx = mx*84+120;    
      omy = my*84+121;
    }

      // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
      tft.drawLine(osx, osy, 120, 121, ILI9341_BLACK);
      osx = sx*90+121;    
      osy = sy*90+121;
      tft.drawLine(osx, osy, 120, 121, ILI9341_RED);
      tft.drawLine(ohx, ohy, 120, 121, ILI9341_WHITE);
      tft.drawLine(omx, omy, 120, 121, ILI9341_WHITE);
      tft.drawLine(osx, osy, 120, 121, ILI9341_RED);

    tft.fillCircle(120, 121, 3, ILI9341_RED);
  }
}

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

