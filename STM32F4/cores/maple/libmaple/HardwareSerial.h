/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
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
 * @file HardwareSerial.h
 * @brief Wirish serial port interface.
 */

#ifndef _HARDWARESERIAL_H_
#define _HARDWARESERIAL_H_

#include "libmaple_types.h"
#include "usart.h"

#include "Print.h"

/*
 * IMPORTANT:
 *
 * This class documented "by hand" (i.e., not using Doxygen) in the
 * leaflabs-docs/ repository.
 *
 * If you alter the public HardwareSerial interface, you MUST update
 * the documentation accordingly.
 */

class HardwareSerial : public Print {
public:
    HardwareSerial(usart_dev *usart_device,
                   uint8 tx_pin,
                   uint8 rx_pin);

    /* Set up/tear down */
    void begin(uint32 baud);
    void end(void);

    /* I/O */
    uint32 available(void);
    uint32 pending(void);
    int read(void);
    void flush(void);
    virtual void write(unsigned char);
    using Print::write;

    /* Pin accessors */
    int txPin(void) { return this->tx_pin; }
    int rxPin(void) { return this->rx_pin; }
private:
    usart_dev *usart_device;
    uint8 tx_pin;
    uint8 rx_pin;
};

extern HardwareSerial Serial1;
extern HardwareSerial Serial2;
extern HardwareSerial Serial3;
#if defined(STM32_HIGH_DENSITY) && !defined(BOARD_maple_RET6)
extern HardwareSerial Serial4;
extern HardwareSerial Serial5;
#endif
extern HardwareSerial &SerialDebug;
#endif
