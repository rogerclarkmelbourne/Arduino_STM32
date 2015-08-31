/*
 Copyright (C) 2012 Andy Karpov <andy.karpov@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 
  Ported to STM32F103 by Vassilis Serasidis on 21 May 2015
  Home:  http://www.serasidis.gr
  email: avrsite@yahoo.gr
  
  29 May 2015 - Added a fix for booting the VS1053B boards into mp3 decoding instead of booting into MIDI.
  01 July 2015 - Added a Flac decoder patch.
 */

//This is an additional test line
#ifndef __VS1003_STM_H__
#define __VS1003_STM_H__

// STL headers
// C headers
// Framework headers

//#define USEFLAC

#include <Arduino.h>
#include <SPI.h>
/**
 * Driver for VS1003 - MP3 / WMA / MIDI Audio Codec Chip
 *
 * See http://www.vlsi.fi/en/products/vs1003.html
 */

class VS1003
{
private:
  uint8_t cs_pin; /**< Pin where CS line is connected */
  uint8_t dcs_pin; /**< Pin where DCS line is connected */
  uint8_t dreq_pin; /**< Pin where DREQ line is connected */
  uint8_t reset_pin; /**< Pin where RESET line is connected */
  uint8_t my_SPCR; /**< Value of the SPCR register how we like it. */
  uint8_t my_SPSR; /**< Value of the SPSR register how we like it. */
  SPIClass my_SPI;

  inline void await_data_request(void) const
  {
    while ( !digitalRead(dreq_pin) );
  }

  inline void control_mode_on(void) const
  {
    digitalWrite(dcs_pin,HIGH);
    digitalWrite(cs_pin,LOW);
  }

  inline void control_mode_off(void) const
  {
    digitalWrite(cs_pin,HIGH);
  }

  inline void data_mode_on(void) const
  {
    digitalWrite(cs_pin,HIGH);
    digitalWrite(dcs_pin,LOW);
  }

  inline void data_mode_off(void) const
  {
    digitalWrite(dcs_pin,HIGH);
  }

  uint16_t read_register(uint8_t _reg) const;
  void write_register(uint8_t _reg,uint16_t _value) const;
  void sdi_send_buffer(const uint8_t* data,size_t len);
  void sdi_send_zeroes(size_t length);
  void print_byte_register(uint8_t reg) const;
  
  /**
   * Load a user code plugin
   *
   * @param buf Location of memory (in PROGMEM) where the code is
   * @param len Number of words to load
   */
  void loadUserCode(const uint16_t* buf, size_t len) const;

public:

  /**
   * Constructor
   *
   * Only sets pin values.  Doesn't do touch the chip.  Be sure to call begin()!
   */
  VS1003( uint8_t _cs_pin, uint8_t _dcs_pin, uint8_t _dreq_pin, uint8_t _reset_pin, SPIClass _spi = SPIClass(1));

  /**
   * Begin operation
   *
   * Sets pins correctly, and prepares SPI bus.
   */
  void begin(void);

  /**
   * Prepare to start playing
   *
   * Call this each time a new song starts.
   */
  void startSong(void);

  /**
   * Play a chunk of data.  Copies the data to the chip.  Blocks until complete.
   *
   * @param data Pointer to where the data lives
   * @param len How many bytes of data to play
   */
  void playChunk(const uint8_t* data, size_t len);
  
  /**
   * Finish playing a song.
   *
   * Call this after the last playChunk call.
   */
  void stopSong(void);

  /**
   * Print configuration details
   *
   * Dumps all registers to stdout.  Be sure to have stdout configured first
   * (see fdevopen() in avr/io.h).
   */
  void printDetails(void) const;
  
  /**
  *
  *
  */
  void modeSwitch(void);

  /**
   * Set the player volume
   *
   * @param vol Volume level from 0-255, lower is louder.
   */
  void setVolume(uint8_t vol) const;

};

#endif

