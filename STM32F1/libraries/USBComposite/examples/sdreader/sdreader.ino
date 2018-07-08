// This uses the greiman sdfat library.
// To use SdFatEX, set ENABLE_EXTENDED_TRANSFER_CLASS to 1 in the library's
// src/SdFatConfig.h
#include <USBComposite.h>
#include <SPI.h>
#include "SdFat.h"

#define LED_PIN PB12
#define PRODUCT_ID 0x29

SdFatEX sd;
const uint32_t speed = SPI_CLOCK_DIV2 ;
const uint8_t SD_CHIP_SELECT = SS;
bool enabled = false;
uint32 cardSize;

bool write(uint32_t memoryOffset, const uint8_t *writebuff, uint16_t transferLength) {
  return sd.card()->writeBlocks(memoryOffset/512, writebuff, transferLength/512);
}

bool read(uint32_t memoryOffset, uint8_t *readbuff, uint16_t transferLength) {
  return sd.card()->readBlocks(memoryOffset/512, readbuff, transferLength/512);
}

void setup() {
  USBComposite.setProductId(PRODUCT_ID);
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,1);
}

void initReader() {
  digitalWrite(LED_PIN,0);
  cardSize = sd.card()->cardSize();  
  MassStorage.setDrive(0, cardSize*512, read, write);
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

