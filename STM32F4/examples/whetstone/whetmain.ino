//This is the same whetstone code with adjustments for the Serial.print function on the Arduino
//To run the code on the Arduino copy and paste the content of "arduino.txt" into the Arduino IDE. 
//The main loop within the procedure whetstone() is identical. Only the output format is Arduino-specific.

#include "Arduino.h"
#include "whetstone.h"
#include <usb_serial.h>
void enablefpu( void );

void setup() {

	/* Initializations */
	pinMode(BOARD_LED_PIN, OUTPUT);

	Serial.begin();
	delay(3000); // wait a while for host to connect


/*
 *  FPU is enabled in board_init in the core itself. Hence we'd not need to enable the fpu here
 *
 *  Note that to use hardware fpu floating point
 *
 * 	the compile options
 *		-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant
 *	or
 *		-mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -fsingle-precision-constant
 *	needs to be stated

#if defined (__GNUC__) && defined (__VFP_FP__) && !defined(__SOFTFP__)
#warning enabling harware fpu
	enablefpu();
#else
#warning using software floating point
#endif
*/
}

void loop() {
	int fcpu = 0;

	digitalWrite(BOARD_LED_PIN, LOW); //led is active low
	Serial.println("press 'g' to start");
	// wait for a key g
	while (1) {
		byte c;
		if (Serial.available())
			c = Serial.read();
		if (c == 'g') {
			break;
		}
		delay(100);
	}

	digitalWrite(BOARD_LED_PIN, HIGH); //led is active low

#ifdef F_CPU
	fcpu = F_CPU / 1000000;
#endif

	whetstone(fcpu);
}

void blinks() {
	for (int i = 0; i < 5; i++) {
		digitalWrite(BOARD_LED_PIN, HIGH);
		delay(100);
		digitalWrite(BOARD_LED_PIN, LOW);
		delay(100);
	}
}

//enable the fpu (cortex-m4 - stm32f4* and above)
void enablefpu()
{
  __asm volatile
  (
    "  ldr.w r0, =0xE000ED88    \n"  /* The FPU enable bits are in the CPACR. */
    "  ldr r1, [r0]             \n"  /* read CAPCR */
    "  orr r1, r1, #( 0xf << 20 )\n" /* Set bits 20-23 to enable CP10 and CP11 coprocessors */
    "  str r1, [r0]              \n" /* Write back the modified value to the CPACR */
    "  dsb                       \n" /* wait for store to complete */
    "  isb"                          /* reset pipeline now the FPU is enabled */

  );
}

