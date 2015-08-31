// OLED_I2C_ViewFont
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// A quick demo of how to use my OLED_I2C library.
//
// To use the hardware I2C (TWI) interface of the Arduino you must connect
// the pins as follows:
//
// Arduino Uno/2009:
// ----------------------
// Display:  SDA pin   -> Arduino Analog 4 or the dedicated SDA pin
//           SCL pin   -> Arduino Analog 5 or the dedicated SCL pin
//
// Arduino Leonardo:
// ----------------------
// Display:  SDA pin   -> Arduino Digital 2 or the dedicated SDA pin
//           SCL pin   -> Arduino Digital 3 or the dedicated SCL pin
//
// Arduino Mega:
// ----------------------
// Display:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA pin
//           SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL pin
//
// Arduino Due:
// ----------------------
// Display:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA1 (Digital 70) pin
//           SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL1 (Digital 71) pin
//
// The internal pull-up resistors will be activated when using the 
// hardware I2C interfaces.
//
// You can connect the OLED display to any available pin but if you use 
// any other than what is described above the library will fall back to
// a software-based, TWI-like protocol which will require exclusive access 
// to the pins used, and you will also have to use appropriate, external
// pull-up resistors on the data and clock signals.
//

#include <OLED_I2C.h>

OLED  myOLED(SDA, SCL, 8);

extern uint8_t SmallFont[];

void setup()
{
  myOLED.begin();
  myOLED.setFont(SmallFont);
}

void loop()
{
  myOLED.clrScr();
  myOLED.print("Upper case:", LEFT, 0);
  myOLED.print("ABCDEFGHIJKLM", CENTER, 16);
  myOLED.print("NOPQRSTUVWXYZ", CENTER, 24);
  myOLED.update();
  delay (5000);
  
  myOLED.clrScr();
  myOLED.print("Lower case:", LEFT, 0);
  myOLED.print("abcdefghijklm", CENTER, 16);
  myOLED.print("nopqrstuvwxyz", CENTER, 24);
  myOLED.update();
  delay (5000);
  
  myOLED.clrScr();
  myOLED.print("Numbers:", LEFT, 0);
  myOLED.print("0123456789", CENTER, 16);
  myOLED.update();
  delay (5000);
  
  myOLED.clrScr();
  myOLED.print("Special:", LEFT, 0);
  myOLED.print("!\"#$%&'()*+,-.", CENTER, 16);
  myOLED.print("/:;<=>?@[\\]^_`", CENTER, 24);
  myOLED.print("{|}~", CENTER, 32);
  myOLED.update();
  delay (5000);
}


