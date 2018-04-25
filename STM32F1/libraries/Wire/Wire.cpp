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
 * @file TwoWire.cpp
 * @author Trystan Jones <crenn6977@gmail.com>
 * @brief Wire library, uses the hardware I2C available in the Maple to
 *        interact with I2C slave devices.
 */

/*
 * Library created by crenn to use the new WireBase system and allow Arduino
 * users easy interaction with the I2C Hardware in a familiar method.
 */

#include "Wire.h"

uint8 TwoWire::process(uint8 stop) {
    int8 res = i2c_master_xfer(sel_hard, &itc_msg, 1, 0);
    if (res == I2C_ERROR_PROTOCOL) {
        if (sel_hard->error_flags & I2C_SR1_AF) { /* NACK */
            res = (sel_hard->error_flags & I2C_SR1_ADDR ? ENACKADDR : 
                                                          ENACKTRNS);
        } else if (sel_hard->error_flags & I2C_SR1_OVR) { /* Over/Underrun */
            res = EDATA;
        } else { /* Bus or Arbitration error */
            res = EOTHER;
        }
        i2c_disable(sel_hard);
        i2c_master_enable(sel_hard, dev_flags);
    }
    return res;
}

uint8 TwoWire::process(){
	return process(true);
}

// TODO: Add in Error Handling if devsel is out of range for other Maples
TwoWire::TwoWire(uint8 dev_sel, uint8 flags) {
    if (dev_sel == 1) {
        sel_hard = I2C1;
    } else if (dev_sel == 2) {
        sel_hard = I2C2;
    } else {
        ASSERT(1);
    }
    dev_flags = flags;
}

TwoWire::~TwoWire() {
    i2c_disable(sel_hard);
    sel_hard = 0;
}

void TwoWire::begin(uint8 self_addr) {
    i2c_master_enable(sel_hard, dev_flags);
}

void TwoWire::end() {
    i2c_disable(sel_hard);
    sel_hard = 0;
}

void TwoWire::setClock(uint32_t frequencyHz)
{
	switch(frequencyHz)
	{
		case 400000:
			dev_flags |= I2C_FAST_MODE;// set FAST_MODE bit
			break;
		case 100000:
		default:
			dev_flags &= ~I2C_FAST_MODE;// clear FAST_MODE bit
			break;
	}
	if (sel_hard->regs->CR1 & I2C_CR1_PE){
	    i2c_disable(sel_hard);
	    i2c_master_enable(sel_hard, dev_flags);
	}
}

TwoWire Wire(1);
