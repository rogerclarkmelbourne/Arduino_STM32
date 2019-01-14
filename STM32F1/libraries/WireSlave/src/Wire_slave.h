/*
 TwoWire.h - TWI/I2C library for Arduino & Wiring
 Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
 */

#ifndef TwoWire_h
#define TwoWire_h

#include <inttypes.h>
#include "Stream.h"
#include "Arduino.h"
#include <libmaple/i2c_slave.h>


// WIRE_HAS_END means Wire has end()
#ifndef WIRE_HAS_END
#define WIRE_HAS_END 1
#endif

#ifndef WIRE_INTERFACES_COUNT
#define WIRE_INTERFACES_COUNT 2
#endif

/* return codes from endTransmission() */
typedef enum EndTranmissionCodes {
  SUCCESS = 0, /* transmission was successful */
  EDATA = 1, /* too much data */
  ENACKADDR = 2, /* received nack on transmit of address */
  ENACKTRNS = 3, /* received nack on transmit of data */
  EOTHER = 4, /* other error */
} EndTranmissionCodes;

class TwoWire: public Stream {
private:
  i2c_dev* sel_hard;

  uint8_t *rxBuffer;  // lazy allocation
  uint8_t rxBufferAllocated;
  uint8_t rxBufferIndex;
  uint8_t rxBufferLength;

  uint8_t *txBuffer; // lazy allocation
  uint8_t txBufferAllocated;
  uint8_t txBufferIndex;
  uint8_t txBufferLength;

  bool transmitting;

  bool master;

  uint8 dev_flags;
  i2c_msg itc_msg;
  i2c_msg itc_slave_msg;

  void (*user_onRequest)(void);
  void (*user_onReceive)(int quantity);

  void allocateRxBuffer(size_t length);
  void allocateTxBuffer(size_t length);

  void resetRxBuffer(void);
  void resetTxBuffer(void);
  uint8 process(bool stop = true); // wrapper for i2c_master_xfer

  inline void __attribute__((always_inline)) onReceiveService(i2c_msg* msg);
  inline void __attribute__((always_inline)) onRequestService(i2c_msg* msg);

  static void onRequestService1(i2c_msg*);
  static void onReceiveService1(i2c_msg*);
  #if WIRE_INTERFACES_COUNT > 1
  static void onRequestService2(i2c_msg*);
  static void onReceiveService2(i2c_msg*);
  #endif

  TwoWire(i2c_dev* i2cDevice);
  TwoWire() = delete;
  TwoWire(const TwoWire&) = delete;
  TwoWire& operator=(const TwoWire&) = delete;
  TwoWire(TwoWire&&) = delete;
  TwoWire& operator=(TwoWire&&) = delete;

public:
  // return the 'Wire' instance (using I2C1)
  static TwoWire& getInstance();
  #if WIRE_INTERFACES_COUNT > 1
  // return the 'Wire1' instance (using I2C2)
  static TwoWire& getInstance1();
  #endif

  void begin(); // master mode
  void begin(uint8_t myAddress); //slave mode
  void begin(int myAddress); //slave mode
  void end();
  void setClock(uint32_t frequencyHz);
  void beginTransmission(uint8_t slaveAddress);
  void beginTransmission(int slaveAddress);
  uint8_t endTransmission(void);
  uint8_t endTransmission(uint8_t sendStop);

  uint8_t requestFrom(uint8_t slaveAddress, uint8_t quantity);
  uint8_t requestFrom(uint8_t slaveAddress, uint8_t quantity, uint8_t sendStop);
  uint8_t requestFrom(uint8_t slaveAddress, uint8_t num_bytes, uint32_t iaddress, uint8_t isize, uint8_t sendStop);
  uint8_t requestFrom(int slaveAddress, int quantity);
  uint8_t requestFrom(int slaveAddress, int quantity, int sendStop);

  virtual size_t write(uint8_t data);
  virtual size_t write(const uint8_t *data, size_t quantity);
  virtual int available(void);
  virtual int read(void);
  virtual int peek(void);
  virtual void flush(void);
  void onReceive(void (*)(int quantity));
  void onRequest(void (*)(void));

  inline size_t write(unsigned long data) {
    return write((uint8_t) data);
  }
  inline size_t write(long data) {
    return write((uint8_t) data);
  }
  inline size_t write(unsigned int data) {
    return write((uint8_t) data);
  }
  inline size_t write(int data) {
    return write((uint8_t) data);
  }
  using Print::write;
};

extern TwoWire& Wire;
#if WIRE_INTERFACES_COUNT > 1
extern TwoWire& Wire1;
#endif

#endif // TwoWire_h
