// OLED_I2C_Bitmap
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

extern uint8_t logo[];
extern uint8_t mice[];

void setup()
{
  myOLED.begin();
}

void loop()
{
  myOLED.clrScr();
  myOLED.drawBitmap(0, 16, logo, 128, 36);
  myOLED.update();
  delay(4000);
  for (int i=0; i<2; i++)
  {
    myOLED.invert(true);
    delay(500);
    myOLED.invert(false);
    delay(500);
  }
  delay(4000);

  myOLED.clrScr();
  myOLED.drawBitmap(0, 0, mice, 128, 64);
  myOLED.update();
  delay(4000);
  for (int i=0; i<2; i++)
  {
    myOLED.invert(true);
    delay(500);
    myOLED.invert(false);
    delay(500);
  }
  delay(4000);
}


