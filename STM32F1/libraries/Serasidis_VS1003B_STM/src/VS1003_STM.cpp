/*
 Copyright (C) 2012 Andy Karpov <andy.karpov@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 
  Ported to STM32F103 by Vassilis Serasidis on 21 May 2015
  Home:  http://www.serasidis.gr
  email: avrsite@yahoo.gr
  29 May 2015 - Added a fix for booting the VS1053B boards into 
                mp3 decoding instead of booting into MID (modeSwitch function).
  01 July 2015 - Added a Flac decoder patch.
 */

//#include <my_SPI.h>
#include <VS1003_STM.h>

#if defined(USEFLAC)
#include "flac.h"
#endif

#define vs1003_chunk_size  32
/****************************************************************************/

// VS1003 SCI Write Command byte is 0x02
#define VS_WRITE_COMMAND 0x02

// VS1003 SCI Read COmmand byte is 0x03
#define VS_READ_COMMAND  0x03

// SCI Registers

const uint8_t SCI_MODE = 0x0;
const uint8_t SCI_STATUS = 0x1;
const uint8_t SCI_BASS = 0x2;
const uint8_t SCI_CLOCKF = 0x3;
const uint8_t SCI_DECODE_TIME = 0x4;
const uint8_t SCI_AUDATA = 0x5;
const uint8_t SCI_WRAM = 0x6;
const uint8_t SCI_WRAMADDR = 0x7;
const uint8_t SCI_HDAT0 = 0x8;
const uint8_t SCI_HDAT1 = 0x9;
const uint8_t SCI_AIADDR = 0xa;
const uint8_t SCI_VOL = 0xb;
const uint8_t SCI_AICTRL0 = 0xc;
const uint8_t SCI_AICTRL1 = 0xd;
const uint8_t SCI_AICTRL2 = 0xe;
const uint8_t SCI_AICTRL3 = 0xf;
const uint8_t SCI_num_registers = 0xf;

// SCI_MODE bits

const uint8_t SM_DIFF = 0;
const uint8_t SM_LAYER12 = 1;
const uint8_t SM_RESET = 2;
const uint8_t SM_OUTOFWAV = 3;
const uint8_t SM_EARSPEAKER_LO = 4;
const uint8_t SM_TESTS = 5;
const uint8_t SM_STREAM = 6;
const uint8_t SM_EARSPEAKER_HI = 7;
const uint8_t SM_DACT = 8;
const uint8_t SM_SDIORD = 9;
const uint8_t SM_SDISHARE = 10;
const uint8_t SM_SDINEW = 11;
const uint8_t SM_ADPCM = 12;
const uint8_t SM_ADCPM_HP = 13;
const uint8_t SM_LINE_IN = 14;

// Register names

const char reg_name_MODE[]  		= "MODE";
const char reg_name_STATUS[]   		= "STATUS";
const char reg_name_BASS[]   		= "BASS";
const char reg_name_CLOCKF[]   		= "CLOCKF";
const char reg_name_DECODE_TIME[]   = "DECODE_TIME";
const char reg_name_AUDATA[]   		= "AUDATA";
const char reg_name_WRAM[]   		= "WRAM";
const char reg_name_WRAMADDR[]   	= "WRAMADDR";
const char reg_name_HDAT0[]   		= "HDAT0";
const char reg_name_HDAT1[]   		= "HDAT1";
const char reg_name_AIADDR[]   		= "AIADDR";
const char reg_name_VOL[]  			= "VOL";
const char reg_name_AICTRL0[]   	= "AICTRL0";
const char reg_name_AICTRL1[]   	= "AICTRL1";
const char reg_name_AICTRL2[]   	= "AICTRL2";
const char reg_name_AICTRL3[]   	= "AICTRL3";

static PGM_P const register_names[] =
{
  reg_name_MODE,
  reg_name_STATUS,
  reg_name_BASS,
  reg_name_CLOCKF,
  reg_name_DECODE_TIME,
  reg_name_AUDATA,
  reg_name_WRAM,
  reg_name_WRAMADDR,
  reg_name_HDAT0,
  reg_name_HDAT1,
  reg_name_AIADDR,
  reg_name_VOL,
  reg_name_AICTRL0,
  reg_name_AICTRL1,
  reg_name_AICTRL2,
  reg_name_AICTRL3,
};

/****************************************************************************/

inline void DMA1_CH3_Event() {
  dma1_ch3_Active = 0;
  dma_disable(DMA1, DMA_CH3);
}

/****************************************************************************/

uint16_t VS1003::read_register(uint8_t _reg) const
{
  uint16_t result;
  control_mode_on();
  delayMicroseconds(1); // tXCSS
  my_SPI.transfer(VS_READ_COMMAND); // Read operation
  my_SPI.transfer(_reg); // Which register
  result = my_SPI.transfer(0xff) << 8; // read high byte
  result |= my_SPI.transfer(0xff); // read low byte
  delayMicroseconds(1); // tXCSH
  await_data_request();
  control_mode_off();
  return result;
}

/****************************************************************************/

void VS1003::write_register(uint8_t _reg,uint16_t _value) const
{
  control_mode_on();
  delayMicroseconds(1); // tXCSS
  my_SPI.transfer(VS_WRITE_COMMAND); // Write operation
  my_SPI.transfer(_reg); // Which register
  my_SPI.transfer(_value >> 8); // Send hi byte
  my_SPI.transfer(_value & 0xff); // Send lo byte
  delayMicroseconds(1); // tXCSH
  await_data_request();
  control_mode_off();
}

/****************************************************************************/

void VS1003::sdi_send_buffer(const uint8_t* data, size_t len)
{
  data_mode_on();
  while ( len )
  {
    await_data_request();
    delayMicroseconds(3);

    size_t chunk_length = min(len,vs1003_chunk_size);
    len -= chunk_length;
    while ( chunk_length-- )
      my_SPI.transfer(*data++);
  }
  data_mode_off();
}

/****************************************************************************/

void VS1003::sdi_send_zeroes(size_t len)
{
  data_mode_on();
  while ( len )
  {
    await_data_request();

    size_t chunk_length = min(len,vs1003_chunk_size);
    len -= chunk_length;
    while ( chunk_length-- )
      my_SPI.transfer(0);
  }
  data_mode_off();
}

/****************************************************************************/

VS1003::VS1003( uint8_t _cs_pin, uint8_t _dcs_pin, uint8_t _dreq_pin, uint8_t _reset_pin, SPIClass _spiChan):
  cs_pin(_cs_pin), dcs_pin(_dcs_pin), dreq_pin(_dreq_pin), reset_pin(_reset_pin), my_SPI(_spiChan)
{
}

/****************************************************************************/

void VS1003::begin(void)
{

  // Keep the chip in reset until we are ready
  pinMode(reset_pin,OUTPUT);
  digitalWrite(reset_pin,LOW);

  // The SCI and SDI will start deselected
  pinMode(cs_pin,OUTPUT);
  digitalWrite(cs_pin,HIGH);
  pinMode(dcs_pin,OUTPUT);
  digitalWrite(dcs_pin,HIGH);

  // DREQ is an input
  pinMode(dreq_pin,INPUT);

  
  // Boot VS1003
  //printf(("Booting VS1003...\r\n"));

  delay(1);

  my_SPI.begin();
  my_SPI.setBitOrder(MSBFIRST);
  my_SPI.setDataMode(SPI_MODE0);
  // init SPI slow mode
 
  my_SPI.setClockDivider(SPI_CLOCK_DIV64); // Slow!
  
  // release from reset
  digitalWrite(reset_pin,HIGH);
  
  // Declick: Immediately switch analog off
  write_register(SCI_VOL,0xffff); // VOL

  /* Declick: Slow sample rate for slow analog part startup */
  write_register(SCI_AUDATA,10);

  delay(100);

  /* Switch on the analog parts */
  write_register(SCI_VOL,0xfefe); // VOL
  
  //printf(("VS1003 still booting\r\n"));

  write_register(SCI_AUDATA,44101); // 44.1kHz stereo

  write_register(SCI_VOL,0x2020); // VOL
  
  // soft reset
  write_register(SCI_MODE, (1<<SM_SDINEW) | (1<<SM_RESET));
  delay(1);
  await_data_request();
#if defined(USEFLAC)
  loadUserCode(flac_patch,FLAC_PATCHLEN);
#endif
  //write_register(SCI_CLOCKF,0xB800); // Experimenting with higher clock settings
  write_register(SCI_CLOCKF,0x6000);
  delay(1);
  await_data_request();

  // Now you can set high speed SPI clock
  // 72 MHz / 16 = 4.5 MHz max is practically allowed by VS1003 SPI interface.
  my_SPI.setClockDivider(SPI_CLOCK_DIV16); 

  //printf(("VS1003 Set\r\n"));
  //printDetails();
  //printf(("VS1003 OK\r\n"));

}

/****************************************************************************/

void VS1003::setVolume(uint8_t vol) const
{
  uint16_t value = vol;
  value <<= 8;
  value |= vol;

  write_register(SCI_VOL,value); // VOL
}

/****************************************************************************/

void VS1003::startSong(void)
{
  sdi_send_zeroes(10);
}

/****************************************************************************/

void VS1003::playChunk(const uint8_t* data, size_t len)
{
  sdi_send_buffer(data,len);
}

/****************************************************************************/

void VS1003::stopSong(void)
{
  sdi_send_zeroes(2048);
}

/****************************************************************************/

void VS1003::print_byte_register(uint8_t reg) const
{
  const char *name = reinterpret_cast<const char*>(pgm_read_word( register_names + reg ));
  char extra_tab = strlen_P(name) < 5 ? '\t' : 0;
  //printf(("%02x %S\t%c = 0x%02x\r\n"),reg,name,extra_tab,read_register(reg));
}

/****************************************************************************/

void VS1003::printDetails(void) const
{
  //printf(("VS1003 Configuration:\r\n"));
  int i = 0;
  while ( i <= SCI_num_registers )
    print_byte_register(i++);
}

/****************************************************************************/
void VS1003::modeSwitch(void)
{
	//GPIO_DDR
	write_register(SCI_WRAMADDR, 0xc017);
	write_register(SCI_WRAM, 0x0003);
	//GPIO_ODATA
	write_register(SCI_WRAMADDR, 0xc019);
	write_register(SCI_WRAM, 0x0000);
	
	delay(100);
	write_register(SCI_MODE, (1<<SM_SDINEW) | (1<<SM_RESET));
	delay(100);
}
/****************************************************************************/

void VS1003::loadUserCode(const uint16_t* buf, size_t len) const
{
  while (len)
  {
    uint16_t addr = pgm_read_word(buf++); len--;
    uint16_t n = pgm_read_word(buf++); len--;
    if (n & 0x8000U) { /* RLE run, replicate n samples */
      n &= 0x7FFF;
      uint16_t val = pgm_read_word(buf++); len--;
      while (n--) {
	//printf(("W %02x: %04x\r\n"),addr,val);
        write_register(addr, val);
      }
    } else {           /* Copy run, copy n samples */
      while (n--) {
	uint16_t val = pgm_read_word(buf++); len--;
	//printf(("W %02x: %04x\r\n"),addr,val);
        write_register(addr, val);
      }
    }
  }
}

/****************************************************************************/
