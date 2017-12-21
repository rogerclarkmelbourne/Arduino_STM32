The Arduino SdFat library provides read/write access to FAT16/FAT32
file systems on SD/SDHC flash cards.

SdFat requires Arduino 1.6x or greater.

Key changes:

The SPI divisor has been replaced by SPISettings in the begin() call.

```
bool begin(uint8_t csPin = SS, SPISettings spiSettings = SPI_FULL_SPEED);
```

Several macros have been defined for backward compatibility. 

```
#define SD_SCK_MHZ(maxMhz) SPISettings(1000000UL*maxMhz, MSBFIRST, SPI_MODE0)
// SPI divisor constants
/** Set SCK to max possible rate. */
#define SPI_FULL_SPEED SD_SCK_MHZ(50)
/** Set SCK rate to F_CPU/3 for Due */
#define SPI_DIV3_SPEED SD_SCK_HZ(F_CPU/3)
/** Set SCK rate to F_CPU/4. */
#define SPI_HALF_SPEED SD_SCK_HZ(F_CPU/4)
// ...
```

There are two new classes, SdFatEX and SdFatSoftSpiEX.

Teensy 3.5/3.6 SDIO support has been added.  Try the TeensySdioDemo example.
Many other example will work with Teensy SDIO if you use the SdFatSdio classes
and call begin with no parameters.

```
 SdFatSdio sd;
 
 ....
 
  if (!sd.begin()) {
    // Handle failure.
  }
 
```
Please read changes.txt and the html documentation in the extras folder for more information.

Please report problems as issues.

A number of configuration options can be set by editing SdFatConfig.h
define macros.  See the html documentation for details

Please read the html documentation for this library.  Start with
html/index.html and read the Main Page.  Next go to the Classes tab and
read the documentation for the classes SdFat, SdFatEX, SdBaseFile,
SdFile, File, StdioStream, ifstream, ofstream, and others.
 
Please continue by reading the html documentation.

Updated 26 Apr 2017
