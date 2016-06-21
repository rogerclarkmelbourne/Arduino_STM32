#include <HAL.h>
#define LED PB1
int c=0;
#define LED_PIN GPIO_PIN_1
#define LED_PORT GPIOB

void setup() 
{
//  pinMode(LED,OUTPUT);
  
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitStruct.Pin = LED_PIN;	
 	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 	GPIO_InitStruct.Pull = GPIO_NOPULL;	
	
  	HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct); 
	
	HAL_GPIO_WritePin(LED_PORT,GPIO_PIN_1,GPIO_PIN_RESET);// Turn off the LED
  
	Serial.begin(115200);
}


void loop()
{
//	Serial.println(c++);

	HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_SET);

	delay(500);

	HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_RESET);

	delay(100);

	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);

	delay(1000);

	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);

	delay(250);
	
}
