/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 * @file SoftWire.h
 * @author Trystan Jones <crenn6977@gmail.com>
 * @brief Wire library, uses the WireBase to create the primary interface
 *        while keeping low level interactions invisible to the user.
 */

/*
 * Library updated by crenn to follow new Wire system.
 * Code was derived from the original Wire for maple code by leaflabs and the
 * modifications by gke and ala42.
 */

#ifndef _SOFTWIRE_H_
#define _SOFTWIRE_H_

#include "utility/WireBase.h"
#include "wirish.h"

/*
 * On the Maple, let the default pins be in the same location as the Arduino
 * pins
 */
#define SDA PB7
#define SCL PB6

#define SOFT_STANDARD 19
#define SOFT_FAST 0


//#define I2C_DELAY(x) {uint32 time=micros(); while(time>(micros()+x));}
#define I2C_DELAY(x) do{for(int i=0;i<x;i++) {asm volatile("nop");}}while(0)




class SoftWire : public WireBase {
 public:
    uint8 		i2c_delay;
    uint8       scl_pin;
    uint8       sda_pin;

    /*
     * Sets the SCL line to HIGH/LOW and allow for clock stretching by slave
     * devices
     */
    void set_scl(bool);

    /*
     * Sets the SDA line to HIGH/LOW
     */
    void set_sda(bool);

    /*
     * Creates a Start condition on the bus
     */
    void i2c_start();

    /*
     * Creates a Stop condition on the bus
     */
    void  i2c_stop();
    
    /*
     * Created a Repeated Start condition on the bus
     */
    void i2c_repeated_start();
    
    /*
     * Gets an ACK condition from a slave device on the bus
     */
    bool i2c_get_ack();

    /*
     * Creates a ACK condition on the bus
     */
    void i2c_send_ack();

    /*
     * Creates a NACK condition on the bus
     */
    void i2c_send_nack();

    /*
     * Shifts in the data through SDA and clocks SCL for the slave device
     */
    uint8 i2c_shift_in();

    /*
     * Shifts out the data through SDA and clocks SCL for the slave device
     */
    void i2c_shift_out(uint8);
	
	

	
 protected:
    /*
     * Processes the incoming I2C message defined by WireBase
     */
    uint8 process(uint8);
    uint8 process();
 private:
	gpio_dev 	*sdaDevice;
	uint8 		sdaBit;
	gpio_dev 	*sclDevice;
	uint8 		sclBit;	
 public:
    /*
     * Accept pin numbers for SCL and SDA lines. Set the delay needed
     * to create the timing for I2C's Standard Mode and Fast Mode.
     */
    SoftWire(uint8 scl=SCL, uint8 sda=SDA, uint8 delay=SOFT_STANDARD);

    /*
     * Sets pins SDA and SCL to OUPTUT_OPEN_DRAIN, joining I2C bus as
     * master. This function overwrites the default behaviour of
     * .begin(uint8) in WireBase
     */
    void begin(uint8 = 0x00);
	
	void setClock(uint32_t frequencyHz);

    /*
     * Sets pins SDA and SCL to INPUT
     */
	void end();	
	
    /*
     * If object is destroyed, set pin numbers to 0.
     */
    ~SoftWire();
};

//extern SoftWire Wire;

#endif // _SOFTWIRE_H_
