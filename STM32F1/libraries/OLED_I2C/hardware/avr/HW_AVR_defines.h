// *** Hardwarespecific defines ***
#define __cbi(reg, bitmask) *reg &= ~bitmask
#define __cbi2(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define __sbi(reg, bitmask) *reg |= bitmask
#define pulseClock __cbi(P_SCK, B_SCK); asm ("nop"); __sbi(P_SCK, B_SCK)

#define fontbyte(x) pgm_read_byte(&cfont.font[x])  
#define bitmapbyte(x) pgm_read_byte(&bitmap[x])  

#define bitmapdatatype uint8_t*

#ifndef TWI_FREQ
	#define TWI_FREQ 400000L
#endif
