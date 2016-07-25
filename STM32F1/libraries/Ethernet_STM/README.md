W5x00 ethernet library for STM32F103 micro-controllers
----

That library has been ported from the **WIZnet WIZ_Ethernet_Library**, to STM32F103 microcontrollers.
The library supports **W5100** and **W5500** Ethernet controllers. The source code has been modified to support the **W5200** too, but has not been tested (yet) on a real W5200 controller.

Ported to STM32F103 on 23 Aug 2015 by **Vassilis Serasidis**

Library installation
----
### *Install the library globally* ###


* Unzip the file **Ethernet_STM-master.zip** into your Arduino IDE directory 

> arduino\libraries

and rename the folder **Ethernet_STM-master** to **Ethernet_STM**

In this case the library can be used from the 8-bit and 32-bit Arduino boards (UNO, Nano, DUE etc) and from STM32F1 microcontroller series (for example STM32F103).

#### *Install the library for using it only with STM32F1 series* ###
 
* Unzip the file **Ethernet_STM-master.zip** into your Arduino IDE directory

> ...arduino\hardware\Arduino_STM32\STM32F1\libraries

and rename the folder **Ethernet_STM-master** to **Ethernet_STM**


Selecting the Ethernet type controller (W5100, W5200 or W5500)
----
For selecting among those tree chips edit the file:
`Ethernet_STM\src\utility\w5100.h`
and comment-out only the line with the chip you want to use.



By default the **W5500** ethernet controller is selected.


```
//#define W5100_ETHERNET_SHIELD // Arduino Ethenret Shield and Compatibles ...
//#define W5200_ETHERNET_SHIELD // WIZ820io, W5200 Ethernet Shield 
#define W5500_ETHERNET_SHIELD   // WIZ550io, ioShield series of WIZnet
```
If you edit the **w5100.h** file, save it and re-compile your sketch.

Selecting W5500 Sockets number and RAM buffer amount
----
For selecting the Sockets number (1-8) and the RAM buffer amount (1-16 kBytes) for each Socket, edit the file:
`Ethernet_STM\src\utility\w5500.h`
and change the values you want.

```
Total RAM buffer is 16 kBytes for Transmitter and 16 kBytes for receiver for 1 Socket.
The Total W5500 RAM buffer is 32 kBytes (16 + 16).
If you use more Sockets then the RAM buffer must be split.
For example: if you use 2 Sockets then all socket must use upto 16 kBytes in total RAM.
So, we have: 
 
   #define MAX_SOCK_NUM 2   // Select two Sockets.
   #define RXBUF_SIZE   8   // The Receiver buffer size will be 8 kBytes
   #define TXBUF_SIZE   8   // The Transmitter buffer size will be 8 kBytes
 
In total we use (2 Sockets)*(8 kBytes) for transmitter + (2 Sockets)*(8 kBytes) for receiver = 32 kBytes.
  
I would prefer to use only 1 Socket with 16 kBytes RAM buffer for transmitter and 16 kByte for receiver buffer.
 
   #define MAX_SOCK_NUM 1   // Select only one Socket
   #define RXBUF_SIZE   16  // Select 16 kBytes Receiver RAM buffer
   #define TXBUF_SIZE   16  // Select 16 kBytes Transmitter RAM buffer
```

Default settings:

```
#define MAX_SOCK_NUM 8   // Select the number of Sockets (1-8)
#define RXBUF_SIZE   2   // Select the Receiver buffer size (1 - 16 kBytes)
#define TXBUF_SIZE   2   // Select the Transmitter buffer size (1 - 16 kBytes)
```
If you edit the **w5500.h** file, save it and re-compile your sketch.

Using the Ethernet_STM library
----
1. Choose the desired ethernet chip you want to use (W5100, W5200 or W5500) from the w5100.h file
2. include this library in your sketch  

`#include <Ethernet_STM.h>`

All functions / commands have the same syntax with the stock Arduino Ethernet library. 


PIN Connections
----
|W5x00|STM32F103|
|:------:|:-----:|
|SS|PA4|
|SCK|PA5|
|MISO|PA6|
|MOSI|PA7|



**DO NOT FORGET TO ADD A RESISTOR 4k7 BETWEEN RESET AND 3.3V (OR RESET AND 5V) PINS ON ETHERNET SHIELD**

![alt tag](http://www.serasidis.gr/images/w5100_shield_1.jpg)
![alt tag](http://www.serasidis.gr/images/w5100_module_2.jpg)
![alt tag](http://www.serasidis.gr/images/w5100_module_1.jpg)

