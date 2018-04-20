// SPI slave example
// STM32 acts as a SPI slave an reads 8 bit data frames over SPI
// The data sent to the master is a simple count (0, 1, 2, 3) that is incremented
// each time a data frame is received.
// Serial output is here for debug

#include <SPI.h>
#include <cstdint>

void setupSPI(void)
{
  pinMode(PA7, INPUT); // MOSI1
  pinMode(PA6, INPUT); // MISO1
  pinMode(PA5, INPUT); // SCK
  pinMode(BOARD_SPI_DEFAULT_SS, INPUT); // SS

  // Select SPI1
  SPI.setModule(1);
  // The clock value is not used
  SPI.beginTransactionSlave(SPISettings(18000000, MSBFIRST, SPI_MODE0, DATA_SIZE_8BIT));
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  // Data that master will receive when transferring a data frame over SPI
  SPI.dev()->regs->DR = 0;
  setupSPI();
}

uint8_t count(0);
void loop()
{
  // Blocking call to read SPI message
  uint8_t msg(SPI.read());
  Serial.print("Received = 0b");
  Serial.print(msg, BIN);
  Serial.print(", 0x");
  Serial.print(msg, HEX);
  Serial.print(", ");
  Serial.println(msg);
  // Next data frame that will be received by master
  SPI.dev()->regs->DR = ++count;
}
