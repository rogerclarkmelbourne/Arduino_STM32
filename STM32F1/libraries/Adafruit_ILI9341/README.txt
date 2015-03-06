This library is based on the Adafruit ILI9341 (see original Adafuit text description below)


It has minor modifications to support STM32 and also one small change to use spi.write(byte) instead of spi.transfer(byte) as this gave
a useful speed improvement.

It has been tested with standard ILI9341 from various suppliers e.g on eBay

This library requires the Adafruit GFC library, https://github.com/adafruit/Adafruit-GFX-Library

An addition example stm32_graphicstest has been added to show how to configure for stm32 - which uses hardware SPI and hence its not possible to 
set the SCK MISO and MOSI pins.

_________________________  Original text from Adafruit ____________________________________________



This is a library for the Adafruit ILI9341 display products

This library works with the Adafruit 2.8" Touch Shield V2 (SPI)
  ----> http://www.adafruit.com/products/1651
 
Check out the links above for our tutorials and wiring diagrams.
These displays use SPI to communicate, 4 or 5 pins are required
to interface (RST is optional).

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution

To download. click the DOWNLOADS button in the top right corner, rename the uncompressed folder Adafruit_ILI9341. Check that the Adafruit_ILI9341 folder contains Adafruit_ILI9341.cpp and Adafruit_ILI9341.

Place the Adafruit_ILI9341 library folder your arduinosketchfolder/libraries/ folder. You may need to create the libraries subfolder if its your first library. Restart the IDE

Also requires the Adafruit_GFX library for Arduino.
