//#include <wirish/wirish.h>
//#include "libraries/FreeRTOS/MapleFreeRTOS.h"
#include <MapleFreeRTOS821.h>

#include <stm32f10x_gpio.h>
#include <misc.h>
#include <stm32f10x_tim.h>



void setup() {
  // initialize the digital pin as an output:
  Serial.begin(115200);

  GPIO_InitTypeDef GPIOInitStructure;
  GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIOInitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIOInitStructure);

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_TimeBaseInitTypeDef timerInitStructure;
  timerInitStructure.TIM_Prescaler = 7200;
  timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timerInitStructure.TIM_Period = 10000 - 1;
  timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timerInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &timerInitStructure);

  timer_attach_interrupt(TIMER3, TIMER_CC1_INTERRUPT, tim3);
  TIM_ITConfig(TIM3, TIM_IT_Update , ENABLE);
  TIM_Cmd(TIM3, ENABLE);
}

uint32_t count;
uint8_t timUpdate;
void loop() {
  if (timUpdate == 1) {
    Serial.println(count);
    timUpdate=0;
  }
}


void tim3(void)
{
  ++count;
  timUpdate = 1;
  GPIOC->ODR ^= (1 << 13);
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}
