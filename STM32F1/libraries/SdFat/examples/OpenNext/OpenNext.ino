/*
 * Print size, modify date/time, and name for all files in root.
 */
#include <SPI.h>
#include "SdFat.h"

// SD default chip select pin.
const uint8_t chipSelect = SS;

// file system object
SdFat sd;

SdFile file;
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  
  // Wait for USB Serial 
  while (!Serial) {
    SysCall::yield();
  }
  
  Serial.println("Type any character to start");
  while (!Serial.available()) {
    SysCall::yield();
  }

  // Initialize at the highest speed supported by the board that is
  // not over 50 MHz. Try a lower speed if SPI errors occur.
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();
  }

  // Open next file in root.  The volume working directory, vwd, is root.
  // Warning, openNext starts at the current position of sd.vwd() so a
  // rewind may be neccessary in your application.
  sd.vwd()->rewind();
  while (file.openNext(sd.vwd(), O_READ)) {
    file.printFileSize(&Serial);
    Serial.write(' ');
    file.printModifyDateTime(&Serial);
    Serial.write(' ');
    file.printName(&Serial);
    if (file.isDir()) {
      // Indicate a directory.
      Serial.write('/');
    }
    Serial.println();
    file.close();
  }
  Serial.println("Done!");
}
//------------------------------------------------------------------------------
void loop() {}
