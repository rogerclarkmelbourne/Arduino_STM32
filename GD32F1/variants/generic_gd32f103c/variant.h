#ifndef _VARIANT_ARDUINO_STM32_
#define _VARIANT_ARDUINO_STM32_

#define digitalPinToPort(P)        ( PIN_MAP[P].gpio_device )
#define digitalPinToBitMask(P)     ( BIT(PIN_MAP[P].gpio_bit) )
#define portOutputRegister(port)   ( &(port->regs->ODR) )
#define portInputRegister(port)    ( &(port->regs->IDR) )

#define portSetRegister(pin)		( &(PIN_MAP[pin].gpio_device->regs->BSRR) )
#define portClearRegister(pin)		( &(PIN_MAP[pin].gpio_device->regs->BRR) )

#define portConfigRegister(pin)		( &(PIN_MAP[pin].gpio_device->regs->CRL) )

#endif /* _VARIANT_ARDUINO_STM32_ */