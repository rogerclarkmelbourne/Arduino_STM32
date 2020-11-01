#include "utility/util_adc.h"
#include "libmaple/dma.h"


class STM32ADC{

public:

/*
    Constructor
    Choose which ADC to use.
    Start it up...
*/
	STM32ADC (adc_dev * dev) {
		_dev = dev;
	}

/*
    Set the ADC Sampling Rate.
    ADC_SMPR_1_5,               < 1.5 ADC cycles
    ADC_SMPR_7_5,               < 7.5 ADC cycles
    ADC_SMPR_13_5,              < 13.5 ADC cycles
    ADC_SMPR_28_5,              < 28.5 ADC cycles
    ADC_SMPR_41_5,              < 41.5 ADC cycles
    ADC_SMPR_55_5,              < 55.5 ADC cycles
    ADC_SMPR_71_5,              < 71.5 ADC cycles
    ADC_SMPR_239_5,             < 239.5 ADC cycles
*/
	void setSampleRate(adc_smp_rate SampleRate) {
		adc_set_sample_rate(_dev, SampleRate);
	}

/*
    Attach an interrupt to the ADC completion.
*/
	void attachInterrupt(voidFuncPtr func) {
		adc_attach_interrupt(_dev, ADC_EOC, func);
	}

	void calibrate()  {
		adc_calibrate(_dev);
	}

/*
    This function is used to setup DMA with the ADC. 
    It will be independent of the mode used. It will either be used in continuous or scan mode
    or even both... go figure. :)  
    
    The reason why this is a uint16 is that I am not ready for dual mode. 
*/
	void setDMA(uint16 * Buf, uint32 dmaFlags, voidFuncPtr func);
	void startDMA(uint16 BufLen);

	void setDMA(uint16 * Buf, uint16 BufLen, uint32 dmaFlags, voidFuncPtr func) {
		setDMA(Buf, dmaFlags, func);
		startDMA(BufLen);
	}
	
/*
    This function is used to setup DMA with the ADC. 
    It will be independent of the mode used. It will either be used in continuous or scan mode
    or even both... 
    This function is to be used with Dual ADC (the difference is to use 32bit buffers).
*/
    void setDualDMA(uint32 * Buf, uint16 BufLen, uint32 Flags);

/*
    This will enable the internal readings. Vcc and Temperature
*/
	void enableInternalReading() {
		enable_internal_reading(_dev);
	}

/*
    This will read the Vcc and return something useful.
    Polling is being used.
*/
    float readVcc();

/*
    This will read the Temperature and return something useful.
    Polling is being used.
*/
    float readTemp();

/*
    This function will set the number of channels to convert
    And which channels.
    This is the ADC channels and not the Maple Pins!!! Important!!
	However, PA0 to 7 correspond to ADC channels 0 to 7.
    Also, this will allow you to sample the AD and Vref channels.
*/
	void setChannels(uint8 *pins, uint8 length) {
		adc_set_reg_seq_channel(_dev, pins, length);
	}

/*
    This function will set the number of pins to convert
    And which pins.
*/
    void setPins(uint8 *pins, uint8 length);

/*
    This function will set the trigger to start the conversion
    Timer, SWStart, etc...
    Possible triggers:

    ADC_EXT_EV_TIM1_CC1
    ADC_EXT_EV_TIM1_CC2
    ADC_EXT_EV_TIM2_CC2
    ADC_EXT_EV_TIM3_TRGO
    ADC_EXT_EV_TIM4_CC4
    ADC_EXT_EV_EXTI11
    ADC_EXT_EV_TIM1_CC3
    ADC_EXT_EV_SWSTART
    ADC_EXT_EV_TIM3_CC1
    ADC_EXT_EV_TIM2_CC3
    ADC_EXT_EV_TIM8_CC1
    ADC_EXT_EV_ADC3_TIM8_TRGO
    ADC_EXT_EV_TIM5_CC1
    ADC_EXT_EV_ADC12_TIM8_TRGO
    ADC_EXT_EV_TIM5_CC3
 */
	void setTrigger(adc_extsel_event trigger) {
		adc_set_extsel(_dev, trigger);
	}

/*
    this function will set the continuous conversion bit.
*/
	void setContinuous() {
		_dev->regs->CR2 |= ADC_CR2_CONT;
	}

/*
    this function will reset the continuous bit.
*/
	void resetContinuous() {
		_dev->regs->CR2 &= ~ADC_CR2_CONT;
	}

/*
    This will be used to start conversions
*/
	void startConversion() {
		_dev->regs->CR2 |= ADC_CR2_SWSTART;
	}

/*
    This will set the Scan Mode on.
    This will use DMA.
*/
	void setScanMode() {
		_dev->regs->CR1 |= ADC_CR1_SCAN;
	}

/*
    This will set the Scan Mode on.
    This will use DMA.
*/
    void attachDMAInterrupt(voidFuncPtr func) { dma_attach_interrupt(DMA1, DMA_CH1, func); };

/*
    This will set an Analog Watchdog on a channel.
    It must be used with a channel that is being converted.
	Set bit 7 of channel parameter to use all channels for AWD
*/
    void setAnalogWatchdog(uint8 channel, uint32 HighLimit, uint32 LowLimit);

/*
    check analog watchdog
    Poll the status on the watchdog. This will return and reset the bit.
*/
    uint8 getAnalogWatchdog();

/*
    Attach an interrupt to the Watchdog...
    This can possibly be set together in one function and determine which peripheral
    it relates to.
*/
	void attachAnalogWatchdogInterrupt(voidFuncPtr func) {
		adc_attach_interrupt(_dev, ADC_AWD, func);
	}
	void enableAnalogWatchdog(void) { enable_awd(_dev); }
	void disableAnalogWatchdog(void) { disable_awd(_dev); }

/*
    Retrieve the contents of the DR register. 
*/
	uint32 getData()  {
		return _dev->regs->DR;
	}

private:

    adc_dev * _dev;
    static constexpr float _AverageSlope = 4.3; // mV/oC   //4.0 to 4.6
    static constexpr float _V25 = 1.43; //Volts //1.34 - 1.52

};
