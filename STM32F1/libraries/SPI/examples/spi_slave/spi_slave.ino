// SPI full-duplex slave example
// STM32 acts as a SPI slave and reads 8 bit data frames over SPI.
// Master also gets a reply from the slave, which is a a simple count (0, 1, 2, 3)
// that is incremented each time a data frame is received.
// Serial output is here for debug

#include <SPI.h>
#include <cstdint>

void setupSPI(void)
{
  // The clock value is not used
  // SPI1 is selected by default
  // MOSI, MISO, SCK and NSS PINs are set by the library
  SPI.beginTransactionSlave(SPISettings(18000000, MSBFIRST, SPI_MODE0, DATA_SIZE_8BIT));
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  setupSPI();
}

uint8_t count = 0;
void loop()
{
  // Blocking call to read SPI message
  uint8_t msg = SPI.transfer(++count);
  Serial.print("Received = 0b");
  Serial.print(msg, BIN);
  Serial.print(", 0x");
  Serial.print(msg, HEX);
  Serial.print(", ");
  Serial.println(msg);
}

