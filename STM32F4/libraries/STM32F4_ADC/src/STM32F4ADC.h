#include <Arduino.h>
#include <HardwareTimer.h>
#include <libmaple/adc.h>
#include <boards.h>


class STM32ADC
{
public:

/*
    Constructor
    Choose which ADC to use.
    Start it up...
*/
    STM32ADC(const adc_dev * dev) { _dev = dev; }

    uint16 readInput(uint8 pin) { return analogReadDev(pin, _dev); }

    uint32 getData() { return adc_get_data(_dev); }

    void enableTsVref(void) { adc_enable_tsvref(); }

    void enableVBAT(void) { adc_enable_vbat(); }

/*
    Set the ADC sampling time.

    ADC_SMPR_3,               <  3 ADC cycles
    ADC_SMPR_15,              < 15 ADC cycles
    ADC_SMPR_28,              < 28 ADC cycles
    ADC_SMPR_56,              < 56 ADC cycles
    ADC_SMPR_84,              < 84 ADC cycles
    ADC_SMPR_112,             < 112 ADC cycles
    ADC_SMPR_144,             < 144 ADC cycles
    ADC_SMPR_480,             < 480 ADC cycles
*/
    void setSamplingTime(adc_smp_rate samplingTime) 
    {
        adc_set_sampling_time(_dev, samplingTime);
    }

/*
    Attach an interrupt to the ADC completion.
*/
    void attachInterrupt(voidFuncPtr func, adc_irq_id irq_id)
    {
        adc_attach_interrupt(_dev, irq_id, func);
    }

/*
    This function is used to setup DMA with the ADC. 
    Used in continuous or scan mode.
*/
    void enableDMA()
    {
        dma_enable(DMA2, _dev->dmaStream); // enable DMA stream
        adc_dma_enable(_dev); // enable ADC DMA transfer
    }
    void disableDMA()
    {
        adc_dma_disable(_dev); // disable ADC DMA transfer
        dma_disable(DMA2, _dev->dmaStream); // disable DMA stream
    }

    void setDMASingle()
    {
        adc_dma_single(_dev); // single DMA buffer
    }
    void setDMACircular()
    {
        adc_dma_continuous(_dev); // continuous DMA buffer
    }

    void setDMA(void * Buf, uint16 BufLen, uint8 dual, uint8 cont, uint32 dmaFlags, voidFuncPtr func);

    uint8_t getDMAHalfFlag() { return dma_get_isr_bit(DMA2, _dev->dmaStream, DMA_ISR_HTIF); }
    uint8_t getDMACompleteFlag() { return dma_get_isr_bit(DMA2, _dev->dmaStream, DMA_ISR_TCIF); }

    void clearDMAIsrBits() { dma_clear_isr_bits(DMA2, _dev->dmaStream); }

/*
    This will read the Vcc and return something useful.
    Polling is being used.
*/
    float readVref();

/*
    This will read the Temperature and return something useful.
    Polling is being used.
*/
    float readTemp();

/*
    This function will set the number of channels to convert
    And which channels.
    For pin numbers, see setPins below
*/
    void setChannels(uint8 * channels, uint8 length)
    {
        adc_set_reg_sequence(_dev, channels, length);
    }

/*
    This function will specify the pins to convert.
*/
    void setPins(const uint8 * pins, uint8 length);

/*
    This function will set the trigger to start the conversion.

    ADC_EXTSEL_TIM1_CC1 
    ADC_EXTSEL_TIM1_CC2 
    ADC_EXTSEL_TIM1_CC3 
    ADC_EXTSEL_TIM2_CC2 
    ADC_EXTSEL_TIM2_CC3 
    ADC_EXTSEL_TIM2_CC4 
    ADC_EXTSEL_TIM2_TRGO
    ADC_EXTSEL_TIM3_CC1 
    ADC_EXTSEL_TIM3_TRGO
    ADC_EXTSEL_TIM4_CC4 
    ADC_EXTSEL_TIM5_CC1 
    ADC_EXTSEL_TIM5_CC2 
    ADC_EXTSEL_TIM5_CC3 
    ADC_EXTSEL_TIM8_CC1 
    ADC_EXTSEL_TIM8_TRGO
    ADC_EXTSEL_EXTI11   
*/
    void setTrigger(adc_extsel_event trigger) 
    {
        adc_set_extsel(_dev, trigger);
        adc_set_exttrig(_dev, ADC_EXT_TRIGGER_ON_RISING_EDGE);
    }

/*
    this function will set the continuous conversion bit.
*/
    void setContinuous() { adc_set_continuous(_dev); }

/*
    this function will reset the continuous bit.
*/
    void resetContinuous() { adc_clear_continuous(_dev); }

/*
    This will be used to start conversions
*/
    void startConversion() { adc_start_convert(_dev); }

/*
    This will set the Scan Mode on.
    This will use DMA.
*/
    void setScanMode() { adc_set_scan_mode(_dev); }

/*
    This will set an Analog Watchdog on a channel.
    It must be used with a channel that is being converted.
*/
    void setWD(uint8 channel, uint32 highLimit, uint32 lowLimit, voidFuncPtr func);

/*
    check analog watchdog
    Poll the status on the watchdog. This will return and reset the bit.
*/
    uint8 getWDActiveFlag();


private:

    const adc_dev * _dev;
    static constexpr float averageSlope = 2.5; // mV/°C
    static constexpr float v25 = 760.0; // [mV]
};
