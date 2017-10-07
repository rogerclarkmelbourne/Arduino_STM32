#include "util_adc.h"
#include<libmaple/nvic.h>
//#include "boards.h"
#include <libmaple/dma.h>

/*
    Interrupt function.
    This handles Analog watchdog and ADC1 and 2.
*/
extern volatile unsigned int adc_result = 0;

/*
    Starts a single conversion in one channel previously defined.
    Results must be read through interrupt or polled outside this function.
*/
void start_single_convert(adc_dev* dev, uint8 channel) {
//  int pinMapADCin = PIN_MAP[analogInPin].adc_channel;
  adc_set_reg_seqlen(dev, 1);
  dev->regs->SQR3 = channel;//use channels next time.
  dev->regs->CR2 |= ADC_CR2_SWSTART;
  }

/*
    Starts the continuous mode on one channel of the AD.
    Results must be read through interrupt or polled outside this function.
*/
void start_continuous_convert(adc_dev* dev, uint8 channel){
//  int pinMapADCin = PIN_MAP[analogInPin].adc_channel;
  adc_set_reg_seqlen(dev, 1);
  dev->regs->SQR3 = channel;
  dev->regs->CR2 |= ADC_CR2_CONT;
  dev->regs->CR2 |= ADC_CR2_SWSTART;
  }

/*
    Enable end of conversion interrupt on the ADC.
    This is for regular conversion, not injected.
*/
void enable_adc_irq(adc_dev* dev) {//ADC1 for now.
  dev->regs->CR1 |= (1U<<ADC_CR1_EOCIE_BIT);
  nvic_irq_enable(NVIC_ADC_1_2 );
  }

/*
    Enable the reading of the internal variables (Temperature and Vref).
*/
void enable_internal_reading(adc_dev *dev) {
  dev->regs->CR2 |= ADC_CR2_TSVREFE;
  }

/*
    Read internal variables.
    Channels are:
    16 - Temperature
    17 - VrefInt
    Results must be read through interrupt or polled outside this function.
*/
void internalRead(adc_dev *dev, uint8 channel) {
    adc_reg_map *regs = dev->regs;
    adc_set_reg_seqlen(dev, 1);
    regs->SQR3 = channel;
    regs->CR2 |= ADC_CR2_SWSTART;
    }

/*
    Enable the Analog Watchdog interrupt
*/
void enable_awd_irq(adc_dev * dev){
  dev->regs->CR1 |= (1U<<ADC_CR1_AWDIE_BIT);
  nvic_irq_enable(NVIC_ADC_1_2 );
  }

/*
    Set Analog Watchdog Low Limit.
    Results must be read through interrupt or polled outside this function.
*/
void set_awd_low_limit(adc_dev * dev, uint32 limit) {
  dev->regs->LTR = limit;
  }

/*
    Set Analog Watchdog High Limit.
    Results must be read through interrupt or polled outside this function.
*/
void set_awd_high_limit(adc_dev * dev, uint32 limit) {
  dev->regs->HTR = limit;
  }

/*
    Enable the Watchdog function on the ADC.
*/
void set_awd_channel(adc_dev * dev, uint8 awd_channel){
    dev->regs->CR1 |= (awd_channel & ADC_CR1_AWDCH);
  }


/*
    Enable the Watchdog function on the ADC.
*/
void enable_awd(adc_dev * dev){
  dev->regs->CR1 |= ADC_CR1_AWDEN;
  }


/*
    Used to configure the sequence and length of the scan mode.
    Can be used instead of adc_set_reg_seqlen() as it sets both information
    The channels are not the pin numbers, but ADC channels.
*/
void adc_set_reg_seq_channel(adc_dev * dev, unsigned char *channels, unsigned char len){

  unsigned int records[3] = {0,0,0};
  unsigned char i = 0, j = 0;
//run away protection
  if (len > 16) len = 16;

//write the length
  records[2] |= (len - 1) << 20;

//i goes through records, j goes through variables.
  for (i = 0, j = 0; i < len; i++) {//go through the channel list.
    if (i!=0 && i%6 == 0) j++;//next variable, please!!
    records[j] |= (channels[i] << ((i%6)*5));
    }
//update the registers inside with the scan sequence.
  dev->regs->SQR1 = records[2];
  dev->regs->SQR2 = records[1];
  dev->regs->SQR3 = records[0];
  };

void adc_dma_disable(adc_dev * dev) {
  bb_peri_set_bit(&dev->regs->CR2, ADC_CR2_DMA_BIT, 0);
}


void adc_dma_enable(adc_dev * dev) {
  bb_peri_set_bit(&dev->regs->CR2, ADC_CR2_DMA_BIT, 1);
}

uint8 poll_adc_convert(adc_dev * dev) {
  return bb_peri_get_bit(&dev->regs->SR, ADC_SR_EOC_BIT);
  }



//uint8 maxSamples = 32;
//uint32_t dataPoints32[maxSamples / 2];
//uint16_t *dataPoints = (uint16_t *)&dataPoints32;

/*
//fast interleaved mode
void setADCs (uint8 analogInPin)
{
  //  const adc_dev *dev = PIN_MAP[analogInPin].adc_device;
  int pinMapADCin = PIN_MAP[analogInPin].adc_channel;
  adc_set_sample_rate(ADC1, ADC_SMPR_1_5); //=0,58uS/sample.  ADC_SMPR_13_5 = 1.08uS - use this one if Rin>10Kohm,
  adc_set_sample_rate(ADC2, ADC_SMPR_1_5);    // if not may get some sporadic noise. see datasheet.

  //  adc_reg_map *regs = dev->regs;
  adc_set_reg_seqlen(ADC1, 1);
  ADC1->regs->SQR3 = pinMapADCin;
  ADC1->regs->CR2 |= ADC_CR2_CONT; // | ADC_CR2_DMA; // Set continuous mode and DMA
  ADC1->regs->CR1 |= ADC_CR1_FASTINT; // Interleaved mode
  ADC1->regs->CR2 |= ADC_CR2_SWSTART;

  ADC2->regs->CR2 |= ADC_CR2_CONT; // ADC 2 continuos
  ADC2->regs->SQR3 = pinMapADCin;
}
*/

/*
    This is run inside the loop() function.
    It stays active and polls for the end of the transfer.

*/


/*
void takeSamples ()
{
  // This loop uses dual interleaved mode to get the best performance out of the ADCs
  //
  unsigned long samplingTime=0;
  dma_init(DMA1);
  dma_attach_interrupt(DMA1, DMA_CH1, DMA1_CH1_Event);

  adc_dma_enable(ADC1);
  dma_setup_transfer(DMA1, DMA_CH1, &ADC1->regs->DR, DMA_SIZE_32BITS,
                     dataPoints32, DMA_SIZE_32BITS, (DMA_MINC_MODE | DMA_TRNS_CMPLT));// Receive buffer DMA
  dma_set_num_transfers(DMA1, DMA_CH1, maxSamples / 2);
  dma1_ch1_Active = 1;
  //  regs->CR2 |= ADC_CR2_SWSTART; //moved to setADC
  dma_enable(DMA1, DMA_CH1); // Enable the channel and start the transfer.
  //adc_calibrate(ADC1);
  //adc_calibrate(ADC2);
  samplingTime = micros();
  while (dma1_ch1_Active);
  samplingTime = (micros() - samplingTime);

  dma_disable(DMA1, DMA_CH1); //End of trasfer, disable DMA and Continuous mode.
  // regs->CR2 &= ~ADC_CR2_CONT;

}

*/
