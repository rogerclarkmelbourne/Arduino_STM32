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
 * @file HardWire.cpp
 * @author Trystan Jones <crenn6977@gmail.com>
 * @brief Wire library, uses the hardware I2C available in the Maple to
 *        interact with I2C slave devices.
 */

/*
 * Library created by crenn to use the new WireBase system and allow Arduino
 * users easy interaction with the I2C Hardware in a familiar method.
 */

#include "HardWire.h"

uint8 HardWire::process() {
    int8 res = i2c_master_xfer(sel_hard, &itc_msg, 1, 0);
    if (res != 0) {
        i2c_disable(sel_hard);
        i2c_master_enable(sel_hard, (I2C_BUS_RESET | dev_flags));
    }
    return 0;
}

// TODO: Add in Error Handling if devsel is out of range for other Maples
HardWire::HardWire(uint8 dev_sel, uint8 flags) {
    if (dev_sel == 1) {
        sel_hard = I2C1;
    } else if (dev_sel == 2) {
        sel_hard = I2C2;
    } else {
        ASSERT(1);
    }
    dev_flags=flags;
    i2c_master_enable(sel_hard, flags);
}

HardWire::~HardWire() {
    i2c_disable(sel_hard);
    sel_hard = 0;
}
