#ifndef UTIL_ADC_H
#define UTIL_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libmaple/adc.h>


void start_single_convert(adc_dev* dev, uint8 channel);

void start_continuous_convert(adc_dev* dev, uint8 channel);

void enable_adc_irq(adc_dev* dev);

void enable_internal_reading(adc_dev *dev);

void internalRead(adc_dev *dev, uint8 channel);

/*
    Enable the Watchdog function on the ADC.
*/
static inline void enable_awd(adc_dev * dev) {
	dev->regs->CR1 |= ADC_CR1_AWDEN;
}

static inline void disable_awd(adc_dev * dev) {
	dev->regs->CR1 &= ~ADC_CR1_AWDEN;
}

/*
    Set Analog Watchdog Low Limit.
    Results must be read through interrupt or polled outside this function.
*/
static inline void set_awd_low_limit(adc_dev * dev, uint32 limit) {
	dev->regs->LTR = limit;
}

/*
    Set Analog Watchdog High Limit.
    Results must be read through interrupt or polled outside this function.
*/
static inline void set_awd_high_limit(adc_dev * dev, uint32 limit) {
	dev->regs->HTR = limit;
}

void set_awd_channel( adc_dev * dev, uint8 awd_channel);

void adc_set_reg_seq_channel( adc_dev * dev, unsigned char *channels, unsigned char len);

void set_continuous( adc_dev * dev);

uint8 poll_adc_convert(adc_dev *dev);

void adc_dma_enable(adc_dev * dev);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // UTIL_ADC_H
