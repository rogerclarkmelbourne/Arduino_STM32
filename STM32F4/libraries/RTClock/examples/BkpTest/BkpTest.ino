#include <RTClock.h>

#include <Arduino.h>
#include <RTClock.h>
#include <usb_serial.h>
#include <libmaple/bkp.h>
#include <string.h>

RTClock rt; // initialise

void setbkpvalues();
void readbkpvalues();
void sleep(uint16_t dur);

int ledPin = BOARD_LED_PIN;

// the setup() method runs once when the sketch starts
void setup() {

	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW); //turn on the led

	Serial.begin();
	//wait for keypress
	while(!Serial.available()) sleep(1);
	while(Serial.available()) Serial.read();

	Serial.println("init");

	// enable backup domain clock
	bkp_init();
	/* enable backup sram
	 * bkp_init() needs to be called prior
	 * passing true enable the backup power regulator, runs on VBAT e.g. coin cell
	 * passing false BKPSRAM is lost if VDD is lost, but preserves across a reset
	 * if only backup sram is used rt.begin() can be skipped, but bkp_init() need to be called prior
	 */
	bkp_initsram(false);
	// setup RTClock,
	// note that an initialized RTClock is necessary to access backup registers.
	// if only backup registers are accessed, bkp_initsram() can be skipped
	rt.begin();

	Serial.println("bkp values:");
	readbkpvalues();
	Serial.flush();
	sleep(500);

	Serial.println("setting bkp values:");
	Serial.flush();
	setbkpvalues();
	sleep(500);

	Serial.println("bkp values:");
	readbkpvalues();
	Serial.flush();

	digitalWrite(ledPin, !digitalRead(ledPin));
}

//the loop() method runs over and over again, 
//as long as maple has power
void loop() {
	sleep(1000);
}

void setbkpvalues() {
	const char *text = (const char*)F("a quick brown fox jumps over the lazy dog");

	// before writing to bkp registers it is necessary to enable writes
	// or it will hardfault / freeze
	bkp_enable_writes();
	// set values in backup registers
	// write values 1000001 .. 1000020 in backup registers
	for (uint8_t r=1; r<=20; r++) {
		bkp_write(r, 100000 + r);
	}
	bkp_disable_writes();

	// check if BKPSRAM is enabled, if not skip writing them
	if(!(RCC->AHB1ENR & RCC_AHB1ENR_BKPSRAMEN))
		return;

	//before writing to bkpsram it is necessary to enable writes
	// or it will hardfault / freeze
	bkp_enable_writes();
	//clear bkp sram
	bkpsram_clear();

	// write 100 in 2nd byte in BKP_SRAM
	bkp_sramwrite8(1, 100);

	// write 10000 in 2nd uint16_t word in BKP_SRAM
	bkp_sramwrite16(1, 10000);

	// write 1000000 in 2nd uint32_t word in BKP_SRAM
	bkp_sramwrite32(1, 1000000);

	//write text in BKPSRAM at offset 10
	bkp_sramwrite(10, (uint8_t *)text, 41);
	bkp_disable_writes();

}

void readbkpvalues() {
	char buf[50];

	// read backup registers
	for (uint8_t r=1; r<=20; r++) {
		uint32_t v = bkp_read(r);
		Serial.print("reg :");
		Serial.print(r);
		Serial.print(':');
		Serial.println(v);
	}

	// check if BKPSRAM is enabled, if not skip reading them
	if(!(RCC->AHB1ENR & RCC_AHB1ENR_BKPSRAMEN))
		return;

	uint32_t val = 0;
	// read 100 in 2nd byte in BKP_SRAM
	val = bkp_sramread8(1);
	Serial.print("BKPSRAM 2nd byte:");
	Serial.println(val);

	// read 10000 in 2nd uint16_t word in BKP_SRAM
	val = 0;
	val = bkp_sramread16(1);
	Serial.print("BKPSRAM 2nd uint16 word:");
	Serial.println(val);

	// read 1000000 in 2nd uint32_t word in BKP_SRAM
	val = 0;
	val = bkp_sramread32(1);
	Serial.print("BKPSRAM 2nd uint32 word:");
	Serial.println(val);

	memset(buf, 0, 50);
	memcpy(buf, bkp_sramread(10), 41);
	Serial.print("BKPSRAM text at offset 10:");
	Serial.println(buf);

}

void sleep(uint16_t dur) {
	//sleep 1ms - wait for systick interrupt
	for(uint16_t i=0; i<dur; i++)
		asm("wfi");
}
