#include <libmaple/adc.h>


void start_single_convert(adc_dev* dev, uint8 channel);

void start_continuous_convert(adc_dev* dev, uint8 channel);

void enable_adc_irq(adc_dev* dev);

void enable_internal_reading(adc_dev *dev);

void internalRead(adc_dev *dev, uint8 channel);

void enable_awd_irq( adc_dev * dev);

void set_awd_low_limit( adc_dev * dev, uint32 limit);

void set_awd_high_limit( adc_dev * dev, uint32 limit);

void enable_awd( adc_dev * dev);

void set_awd_channel( adc_dev * dev, uint8 awd_channel);

void adc_set_reg_seq_channel( adc_dev * dev, unsigned char *channels, unsigned char len);

void set_continuous( adc_dev * dev);

uint8 poll_adc_convert(adc_dev *dev);

void adc_dma_enable(adc_dev * dev);