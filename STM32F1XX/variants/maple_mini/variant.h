#ifndef _VARIANT_ARDUINO_STM32_
#define _VARIANT_ARDUINO_STM32_

// From SAM implementation #define digitalPinToBitMask(P)     ( g_APinDescription[P].ulPin )
// Added placeholder to please the compiler

#warning "TO DO. IMPLEMENT digitalPinToBitMask in variant.h"
#define digitalPinToBitMask(P)     ( 1 )

#endif /* _VARIANT_ARDUINO_STM32_ */