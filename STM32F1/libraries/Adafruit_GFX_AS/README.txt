This library adds a few font-related functions on top of the Adafruit_GFX library (which you need to install, separately).

Historically, Adafruit_GFX_AS started as a fork of Adafruit_GFX. Most importantly, it added support for using additional fonts,
before similar functionality became available in Adafruit_GFX.

Today, the main point of this library is to continue supporting projects that have made use of the functions added in Adafruit_GFX_AS,
while also being fully compatible with Adafruit_GFX (which is used as the base class). It is not recommended to use the functions
provided by the Adafruit_GFX_AS class in newly developed code.

To use this library with a driver that is not based on Adafruit_GFX_AS, all you will have to do is to replace "Adafrui_GFX" with "Adafruit_GFX_AS"
in your driver code. This will usually be three places: The #include-directive, the base-class declaration, and the call to the base-contstructor.

Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

Adafruit_GFX Written by Limor Fried/Ladyada for Adafruit Industries.
Adafruit_GFX_AS funtions added by "Bodmer"(?)
BSD license, check license.txt for more information.
All text above must be included in any redistribution.

To download, click the DOWNLOAD ZIP button, uncompress and rename the uncompressed folder Adafruit_GFX_AS. Confirm that the Adafruit_GFX_AS folder contains Adafruit_GFX_AS.cpp and Adafruit_GFX_AS.h

Place the Adafruit_GFX_AS library folder your <arduinosketchfolder>/Libraries/ folder. You may need to create the Libraries subfolder if its your first library. Restart the IDE.
