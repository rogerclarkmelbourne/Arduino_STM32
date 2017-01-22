//#include <wirish/wirish.h>
//#include "libraries/FreeRTOS/MapleFreeRTOS.h"
#include <MapleFreeRTOS821.h>

#include <stm32f10x_gpio.h>

static void vLEDFlashTask(void *pvParameters) {
	for (;;) {
		vTaskDelay(1000);
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
		Serial.println(bitRead(GPIOC->IDR, 13), BIN);

		vTaskDelay(1000);
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
		Serial.println(bitRead(GPIOC->IDR, 13), BIN);
	}
}

void setup() {
	// initialize the digital pin as an output:
	Serial.begin(115200);

	GPIO_InitTypeDef GPIOInitStructure;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIOInitStructure);

	xTaskCreate(vLEDFlashTask,
		"Task1",
		configMINIMAL_STACK_SIZE,
		NULL,
		tskIDLE_PRIORITY + 2,
		NULL);
	vTaskStartScheduler();
}

void loop() {
	// Insert background code here
}