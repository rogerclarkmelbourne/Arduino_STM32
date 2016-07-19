/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2011, 2012 LeafLabs, LLC.
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
 * @file wirish/include/wirish/HardwareSerial.h
 * @brief Wirish serial port interface.
 */

#ifndef _WIRISH_HARDWARESERIAL_H_
#define _WIRISH_HARDWARESERIAL_H_

#include <libmaple/libmaple_types.h>

#include <wirish/Print.h>
#include <wirish/boards.h>

/*
 * IMPORTANT:
 *
 * This class documented "by hand" (i.e., not using Doxygen) in the
 * leaflabs-docs/ repository.
 *
 * If you alter the public HardwareSerial interface, you MUST update
 * the documentation accordingly.
 */

struct usart_dev;

class HardwareSerial : public Print {
public:
    HardwareSerial(struct usart_dev *usart_device,
                   uint8 tx_pin,
                   uint8 rx_pin);

    /* Set up/tear down */
    void begin(uint32 baud);
    void end(void);

    /* I/O */
    uint32 available(void);
    int read(void);
    void flush(void);
    virtual void write(unsigned char);
    using Print::write;

    /* Pin accessors */
    int txPin(void) { return this->tx_pin; }
    int rxPin(void) { return this->rx_pin; }

    /* Escape hatch into libmaple */
    /* FIXME [0.0.13] documentation */
    struct usart_dev* c_dev(void) { return this->usart_device; }
private:
    struct usart_dev *usart_device;
    uint8 tx_pin;
    uint8 rx_pin;
};

#if BOARD_HAVE_USART1
extern HardwareSerial Serial1;
#endif
#if BOARD_HAVE_USART2
extern HardwareSerial Serial2;
#endif
#if BOARD_HAVE_USART3
extern HardwareSerial Serial3;
#endif
#if BOARD_HAVE_UART4
extern HardwareSerial Serial4;
#endif
#if BOARD_HAVE_UART5
extern HardwareSerial Serial5;
#endif
#if BOARD_HAVE_USART6
extern HardwareSerial Serial6;
#endif

#endif
