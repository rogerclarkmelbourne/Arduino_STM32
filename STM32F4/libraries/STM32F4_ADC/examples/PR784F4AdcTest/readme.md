## This sketch is a test for some functions added to the F4 core in adc.h and adc.c added in PR 784

ref: [PR 784 F4: updates in adc.h and adc.c #784](https://github.com/rogerclarkmelbourne/Arduino_STM32/pull/784)

### How to run this sketch

Build the sketch against the F4 libmaple core and install it on the board. The boards that should work 
may include STM32F4xx( STM32F401, STM32F411, STM32F405, STM32F407, STM32F427, STM32F429) boards.

Connect to the serial terminal and it should start printing the core temperature.
it responds to a single char command 'w' which toggles the analog watch dog on and off.

### * ADC_PRESCALER

The function adc_set_prescaler() has been added in adc.h, adc.c
adc.h
```
/**
 * @brief STM32F1/F4 ADC prescalers, as divisors of PCLK2.
 */
typedef enum adc_prescaler {
    /** PCLK2 divided by 2 */
    ADC_PRE_PCLK2_DIV_2 = 0,
    /** PCLK2 divided by 4 */
    ADC_PRE_PCLK2_DIV_4 = 1,
    /** PCLK2 divided by 6 */
    ADC_PRE_PCLK2_DIV_6 = 2,
    /** PCLK2 divided by 8 */
    ADC_PRE_PCLK2_DIV_8 = 3,
} adc_prescaler;

void adc_set_prescaler(adc_prescaler pre);
```
### How is this tested in the sketch

The function in the sketch void ``setADCprescaler()`` selects a prescalar to get the highest
adc clocks based on the define CLOCK_SPEED_MHZ defined in the variant specific include file.

 
### * additional fixes and support for overrun interrupt and awd interrupt, added adc_awd_disable()

Updated codes in ``adc_attach_interrupt()`` and ``adc_irq_handler()`` to support overrun interrupt and awd
(analog watch dog) interrupt.
added missing function ``adc_awd_disable()`` in adc.h, adc.c

 
### How is this tested in the sketch

This sketch sets up an analog watch dog to test an upper threshold voltage. in this case it reads 
the internal temperature sensor and triggers when a higher temperature is detected

### How is the overrun interrupt tested in the sketch

To test the overrun interrupt you need to uncomment the statements in ``loop()``:
```
    }
    uint16_t data = adc_get_data(ADC1);
    
    // uncomment next 3 statements to test overrun
    ADC1->regs->CR2 |= ADC_CR2_SWSTART;
    delay(1);
    ADC1->regs->CR2 |= ADC_CR2_SWSTART;
```	
The sketch would respond with printing a "ADC Overrun!" and blinks the led. 

