// This uses the greiman sdfat library.
// To use SdFatEX, set ENABLE_EXTENDED_TRANSFER_CLASS to 1 in the library's
// src/SdFatConfig.h
#include <USBComposite.h>
#include <SPI.h>
#include "SdFat.h"

USBMassStorage MassStorage;
USBCompositeSerial CompositeSerial;

#define LED_PIN PB12
#define PRODUCT_ID 0x29

SdFatEX sd;
const uint32_t speed = SPI_CLOCK_DIV2 ;
const uint8_t SD_CHIP_SELECT = SS;
bool enabled = false;
uint32 cardSize;

bool write(const uint8_t *writebuff, uint32_t startSector, uint16_t numSectors) {
  return sd.card()->writeBlocks(startSector, writebuff, numSectors);
}

bool read(uint8_t *readbuff, uint32_t startSector, uint16_t numSectors) {
  return sd.card()->readBlocks(startSector, readbuff, numSectors);
}

void setup() {
  USBComposite.setProductId(PRODUCT_ID);
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,1);
}

void initReader() {
  digitalWrite(LED_PIN,0);
  cardSize = sd.card()->cardSize();  
  MassStorage.setDriveData(0, cardSize, read, write);
  MassStorage.registerComponent();
  CompositeSerial.registerComponent();
  USBComposite.begin();
  enabled=true;
}

void loop() {
  if (!enabled) {
    if (sd.begin(SD_CHIP_SELECT)) {
      initReader();
    }
    else {
      delay(50);
    }
  }
  else {
    MassStorage.loop();
  }
}

