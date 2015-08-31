
// ST7920-based 128x64 pixel graphic LCD demo program
// Written by D. Crocker, Escher Technologies Ltd.
// adapted for STM32-arduino by Matthias Diro and added some fonts and new commands (box, fillbox, fastVline, fastHline)
// library doesn't use SPI or some low level pin toggles --> too fast for the display
// setup Display:
// PSB must be set to ground for serial mode: maybe you must unsolder a resistor!
// only two lines are needed: R/W = Mosi, E=SCK, just solder RS (Slave select) to +5V, it isn't needed!
// maple mini user: you must set VCC to +5V, but you don't have it onboard!
#include "lcd7920_STM.h"
// some fonts
//extern const PROGMEM LcdFont font10x10_STM;    // in glcd10x10.cpp
extern const PROGMEM LcdFont font5x7_STM;    // in glcd10x10.cpp
//extern const PROGMEM LcdFont font16x16_STM;    // in glcs16x16.cpp

// Define the following as true to use SPI to drive the LCD, false to drive it slowly instead
#define LCD_USE_SPI    (false) // no SPI yet for STM32

const int MOSIpin = 6;
const int SCLKpin = 7;

// LCD
static Lcd7920 lcd(SCLKpin, MOSIpin, LCD_USE_SPI);

// Initialization
void setup()
{
    lcd.begin(true);          // init lcd in graphics mode
    lcd.setFont(&font5x7_STM);
    lcd.setCursor(35, 0);
    lcd.textInvert(1);
    lcd.print("Counter: ");
    lcd.textInvert(0);
}

void loop()
{
    static byte counter = 0;
    //
    counter++;
    if (counter > 128)
    {
        counter = 0;
        delay(1000);
        lcd.clear();
        lcd.setCursor(35, 0);
        lcd.print("Counter: ");
    }
    
    lcd.box(0, 0, counter - 1, 5, PixelClear);
    lcd.box(0, 0, counter, 5, PixelSet);
    lcd.fillbox(0, 55, counter, 10, PixelSet);
    lcd.circle(100, 31, counter / 8, PixelSet);
    lcd.flush();
    lcd.setCursor(35, 55);
    lcd.print(counter);
    lcd.print(" ");
    
}

