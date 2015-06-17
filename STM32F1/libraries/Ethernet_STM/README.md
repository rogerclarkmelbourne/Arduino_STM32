W5100 ethernet library for STM32F103 micro-controllers
----

That library has been ported from the stock Arduino **Ethernet** library, to STM32F103 microcontrollers.

Ported to STM32F103 on 16 Jun 2015 by Vassilis Serasidis
 
 
PIN Connections
----
|W5100|STM32F103|
|:------:|:-----:|
|SS|PA4|
|SCK|PA5|
|MISO|PA6|
|MOSI|PA7|

**DO NOT FORGET TO ADD A RESISTOR 4k7 BETWEEN RESET AND 3.3V (OR RESET AND 5V) PINS ON ETHERNET SHIELD**

![alt tag](http://www.serasidis.gr/images/w5100_shield.jpg)