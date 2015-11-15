LiquidCrystal_I2C V2.0

The LiquidCrystal_I2C library is a modified version of the standard LiquidCrystal library as found on 
the Arduino website.
This library is intended to be used when a parallel HD44780 compatible LCD is controlled over I2C using 
a PCF8574 extender (see datasheet for details).
4 of the 8 outputs are used for LDC data lines 4 to 7.
3 outputs are used for the Enable, register-select and Read/Write lines.
The one output left can be used to control the backlight of the LCD (if available).
For backlight control some extra resistors and a pnp-type transistor are required (for details see 
schematic diagram).

The PCF8574 extender is available in two versions, the PCF8574 and the PCF8574A.
The only difference between the two is the I2C base address.
The base address for the PCF8574 is 0x20 and the base address for the PCF8574A is 0x38.
The examples included in this zip file assume the use of an PCF8574 set for address 0x20 
(A0, A1 and A3 grounded).

For compatibility reasons this library contains some aliases for functions that are known under different 
names in other libraries. This should make it fairly easy to implement the library in existing sketches 
without changing to much code.
Functions not supported by this library will return nothing at all and in case a return value is expected 
the function will return 0.

Update 8-12-2011:
Due to the relaese of Arduino IDE 1.0 some changes were made to the library to get it working under the new IDE.
Because of these changes this version of the LiquidCrystal_I2C library can not be used for older IDE versions.
The old version of the LiquidCrystal_I2Clibrary can be downloaded form http://www.xs4all.nl/~hmario/arduino/LiquidCrystal_I2C/V1.0/LiquidCrystal_I2C_V1.0.zip

Download the latest version from:  
http://www.xs4all.nl/~hmario/arduino/LiquidCrystal_I2C/LiquidCrystal_I2C.zip
(Thanks to Ailton F. for beta testing.)


Mario H.
atmega@xs4all.nl