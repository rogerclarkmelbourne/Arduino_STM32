STM32 adaption by Matthias Diro, 25.03.2015
Things to know:
This adaption uses hardware I2C (hardwire.h), Port: I2c2. SDA=0, SCL=1 on maple mini
To change it to Port I2C1: 
//HardWire HWIRE(1,I2C_FAST_MODE); // I2c1
HardWire HWIRE(2,I2C_FAST_MODE); // I2c2