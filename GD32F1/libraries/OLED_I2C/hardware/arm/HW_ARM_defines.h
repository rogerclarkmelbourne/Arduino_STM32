// *** Hardwarespecific defines ***
#define cbi(reg, bitmask) *reg &= ~bitmask
#define sbi(reg, bitmask) *reg |= bitmask
#define pulseClock cbi(P_SCK, B_SCK); asm ("nop"); sbi(P_SCK, B_SCK)

#define fontbyte(x) cfont.font[x]  
#define bitmapbyte(x) bitmap[x]

#define bitmapdatatype unsigned char*

#define SDA		20
#define SCL		21
#define SDA1	70
#define SCL1	71

#define TWI_SPEED		TWI_SPEED_400k	// Set default TWI Speed
#define TWI_SPEED_100k	208
#define TWI_SPEED_400k	101

#define TWI_DIV			TWI_DIV_400k	// Set divider for TWI Speed (must match TWI_SPEED setting)
#define TWI_DIV_100k	1
#define TWI_DIV_400k	0
