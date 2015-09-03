#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
 
LiquidCrystal_I2C lcd1(0x20,16,2);  // set the LCD address of the first lcd to 0x20 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd2(0x21,16,2);  // set the LCD address of the second lcd to 0x21 for a 16 chars and 2 line display
 
void setup()
{
  lcd1.init();                      // initialize the first lcd 
  lcd2.init();                      // initialize the second lcd 
 
  // Print a message on the first LCD.
  lcd1.backlight();
  lcd1.print("Hello, #1 world!");
 
  // Print a message on the second LCD.
  lcd2.backlight();
  lcd2.print("Hello, #2 world!");
 
}
 
void loop()
{
}
