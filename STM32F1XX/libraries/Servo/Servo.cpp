/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010, LeafLabs, LLC.
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

#include "Servo.h"

#include <boards.h>
#include <io.h>
#include <pwm.h>
#include <wirish_math.h>

// 20 millisecond period config.  For a 1-based prescaler,
//
//    (prescaler * overflow / CYC_MSEC) msec = 1 timer cycle = 20 msec
// => prescaler * overflow = 20 * CYC_MSEC
//
// This picks the smallest prescaler that allows an overflow < 2^16.
#define MAX_OVERFLOW    ((1 << 16) - 1)
#define CYC_MSEC        (1000 * CYCLES_PER_MICROSECOND)
#define TAU_MSEC        20
#define TAU_USEC        (TAU_MSEC * 1000)
#define TAU_CYC         (TAU_MSEC * CYC_MSEC)
#define SERVO_PRESCALER (TAU_CYC / MAX_OVERFLOW + 1)
#define SERVO_OVERFLOW  ((uint16)round((double)TAU_CYC / SERVO_PRESCALER))

// Unit conversions
#define US_TO_COMPARE(us) ((uint16)map((us), 0, TAU_USEC, 0, SERVO_OVERFLOW))
#define COMPARE_TO_US(c)  ((uint32)map((c), 0, SERVO_OVERFLOW, 0, TAU_USEC))
#define ANGLE_TO_US(a)    ((uint16)(map((a), this->minAngle, this->maxAngle, \
                                        this->minPW, this->maxPW)))
#define US_TO_ANGLE(us)   ((int16)(map((us), this->minPW, this->maxPW,  \
                                       this->minAngle, this->maxAngle)))

Servo::Servo() {
    this->resetFields();
}

bool Servo::attach(uint8 pin,
                   uint16 minPW,
                   uint16 maxPW,
                   int16 minAngle,
                   int16 maxAngle) {
    timer_dev *tdev = PIN_MAP[pin].timer_device;

    if (tdev == NULL) {
        // don't reset any fields or ASSERT(0), to keep driving any
        // previously attach()ed servo.
        return false;
    }

    if (this->attached()) {
        this->detach();
    }

    this->pin = pin;
    this->minPW = minPW;
    this->maxPW = maxPW;
    this->minAngle = minAngle;
    this->maxAngle = maxAngle;

    pinMode(pin, PWM);

    timer_pause(tdev);
    timer_set_prescaler(tdev, SERVO_PRESCALER - 1); // prescaler is 1-based
    timer_set_reload(tdev, SERVO_OVERFLOW);
    timer_generate_update(tdev);
    timer_resume(tdev);

    return true;
}

bool Servo::detach() {
    if (!this->attached()) {
        return false;
    }

    timer_dev *tdev = PIN_MAP[this->pin].timer_device;
    uint8 tchan = PIN_MAP[this->pin].timer_channel;
    timer_set_mode(tdev, tchan, TIMER_DISABLED);

    this->resetFields();

    return true;
}

void Servo::write(int degrees) {
    degrees = constrain(degrees, this->minAngle, this->maxAngle);
    this->writeMicroseconds(ANGLE_TO_US(degrees));
}

int Servo::read() const {
    int a = US_TO_ANGLE(this->readMicroseconds());
    // map() round-trips in a weird way we mostly correct for here;
    // the round-trip is still sometimes off-by-one for write(1) and
    // write(179).
    return a == this->minAngle || a == this->maxAngle ? a : a + 1;
}

void Servo::writeMicroseconds(uint16 pulseWidth) {
    if (!this->attached()) {
        ASSERT(0);
        return;
    }

    pulseWidth = constrain(pulseWidth, this->minPW, this->maxPW);
    pwmWrite(this->pin, US_TO_COMPARE(pulseWidth));
}

uint16 Servo::readMicroseconds() const {
    if (!this->attached()) {
        ASSERT(0);
        return 0;
    }

    stm32_pin_info pin_info = PIN_MAP[this->pin];
    uint16 compare = timer_get_compare(pin_info.timer_device,
                                       pin_info.timer_channel);

    return COMPARE_TO_US(compare);
}

void Servo::resetFields(void) {
    this->pin = NOT_ATTACHED;
    this->minAngle = SERVO_DEFAULT_MIN_ANGLE;
    this->maxAngle = SERVO_DEFAULT_MAX_ANGLE;
    this->minPW = SERVO_DEFAULT_MIN_PW;
    this->maxPW = SERVO_DEFAULT_MAX_PW;
}
