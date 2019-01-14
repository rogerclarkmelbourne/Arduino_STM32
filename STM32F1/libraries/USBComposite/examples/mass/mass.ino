#include <USBComposite.h>

USBMassStorage MassStorage;
USBCompositeSerial CompositeSerial;

#define PRODUCT_ID 0x29

#include "image.h"

bool write(const uint8_t *writebuff, uint32_t memoryOffset, uint16_t transferLength) {
  memcpy(image+SCSI_BLOCK_SIZE*memoryOffset, writebuff, SCSI_BLOCK_SIZE*transferLength);

  return true;
}

bool read(uint8_t *readbuff, uint32_t memoryOffset, uint16_t transferLength) {
  memcpy(readbuff, image+SCSI_BLOCK_SIZE*memoryOffset, SCSI_BLOCK_SIZE*transferLength);
  
  return true;
}

char hexNibble(uint8 x) {
  return x < 10 ? x + '0' : x + 'A' - 10;
}

char* format16(uint16 c) {
  static char str[6];
  str[5] = 0;
  char *p = str+5;
  do {
    *--p = (c % 10) + '0';
    c /= 10;
  } while(c);
  return p;
}

void dumpDrive() {
  char hex[7] = "0x11,";
  CompositeSerial.print("uint8 image[");
  CompositeSerial.print(format16(sizeof(image)));
  CompositeSerial.println("] = {");
  int last;
  for (last=sizeof(image)-1;last>=0 && image[last] == 0;last--);
  if (last<0) last=0;
  
  for (int i=0; i<=last; i++) {
    if (i && i % 16 == 0)
      CompositeSerial.println("");
    hex[2] = hexNibble(image[i]>>4);
    hex[3] = hexNibble(image[i]&0xF);
    CompositeSerial.print(hex);
  }
  CompositeSerial.println("\n};\n");
}

void setup() {
  USBComposite.setProductId(PRODUCT_ID);
  MassStorage.setDriveData(0, sizeof(image)/SCSI_BLOCK_SIZE, read, write);
  MassStorage.registerComponent();
  CompositeSerial.registerComponent();
  USBComposite.begin();
  delay(2000);
}

void loop() {
  MassStorage.loop();
  if (CompositeSerial.available() && 'd' == CompositeSerial.read()) {
      dumpDrive();
  }
}

