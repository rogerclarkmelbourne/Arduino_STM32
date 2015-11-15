// OLED_I2C_NumberFonts
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// A quick demo of how to use my OLED_I2C library.
//
// To use the hardware I2C (TWI) interface of the chipKit you must connect
// the pins as follows:
//
// chipKit Uno32/uC32:
// ----------------------
// Display:  SDA pin   -> Analog 4
//           SCL pin   -> Analog 5
// *** Please note that JP6 and JP8 must be in the I2C position (closest to the analog pins)
//
// chipKit Max32:
// ----------------------
// Display:  SDA pin   -> Digital 20 (the pin labeled SDA)
//           SCL pin   -> Digital 21 (the pin labeled SCL)
//
// The chipKit boards does not have pull-up resistors on the hardware I2C interface
// so external pull-up resistors on the data and clock signals are required.
//
// You can connect the OLED display to any available pin but if you use any
// other than what is described above the library will fall back to
// a software-based, TWI-like protocol which will require exclusive access 
// to the pins used.
//

#include <OLED_I2C.h>

OLED  myOLED(SDA, SCL, 8);

extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

void setup()
{
  myOLED.begin();
  myOLED.setFont(SmallFont);
}

void loop()
{
  for (int i=0; i<=10000; i++)
  {
    myOLED.setFont(MediumNumbers);
    myOLED.printNumF(float(i)/3, 2, RIGHT, 0);
    myOLED.setFont(BigNumbers);
    myOLED.printNumI(i, RIGHT, 40);
    myOLED.update();
  }
  
  myOLED.setFont(SmallFont);
  myOLED.print("|", LEFT, 24);
  myOLED.print("|", RIGHT, 24);
  myOLED.update();
  delay(500);
  for (int i=0; i<19; i++)
  {
    myOLED.print("\\", 7+(i*6), 24);
    myOLED.update();
    delay(250);
  }
  myOLED.clrScr();
}


