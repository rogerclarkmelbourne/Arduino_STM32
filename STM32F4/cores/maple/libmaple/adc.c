/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 * @file adc.c
 *
 * @brief Analog to digital converter routines
 *
 * IMPORTANT: maximum external impedance must be below 0.4kOhms for 1.5
 * sample conversion time.
 *
 * At 55.5 cycles/sample, the external input impedance < 50kOhms.
 *
 * See STM32 manual RM0008 for how to calculate this.
 */

#include "libmaple.h"
#include "rcc.h"
#include "adc.h"
#include "wirish_time.h"


voidFuncPtr adc1Handlers[ADC_LAST_IRQ_ID] = {NULL};
voidFuncPtr adc2Handlers[ADC_LAST_IRQ_ID] = {NULL};
voidFuncPtr adc3Handlers[ADC_LAST_IRQ_ID] = {NULL};

/** ADC1 device. */
const adc_dev adc1 = {
    .regs   = ADC1_BASE,
    .clk_id = RCC_ADC1,
    .dmaStream = DMA_STREAM0, // or DMA_STREAM4
    .dmaChannel = DMA_CH0,
    .handler_p = &adc1Handlers
};

/** ADC2 device. */
const adc_dev adc2 = {
    .regs   = ADC2_BASE,
    .clk_id = RCC_ADC2,
    .dmaStream = DMA_STREAM2, // or DMA_STREAM3
    .dmaChannel = DMA_CH1,
    .handler_p = &adc2Handlers
};

/** ADC3 device. */
const adc_dev adc3 = {
    .regs   = ADC3_BASE,
    .clk_id = RCC_ADC3,
    .dmaStream = DMA_STREAM0, // or DMA_STREAM1
    .dmaChannel = DMA_CH2,
    .handler_p = &adc3Handlers
};

adc_common_reg_map* const ADC_COMMON = ADC_COMMON_BASE;

/**
 * @brief Initialize an ADC peripheral.
 *
 * Initializes the RCC clock line for the given peripheral.  Resets
 * ADC device registers.
 *
 * @param dev ADC peripheral to initialize
 */
void adc_init(const adc_dev *dev)
{
    rcc_clk_enable(dev->clk_id);
    if(dev->clk_id == RCC_ADC1) {
        rcc_reset_dev(dev->clk_id);
    }
}

/**
 * @brief Set external trigger for regular channels
 * @param dev    ADC device
 * @param adc_exten_trigger
 */
void adc_set_exttrig(const adc_dev *dev, adc_ext_trigger trigger)
{
    uint32 cr2 = dev->regs->CR2 & (~ADC_CR2_EXTEN);
    dev->regs->CR2 = cr2 | (trigger << ADC_CR2_EXTEN_SHIFT);
}

void adc_set_jexttrig(const adc_dev *dev, adc_ext_trigger trigger)
{
    uint32 cr2 = dev->regs->CR2 & (~ADC_CR2_JEXTEN);
    dev->regs->CR2 = cr2 | (trigger << ADC_CR2_JEXTEN_SHIFT);
}

/**
 * @brief Set external event select for regular group
 * @param dev ADC device
 * @param event Event used to trigger the start of conversion.
 * @see adc_extsel_event
 */
void adc_set_extsel(const adc_dev *dev, adc_extsel_event event)
{
    uint32 cr2 = dev->regs->CR2 & (~ADC_CR2_EXTSEL);
    dev->regs->CR2 = cr2 | (event << ADC_CR2_EXTSEL_SHIFT);
}

void adc_set_jextsel(const adc_dev *dev, adc_jextsel_event event)
{
    uint32 cr2 = dev->regs->CR2 & (~ADC_CR2_JEXTSEL);
    dev->regs->CR2 = cr2 | (event << ADC_CR2_JEXTSEL_SHIFT);
}

/*
    enable interrupts on the ADC: ADC_EOC, ADC_JEOC, ADC_AWD
*/
void adc_enable_irq(const adc_dev* dev)
{
    dev->regs->CR1 |= ADC_CR1_EOCIE;
    nvic_irq_enable(NVIC_ADC_1_2_3);
}

void adc_disable_irq(const adc_dev* dev)
{
    nvic_irq_disable(NVIC_ADC_1_2_3);
    dev->regs->CR1 &= ~ADC_CR1_EOCIE;
}

void adc_awd_enable_irq(const adc_dev * dev)
{
    dev->regs->CR1 |= ADC_CR1_AWDIE;
    nvic_irq_enable(NVIC_ADC_1_2_3);
}

/*
    attach interrupt functionality for ADC
    use ADC_EOC, ADC_JEOC, ADC_AWD
    added by bubulindo
*/
void adc_attach_interrupt(const adc_dev *dev,
                            adc_irq_id irq_id,
                            voidFuncPtr handler)
{
    if (irq_id<ADC_LAST_IRQ_ID)
    {
        (*(dev->handler_p))[irq_id] = handler;
        adc_enable_irq(dev);
    }
}

void adc_detach_interrupt(const adc_dev *dev, adc_irq_id irq_id)
{
    if (irq_id<ADC_LAST_IRQ_ID)
    {
        (*(dev->handler_p))[irq_id] = NULL;
    }
}

/**
 * @brief Call a function on all ADC devices.
 * @param fn Function to call on each ADC device.
 */
void adc_foreach(void (*fn)(const adc_dev*))
{
    fn(ADC1);
    fn(ADC2);
    fn(ADC3);
}

/**
 * @brief Set the given sampling times for all channels.
 *        Don't call this during conversion!
 * @param dev adc device
 * @param smpl_time sampling time to set
 * @see adc_smp_rate
 */
void adc_set_sampling_time(const adc_dev *dev, adc_smp_rate smpl_time)
{
    uint32 adc_smpr_val = 0;

    for (uint8 i = 0; i < 10; i++)
    {
        adc_smpr_val |= smpl_time << (i * 3);
    }

    dev->regs->SMPR2 = adc_smpr_val; // channels 0..9
    dev->regs->SMPR1 = adc_smpr_val&0x7FFFFFF; // channels 10..18
}

/*
    Starts a single conversion in one channel previously defined.
    Results must be read through interrupt or polled outside this function.
*/
void adc_start_single_convert(const adc_dev* dev, uint8 channel)
{
    adc_set_reg_seqlen(dev, 1);
    dev->regs->SQR3 = channel;
    adc_start_convert(dev);
}

/*
    Starts the continuous mode on one channel of the AD.
    Results must be read through interrupt or polled outside this function.
*/
void adc_start_continuous_convert(const adc_dev* dev, uint8 channel)
{
    adc_set_continuous(dev);
    adc_start_single_convert(dev, channel);
}

/**
 * @brief Perform a single synchronous software triggered conversion on a
 * channel.
 * @param dev ADC device to use for reading.
 * @param channel channel to convert
 * @return conversion result
 */
uint16 adc_read(const adc_dev *dev, uint8 channel)
{
    adc_start_single_convert(dev, channel);

    while ( !adc_is_end_of_convert(dev) );

    return adc_get_data(dev);
}

void adc_enable_tsvref(void)
{
    ADC_COMMON->CCR |= ADC_CCR_TSVREFE;
    delayMicroseconds(10);
}

void adc_enable_vbat(void)
{
    ADC_COMMON->CCR |= ADC_CCR_VBATE;
    delayMicroseconds(10);
}

//-----------------------------------------------------------------------------
void adc_set_reg_sequence(const adc_dev * dev, uint8 * channels, uint8 len)
{
    //run away protection
    if (len > 16) len = 16;

    uint32_t seq_reg[3];
    //write the length
    seq_reg[0] = seq_reg[1] = 0;
    seq_reg[2] = (len - 1) << 20;

    //i goes through records, j goes through variables.
    for (uint8 i = 0; i < len; i++) { //go through the channel list.
        seq_reg[i/6] |= (channels[i] << ((i%6)*5));
    }
    //update the registers inside with the scan sequence.
    dev->regs->SQR1 = seq_reg[2];
    dev->regs->SQR2 = seq_reg[1];
    dev->regs->SQR3 = seq_reg[0];
};

/*
    ADC IRQ handler. 
    added by bubulindo
*/
void adc_irq_handler(const adc_dev * dev)
{
    //End Of Conversion
    if (dev->regs->SR & ADC_SR_EOC) {
        dev->regs->SR = ~ADC_SR_EOC;
        voidFuncPtr handler = (*(dev->handler_p))[ADC_EOC];
        if (handler) {
            handler();
        }
    }
    //Injected End Of Conversion
    if (dev->regs->SR & ADC_SR_JEOC) {
        dev->regs->SR = ~ADC_SR_JEOC;
        voidFuncPtr handler = (*(dev->handler_p))[ADC_JEOC];
        if (handler) {
            handler();
        }
    }
    //Analog Watchdog
    if (dev->regs->SR & ADC_SR_AWD) {
        dev->regs->SR = ~ADC_SR_AWD;
        voidFuncPtr handler = (*(dev->handler_p))[ADC_AWD];
        if (handler) {
            handler();
        }
    }
}

void __irq_adc()
{
    adc_foreach(&adc_irq_handler);
}
//-----------------------------------------------------------------------------
//  Read internal variables.
//  Channels are:
//  16 - Temperature
//  17 - VrefInt
//  Results must be read through interrupt or polled outside this function.
/*
inline void internalRead(adc_dev *dev, uint8 channel) {
    adc_reg_map *regs = dev->regs;
    adc_set_reg_seqlen(dev, 1);
    regs->SQR3 = channel;
    regs->CR2 |= ADC_CR2_SWSTART;
}
*/

/*
    Used to configure the sequence and length of the scan mode.
    Can be used instead of adc_set_reg_seqlen() as it sets both information
    The channels are not the pin numbers, but ADC channels.
*/

/*
//fast interleaved mode
void setADCs (uint8 analogInPin)
{
  //  const adc_dev *dev = adc_devices[PIN_MAP[analogInPin].adc_device];
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
  samplingTime = micros();
  while (dma1_ch1_Active);
  samplingTime = (micros() - samplingTime);

  dma_disable(DMA1, DMA_CH1); //End of trasfer, disable DMA and Continuous mode.
  // regs->CR2 &= ~ADC_CR2_CONT;

}

*/
//-----------------------------------------------------------------------------
void setupADC_F4(void)
{
    uint32 tmpreg1 = 0;

    tmpreg1 = ADC_COMMON->CCR;

    /* Clear all bits */
#define CR_CLEAR_MASK  ((uint32)0xFF3C10E0)
    tmpreg1 &= CR_CLEAR_MASK;

    /* Configure ADCx: Multi mode, Delay between two sampling time, ADC prescaler,
     and DMA access mode for multimode */
    /* Set MULTI bits according to ADC_Mode value */
    /* Set ADCPRE bits according to ADC_Prescaler value */
    /* Set DMA bits according to ADC_DMAAccessMode value */
    /* Set DELAY bits according to ADC_TwoSamplingDelay value */
#define ADC_Mode_Independent            0
#define ADC_Prescaler_Div2              0
#define ADC_DMAAccessMode_Disabled      0     /* DMA mode disabled */
#define ADC_TwoSamplingDelay_5Cycles    0

    tmpreg1 |= ( ADC_Mode_Independent | ADC_Prescaler_Div2 | ADC_DMAAccessMode_Disabled | ADC_TwoSamplingDelay_5Cycles );

    /* Write to ADC CCR */
    ADC_COMMON->CCR = tmpreg1;
}
