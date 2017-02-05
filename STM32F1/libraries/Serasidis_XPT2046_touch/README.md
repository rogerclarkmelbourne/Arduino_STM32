Description
---

A simple **XPT2046** Touch Screen library for STM32 micro-controllers.

* The first example sketch (**TouchTest.ino**) checks if the touch screen has been pressed and prints the X,Y coordinates on the Serial port 1.
* The second example sketch (**TouchButtons.ino**) creates some virtual buttons defined by the user.

Copyright (c) 03 December 2015 by **Vassilis Serasidis**

Home: http://www.serasidis.gr , https://github.com/Serasidis

The library is written for STM32duino (http://stm32duino.com)

```
 //Create 4*2=8 virtual buttons
 #define LINES     2 //Divide the touch screen into 2 lines
 #define COLUMNS   4 //Divide the touch screen into 4 columns

```


Screenshot of the Serial port 1 (TouchButtons.ino)
```
-------------------------------------------------
XPT2046 touch screen buttons
Copyright (c) 02 Dec 2015 by Vassilis Serasidis
Home: http://www.serasidis.gr
-------------------------------------------------
Button: 1
X: 1097
Y: 800

Button: 2
X: 3455
Y: 617

Button: 3
X: 684
Y: 1483

Button: 4
X: 3412
Y: 1198

Button: 5
X: 860
Y: 2395

Button: 6
X: 3353
Y: 2355

```

PIN Connections between XPT2046 and STM32F103
----
| XPT2046 | STM32F103 |
|:------:|:-----:|
|T_DO|PA6|
|T_DIN|PA7|
|T_CS|PA3|
|T_CLK|PA5|

Selecting the SPI port number and Chip Select pin
----
Can be used other SPI port than the default SPI1 port. Just select the SPI port you want by replacing the following line into the sketch:

```
SPIClass mySPI(1); //Create an SPI instance on SPI1 port.
//SPIClass mySPI(2); //Create an SPI instance on SPI2 port.
```

The Chip select pin can be defined by the user also.

```
#define CS_PIN  PA3  // The pin PA3 has been chosen as Chip Select pin.
```
