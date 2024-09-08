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
 * @file Wire.h
 * @author Trystan Jones <crenn6977@gmail.com>
 * @brief Wire library, uses the hardware I2C available in the Maple to
 *        interact with I2C slave devices.
 */

/*
 * Library created by crenn to use the new WireBase system and allow Arduino
 * users easy interaction with the I2C Hardware in a familiar method.
 */

#ifndef _TWOWIRE_H_
#define _TWOWIRE_H_

#include "utility/WireBase.h"
#include "wirish.h"
#include <libmaple/i2c.h>

class TwoWire : public WireBase {
private:
    i2c_dev* sel_hard;
    uint8    dev_flags;
	uint32	frequency; //new variable to store i2c frequency
protected:
    /*
     * Processes the incoming I2C message defined by WireBase to the
     * hardware. If an error occured, restart the I2C device.
     */
    uint8 process(uint8 stop);
    uint8 process();
public:
    /*
     * Check if devsel is within range and enable selected I2C interface with
     * passed flags and frequency.
	 *(frequency tested go up to 1.2Mhz @F_CPU 72Mhz with capable slaves)
     */
    TwoWire(uint8 dev_sel, uint8 flags = 0, uint32 freq = 100000);
	
	/*
	 * Shuts down (disables) the hardware I2C
	 */
	void end();

	/*
	* Sets the hardware I2C clock
	*/
	void setClock(uint32_t frequencyHz);
	
	/*
	* Useful to remap I2C1 to alternate pins PB8, PB9
	* after construction by adding flag I2C_REMAP
	*/
	void setDevFlags(uint8 dev_flags) {
		dev_flags = df;
	}
	
	uint8 getDevFlags(void) {
		return dev_flags;
	}
	
    /*
     * Disables the I2C device and remove the device address.
     */
    ~TwoWire();

    void begin(uint8 self_addr = 0x00);
};
extern TwoWire Wire;
#endif // _TWOWIRE_H_
