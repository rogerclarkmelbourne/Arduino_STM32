STM32F3 files have been supplied by Frank-Michael Krause.

The API is not as up to date as in the STM F103 files.
It looks like this port is as the F103 files orginally were when I started work on the project.
The API is for pre- Arduino 1.0 and doesn't include a lot of functions in the 1.0 API, and the class inheritance will
be different in a number of places, most importantly in the Serial class. 
Specifically Stream.cpp / .h are not present in this port.



-------------------- Details from Frank-Michael ----------------------------

This port of libmaple for Arduino-IDE is based on the inofficial F3 branch 
of libmaple found on 
https://github.com/ventosus/libmaple/tree/F3
and supports at the moment the STM32F3Discovery board with programming via ST-link.
Linker is configured for linking to flash, not to RAM

Changes by Frank-Michael Krause (mich dot krause at web dot de):

- renaming some stm32f3 related files because the Arduino build system copy all objects to same directory so the source file names must be different
- selected the correct startup modules and linker scripts for board variant stm32f3discovery

Notes:

- the pin names as noted on stm32f3discovery board (PA0, PA1, ..., PF10) can be used as pin id
- usb is untested because I haven't used it really
- There are some libraries and examples related to the STM32F3discovery board. 
  They are commented mostly in german only because I wrote them for use at robotic club of a secondary school 
  and the kids like to have the documentation in german.
  
  gyro: Interface library for the gyro-chip of the stm32f3discovery, setup the gyro in FIFO mode and so 
        requiring update calls only every < 120ms
  lsm303: Interface library for Acceleration sensor of the stm32f3discovery, magnetometer  not yet supported (writing this driver is currently under work by a student at school)
  ringbuffer: Class impelentig a ringbuffer with functions for adding time stampes per added element, averaging functions, accessing the stored data like an array
  roboter: several functionalities needed to program a robot:
            - dc motor control as known from Lego NXC language
            - support for HC-SR04 ultrasonic sensors driven by interrupt or blocking direct measurement
			- function timers based on timer interrupt
			- modul for interfacing a 24Cxx I2C eeprom chip





