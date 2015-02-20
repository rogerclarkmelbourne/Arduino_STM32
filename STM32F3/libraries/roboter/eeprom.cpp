#include <wirish/wirish.h>
#include <libmaple/i2c.h>
#include <string.h>
#include "eeprom.h"
#include "debug.h"


/*
     Source Code fuer 24C128 EEPROM (16Kbyte)
*
            __ __
      A0  -|     |- VCC
      A1  -|     |- WP --> unconnected low: write enabled
      A2  -|     |- SCL
      VSS -|_____|- SDA
          
	 
*/

#define EEPROM_I2C_BASE_ADDR (0xA0>>1)
#define EEPROM_I2C_ADDR(mem_addr) EEPROM_I2C_BASE_ADDR
#define EEPROM_I2C_CHANNEL I2C1

#define EEPROM_MAX_BUF 3

static uint8    msg_wr_data[EEPROM_MAX_BUF];
static i2c_msg  msg[2];
static uint8    msg_rd_data[EEPROM_MAX_BUF];


static void writeDataByte(int device_address, byte memory_address, byte data) 
// write one byte of data 'data' to eeprom memory address 'memory_address' to chip with I2C address 'device_address'
{
/* Arduino Code
  Wire.beginTransmission(device_address);  // device address
  Wire.write(memory_address );             // memory address
  Wire.write(data);                        // data to send
  Wire.endTransmission();                  // end
  delay(10);
*/
   msg[0].addr = device_address;
   msg[0].flags = 0; // write, 7 bit address
   msg[0].xferred = 0;
   msg[0].length = 3;
   msg[0].data = msg_wr_data;

   msg_wr_data[0] = (memory_address>>8)&0xFF;
   msg_wr_data[1] = memory_address&0xFF;
   msg_wr_data[2] = data;

   i2c_master_xfer(EEPROM_I2C_CHANNEL, msg, 1, 10);
   
   delay(10);
}


static byte readDataByte(int device_address, byte memory_address) 
// reads one byte of data from memory location 'memory_address' in chip at I2C address 'device_address' 
{

/* Arduino Code
  byte result;  // return value
  Wire.beginTransmission(device_address); // device address
  Wire.write(memory_address);             // memory address
  Wire.endTransmission();                 // end
  Wire.requestFrom(device_address,1);     // get one byte of data from device
  if (Wire.available()) 
    result = Wire.read();
  return result;                          // return the read data byte
  delay(10);
*/
   msg[0].addr = device_address;
   msg[0].flags = 0; // write, 7 bit address
   msg[0].xferred = 0;
   msg[0].length = 2;
   msg[0].data = msg_wr_data;

   msg_wr_data[0] = (memory_address>>8)&0xFF;
   msg_wr_data[1] = memory_address&0xFF;

   msg[1].addr = device_address;
   msg[1].flags = I2C_MSG_READ;
   msg[1].xferred = 0;
   msg[1].length = 1;
   msg[1].data = msg_rd_data;

   i2c_master_xfer(EEPROM_I2C_CHANNEL, msg, 2, 10);
   return msg_rd_data[0];
}

//---------------------------------------------------------------------------------
int  eepromWriteBytes(int addr, int len, unsigned char *pData)
{
    for (int i = 0; i<len; i++)
	    writeDataByte( EEPROM_I2C_ADDR(addr+i), (byte)((addr+i)%256), pData[i]);

    return len; // FIXME: check successful write operation		
}

//---------------------------------------------------------------------------------
int  eepromReadBytes(int addr, int len, unsigned char *pData)
{
    for (int i = 0; i<len; i++)
	    pData[i] = readDataByte( EEPROM_I2C_ADDR(addr+i), (byte)((addr+i)%256));

    return len; // FIXME: check successful read operation		
}


//---------------------------------------------------------------------------------
void          eepromWriteByte(int addr, unsigned char b)
{
   writeDataByte( EEPROM_I2C_ADDR(addr), (byte)(addr%256), b);
}


//---------------------------------------------------------------------------------
unsigned char eepromReadByte (int addr)
{
	return  readDataByte( EEPROM_I2C_ADDR(addr), (byte)(addr%256));
}

//---------------------------------------------------------------------------------
void          eepromWriteWord16(int addr, unsigned int w)
{
   eepromWriteBytes(addr, 2, (byte *)&w);

}

//---------------------------------------------------------------------------------
unsigned int  eepromReadWord16 (int addr)
{
   unsigned int w;
   eepromReadBytes(addr, 2, (byte *)&w);
   return w;
}



//#define TEST_EEPROM
#ifdef TEST_EEPROM
void TestEeprom()
{
   int i;
   i2c_master_enable(I2C1, 0);

   char Buf[6];

   memcpy(Buf, "Hallo",6);
   char rdbuf[6];
   /*for (i=0; i<20; i++)
      eepromWriteByte(i,i);

   eepromWriteBytes(20,6,(unsigned char*)Buf);
   */
   eepromReadBytes(20,6,(unsigned char*)rdbuf);
   Serial2.println(rdbuf);

   for (i=0; i<20; i++)
      Serial2.println(eepromReadByte(i));
      //  Serial2.println(i);
   while(1);

}
#endif




