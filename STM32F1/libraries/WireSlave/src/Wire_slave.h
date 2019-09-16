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

 Modified 2019 by Donna Whisnant to merge WireSlave changes with the core to
 make slave mode work and without having conflicting data type defintions and
 to improve the WireSlave functionality, completing some of the missing pieces.

 */

#ifndef TwoWire_h
#define TwoWire_h

#include <inttypes.h>
#include <Stream.h>
#include <Arduino.h>
#include <libmaple/i2c.h>


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

class TwoWire: public Stream
{
private:
	i2c_dev* sel_hard;

	uint8_t *rxBuffer;		// lazy allocation
	size_t rxBufferAllocated;
	size_t rxBufferIndex;
	size_t rxBufferLength;

	uint8_t *txBuffer;		// lazy allocation
	size_t txBufferAllocated;
	size_t txBufferIndex;
	size_t txBufferLength;

	bool transmitting;

	uint32_t dev_flags;
	i2c_msg itc_msg;			// Master Tx/Rx Message and Slave Tx Message
	i2c_msg itc_slave_msg;		// Slave Rx Message (since it's completely asynchronous)

	// TODO : Add additional slave functions that relay the address
	//	being accessed:
	void (*user_onRequest)(void);
	void (*user_onReceive)(int quantity);

protected:
	bool isMaster() const { return ((dev_flags & I2C_SLAVE_MODE) == 0); }
	void allocateRxBuffer(size_t length);
	void allocateTxBuffer(size_t length);

	uint8 process(bool stop = true);	// wrapper for i2c_master_xfer

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

	static TwoWire g_Wire;
	#if WIRE_INTERFACES_COUNT > 1
	static TwoWire g_Wire1;
	#endif

public:
	// return the 'Wire' instance (using I2C1)
	static inline TwoWire& getInstance()
	{
		return g_Wire;
	}

#if WIRE_INTERFACES_COUNT > 1
	// return the 'Wire1' instance (using I2C2)
	static inline TwoWire& getInstance1()
	{
		return g_Wire1;
	}
#endif

	void begin();	// master mode
	void begin(uint16_t myAddress);	//slave mode
	void begin(uint16_t myAddress1, uint16_t myAddress2);		// Dual-Address slave mode
	void end();
	void setClock(uint32_t frequencyHz);
	void beginTransmission(uint16_t slaveAddress);
	uint8_t endTransmission(bool sendStop = true);

	uint8_t requestFrom(uint16_t slaveAddress, uint8_t quantity);
	uint8_t requestFrom(uint16_t slaveAddress, uint8_t quantity, bool sendStop);
	uint8_t requestFrom(uint16_t slaveAddress, uint8_t num_bytes, uint32_t iaddress, uint8_t isize, bool sendStop);

	virtual size_t write(uint8_t data) override;
	virtual size_t write(const void *data, uint32_t quantity) override;	// Note: this is the signature of the base Print::write function

	virtual int available(void) override;
	virtual int read(void) override;
	virtual int peek(void) override;
	virtual void flush(void) override;
	void onReceive(void (*)(int quantity));
	void onRequest(void (*)(void));

	using Print::write;

	// Minimum I2C Buffer Length:
	static constexpr size_t BUFFER_LENGTH  = 36;		// Use 36 instead of 32 for minimum size so we can transfer 32 bytes of data in addition to addresses for EEPROM, etc.

private:
	uint8_t slaveRxBuffer[BUFFER_LENGTH];			// Fixed length slave IRQ receive buffer
};

extern TwoWire& Wire;
#if WIRE_INTERFACES_COUNT > 1
extern TwoWire& Wire1;
#endif

#endif // TwoWire_h
