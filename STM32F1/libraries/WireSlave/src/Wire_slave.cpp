/*
  TwoWire.cpp - TWI/I2C library for Wiring & Arduino
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

extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}

#include "Wire_slave.h"
#include "wirish.h"
#include <libmaple/i2c_slave.h>

#define BUFFER_LENGTH 32

#define MASTER_ADDRESS 0x33
// Constructors ////////////////////////////////////////////////////////////////

TwoWire::TwoWire(i2c_dev* i2cDevice) :
  sel_hard(i2cDevice),
  rxBuffer(nullptr),
  rxBufferAllocated(0),
  rxBufferIndex(0),
  rxBufferLength(0),
  txBuffer(nullptr),
  txBufferAllocated(0),
  txBufferIndex(0),
  txBufferLength(0),
  transmitting(false),
  master(true),
  dev_flags(0),
  itc_msg(),
  itc_slave_msg(),
  user_onRequest(nullptr),
  user_onReceive(nullptr) {
}

// Public Methods //////////////////////////////////////////////////////////////

void TwoWire::begin(void) {
  begin(MASTER_ADDRESS);
}

void TwoWire::begin(uint8_t address){

  rxBufferIndex = 0;
  rxBufferLength = 0;
  allocateRxBuffer(BUFFER_LENGTH);

  txBufferIndex = 0;
  txBufferLength = 0;
  allocateTxBuffer(BUFFER_LENGTH);

  transmitting = false;

  master = (address == MASTER_ADDRESS);

  // Set default speed to 100KHz
  if (master) {
    dev_flags = 0;
    i2c_master_enable(sel_hard, dev_flags);
  } else {
    // TODO: I2C_SLAVE_DUAL_ADDRESS ?
    dev_flags =  I2C_SLAVE_GENERAL_CALL | I2C_SLAVE_USE_RX_BUFFER |
                 I2C_SLAVE_USE_TX_BUFFER;

    itc_slave_msg.addr = address;
    itc_slave_msg.flags = 0;
    itc_slave_msg.data = rxBuffer;
    itc_slave_msg.length = 0;
    itc_slave_msg.flags = 0;

    // TODO why does enable only work before setting IRS and address?
    i2c_slave_enable(sel_hard, dev_flags);

    if (sel_hard==I2C1){
      // attach  receive handler
      i2c_slave_attach_recv_handler(sel_hard, &itc_slave_msg, onReceiveService1);
      // attach transmit handler
      i2c_slave_attach_transmit_handler(sel_hard, &itc_slave_msg, onRequestService1);
    }
    #if WIRE_INTERFACES_COUNT > 1
    else if (sel_hard==I2C2){
      // attach  receive handler
      i2c_slave_attach_recv_handler(sel_hard, &itc_slave_msg, onReceiveService2);
      // attach transmit handler
      i2c_slave_attach_transmit_handler(sel_hard, &itc_slave_msg, onRequestService2);
    }
    #endif

    i2c_slave_set_own_address(sel_hard, address);
  }
}

void TwoWire::begin(int address) {
  begin((uint8_t) address);
}

void TwoWire::end(void) {
  free(txBuffer);
  txBuffer = nullptr;
  txBufferAllocated = 0;
  free(rxBuffer);
  rxBuffer = nullptr;
  rxBufferAllocated = 0;
  i2c_peripheral_disable(sel_hard);
  i2c_master_release_bus(sel_hard); // TODO is this required?
}

void TwoWire::setClock(uint32_t frequencyHz) {
  switch (frequencyHz) {
  case 400000:
    dev_flags |= I2C_FAST_MODE; // set FAST_MODE bit
    break;
  case 100000:
  default:
    dev_flags &= ~I2C_FAST_MODE; // clear FAST_MODE bit
    break;
  }
  if (sel_hard->regs->CR1 & I2C_CR1_PE){
    i2c_disable(sel_hard);
    i2c_master_enable(sel_hard, dev_flags);
  }
}

uint8 TwoWire::process(bool stop) {
  int8 res = i2c_master_xfer(sel_hard, &itc_msg, 1, 0);

  if (res == I2C_ERROR_PROTOCOL) {
    if (sel_hard->error_flags & I2C_SR1_AF) { /* NACK */
      res = (sel_hard->error_flags & I2C_SR1_ADDR ? ENACKADDR : ENACKTRNS);
    } else if (sel_hard->error_flags & I2C_SR1_OVR) { /* Over/Underrun */
      res = EDATA;
    } else { /* Bus or Arbitration error */
      res = EOTHER;
    }
    i2c_disable(sel_hard);
    i2c_master_enable(sel_hard, (I2C_BUS_RESET | dev_flags));
  }

  return res;
}

//TODO: Add the ability to queue messages (adding a boolean to end of function
// call, allows for the Arduino style to stay while also giving the flexibility
// to bulk send
uint8 TwoWire::requestFrom(uint8_t address, uint8_t num_bytes,
    uint32_t iaddress, uint8_t isize, uint8_t sendStop) {

  ASSERT(master);

  allocateRxBuffer(num_bytes);
  // error if no memory block available to allocate the buffer
  if (rxBuffer == nullptr) {
    return EDATA;
  }

  // reset tx buffer iterator vars
  rxBufferIndex = 0;
  rxBufferLength = 0;

  itc_msg.addr = address;
  itc_msg.flags = I2C_MSG_READ;
  itc_msg.length = num_bytes;
  itc_msg.data = &rxBuffer[rxBufferIndex];
  process(sendStop); // TODO deal with to the return value

  // TODO handle iaddress & isize
  rxBufferLength += itc_msg.xferred;
  itc_msg.flags = 0;

  return rxBufferLength;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity,
    uint8_t sendStop) {
  // TODO shouldn't this set flag |= I2C_MSG_10BIT_ADDR ???
  return requestFrom((uint8_t) address, (uint8_t) quantity, (uint32_t) 0,
      (uint8_t) 0, sendStop);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity) {
  return requestFrom((uint8_t) address, (uint8_t) quantity, (uint32_t) 0,
      (uint8_t) 0, (uint8_t) true);
}

uint8_t TwoWire::requestFrom(int address, int quantity) {
  return requestFrom((uint8_t) address, (uint8_t) quantity, (uint32_t) 0,
      (uint8_t) 0, (uint8_t) true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop) {
  return requestFrom((uint8_t) address, (uint8_t) quantity, (uint32_t) 0,
      (uint8_t) 0, (bool) sendStop);
}

void TwoWire::beginTransmission(uint8_t address) {

  // indicate that we are transmitting
  transmitting = true;

  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;

  itc_msg.addr = address;
  itc_msg.data = &txBuffer[txBufferIndex];
  itc_msg.length = 0;
  itc_msg.flags = 0;

}

void TwoWire::beginTransmission(int address) {
  beginTransmission((uint8_t) address);
}

//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(false) allows a sketch to
//	perform a repeated start.
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
uint8_t TwoWire::endTransmission(uint8_t sendStop) {
  //UNUSED(sendStop);
  int8_t ret = 4;

  if (master == true) {

    itc_msg.data = txBuffer;
    itc_msg.length = txBufferLength;
    itc_msg.flags = 0;
    ret = process(sendStop); // Changed so that the return value from process is returned by this function see also the return line below
    txBufferIndex = 0;

    // reset Tx buffer
    resetTxBuffer(); // TODO why? isn't this just unesssesary?

    // reset tx buffer iterator vars
    txBufferIndex = 0;
    txBufferLength = 0;

    // indicate that we are done transmitting
    transmitting = false;
  }

  return ret;
}

//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//

uint8_t TwoWire::endTransmission(void) {
  return endTransmission(true);
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(uint8_t data) {
  if (!transmitting && master) {
    return 0;
  } else {
    // in master transmitter mode or slave tx event callback
    allocateTxBuffer(txBufferLength + 1);
    // error if no memory block available to allocate the buffer
    if (txBuffer == nullptr) {
      setWriteError();
      return 0;
    }
    // put byte in tx buffer
    txBuffer[txBufferIndex] = data;
    ++txBufferIndex;
    // update amount in buffer
    txBufferLength = txBufferIndex;
  }
  return 1;
}

/**
 * @brief  This function must be called in slave Tx event callback or after
 *         beginTransmission() and before endTransmission().
 * @param  pdata: pointer to the buffer data
 * @param  quantity: number of bytes to write
 * @retval number of bytes ready to write.
 */
size_t TwoWire::write(const uint8_t *data, size_t quantity) {
  if (!transmitting && master) {
    return 0;
  } else {
    // in master transmitter mode or slave Tx event callback
    allocateTxBuffer(txBufferLength + quantity);
    // error if no memory block available to allocate the buffer
    if (txBuffer == nullptr) {
      setWriteError();
      return 0;
    }
    // put bytes in tx buffer
    memcpy(&(txBuffer[txBufferIndex]), data, quantity);
    txBufferIndex = txBufferIndex + quantity;
    // update amount in buffer
    txBufferLength = txBufferIndex;
    return quantity;
  }
  return 0;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::available(void) {
  return rxBufferLength - rxBufferIndex;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::read(void) {
  int value = -1;

  // get each successive byte on each call
  if (rxBufferIndex < rxBufferLength) {
    value = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;

    /* Commented as not I think it is not useful
     * but kept to show that it is possible to
     * reset rx buffer when no more data available */
    /*if(rxBufferIndex == rxBufferLength) {
     resetRxBuffer();
     }*/
  }

  return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::peek(void) {
  int value = -1;

  if (rxBufferIndex < rxBufferLength) {
    value = rxBuffer[rxBufferIndex];
  }

  return value;
}

void TwoWire::flush(void) {
  rxBufferIndex = 0;
  rxBufferLength = 0;
  resetRxBuffer();
  txBufferIndex = 0;
  txBufferLength = 0;
  resetTxBuffer();
}

// behind the scenes function that is called when data is received
void __attribute__((always_inline)) TwoWire::onReceiveService(i2c_msg* msg) {
  // don't bother if user hasn't registered a callback
  if (!user_onReceive) {
    return;
  }

  // don't bother if rx buffer is in use by a master requestFrom() op
  // i know this drops data, but it allows for slight stupidity
  // meaning, they may not have read all the master requestFrom() data yet
  if (rxBufferIndex < rxBufferLength) {
    return;
  }
  // copy twi rx buffer into local read buffer
  // this enables new reads to happen in parallel
  //
  // TODO: Something is strange here, isn't msg->data==rxBuffer? nope, itsnot
  //
  memcpy(rxBuffer, msg->data, msg->length);
  // set rx iterator vars
  rxBufferIndex = 0;
  rxBufferLength = msg->length;
  // alert user program
  user_onReceive(msg->length);
}

// behind the scenes function that is called when data is requested
void __attribute__((always_inline)) TwoWire::onRequestService(i2c_msg* msg) {
  // don't bother if user hasn't registered a callback
  if (!user_onRequest) {
    return;
  }

  // reset tx buffer iterator vars
  // !!! this will kill any pending pre-master sendTo() activity
  txBufferIndex = 0;
  txBufferLength = 0;
  // alert user program
  user_onRequest();

  // update i2c_msg
  msg->data = txBuffer;
  msg->length = txBufferLength;
  msg->xferred = 0;
}

// sets function called on slave write
void TwoWire::onReceive(void (*function)(int)) {
  user_onReceive = function;
}

// sets function called on slave read
void TwoWire::onRequest(void (*function)(void)) {
  user_onRequest = function;
}

/**
 * @brief  Allocate the Rx/Tx buffer to the requested length if needed
 * @note   Minimum allocated size is BUFFER_LENGTH)
 * @param  length: number of bytes to allocate
 */
inline void TwoWire::allocateRxBuffer(size_t length) {
  // By default we allocate BUFFER_LENGTH bytes. It is the min size of the buffer.
  if (length < BUFFER_LENGTH) {
    length = BUFFER_LENGTH;
  }
  if (rxBufferAllocated < length) {
    rxBuffer = (uint8_t *) realloc(rxBuffer, length * sizeof(uint8_t));
    rxBufferAllocated = (rxBuffer != nullptr) ? length : 0;
  }
}

inline void TwoWire::allocateTxBuffer(size_t length) {
  // By default we allocate BUFFER_LENGTH bytes. It is the min size of the buffer.
  if (length < BUFFER_LENGTH) {
    length = BUFFER_LENGTH;
  }
  if (txBufferAllocated < length) {
    txBuffer = (uint8_t *) realloc(txBuffer, length * sizeof(uint8_t));
    txBufferAllocated = (txBuffer != nullptr) ? length : 0;
  }
}

/**
 * @brief  Reset Rx/Tx buffer content to 0
 */
inline void TwoWire::resetRxBuffer(void) {
  if (rxBuffer != nullptr)
    memset(rxBuffer, 0, rxBufferAllocated);
}

inline void TwoWire::resetTxBuffer(void) {
  if (txBuffer != nullptr)
    memset(txBuffer, 0, txBufferAllocated);
}


// Preinstantiate Objects //////////////////////////////////////////////////////
TwoWire& Wire = TwoWire::getInstance(); //SCL:D14 SDA:D15
#if WIRE_INTERFACES_COUNT > 1
TwoWire& Wire1 = TwoWire::getInstance1(); //SCL: D1 SDA: D0
#endif

// Static methods //////////////////////////////////////////////////////////////

TwoWire& TwoWire::getInstance(){
  static TwoWire* instance = nullptr;
  if (!instance) {
    instance = new TwoWire(I2C1);
  }
  return *instance;
}

#if WIRE_INTERFACES_COUNT > 1
TwoWire& TwoWire::getInstance1(){
  static TwoWire* instance = nullptr;
  if (!instance) {
    instance = new TwoWire(I2C2);
  }
  return *instance;
}
#endif

// onRequestServiceX and onReceiveServiceX can't be inline since they
// are exclusively called via a function pointer
void TwoWire::onRequestService1(i2c_msg* msg) {
  Wire.onRequestService(msg);
}

void TwoWire::onReceiveService1(i2c_msg* msg) {
  Wire.onReceiveService(msg);
}

#if WIRE_INTERFACES_COUNT > 1
void TwoWire::onRequestService2(i2c_msg* msg) {
  Wire1.onRequestService(msg);
}
void TwoWire::onReceiveService2(i2c_msg* msg) {
  Wire1.onReceiveService(msg);
}
#endif
