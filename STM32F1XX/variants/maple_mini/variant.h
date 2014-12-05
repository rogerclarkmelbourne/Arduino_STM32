#ifndef _VARIANT_ARDUINO_STM32_
#define _VARIANT_ARDUINO_STM32_

// From SAM implementation #define digitalPinToBitMask(P)     ( g_APinDescription[P].ulPin )


#warning "TO DO. IMPLEMENT digitalPinToBitMask in variant.h"
// Its likely that this function has no meaning with reference to the STM32 GPIO
// But its required by some libraries. 
#define digitalPinToBitMask(P)     ( 1 )

#endif /* _VARIANT_ARDUINO_STM32_ */