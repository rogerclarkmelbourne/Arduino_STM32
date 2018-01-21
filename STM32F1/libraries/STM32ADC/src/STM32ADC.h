#include "utility/util_adc.h"
#include "libmaple/dma.h"


class STM32ADC{

public:

/*
    Constructor
    Choose which ADC to use.
    Start it up...
*/
    STM32ADC (adc_dev * dev);

/*
    Set the ADC Sampling Rate.
*/
    void setSampleRate(adc_smp_rate SampleRate);

/*
    Attach an interrupt to the ADC completion.
*/
    void attachInterrupt(voidFuncPtr func, uint8 interrupt);

    void calibrate();

/*
    This function is used to setup DMA with the ADC. 
    It will be independent of the mode used. It will either be used in continuous or scan mode
    or even both... go figure. :)  
    
    The reason why this is a uint16 is that I am not ready for dual mode. 
*/
    void setDMA(uint16 * Buf, uint16 BufLen, uint32 dmaFlags, voidFuncPtr func);

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
    void enableInternalReading();

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
    For pin numbers, see setPins below
*/
    void setChannels(uint8 *pins, uint8 length);

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
    void setTrigger(adc_extsel_event trigger);

/*
    this function will set the continuous conversion bit.
*/
    void setContinuous();

/*
    this function will reset the continuous bit.
*/
    void resetContinuous();

/*
    This will be used to start conversions
*/
    void startConversion();

/*
    This will set the Scan Mode on.
    This will use DMA.
*/
    void setScanMode();

/*
    This will set the Scan Mode on.
    This will use DMA.
*/
    void attachDMAInterrupt(voidFuncPtr func);

/*
    This will set an Analog Watchdog on a channel.
    It must be used with a channel that is being converted.
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
    void attachAnalogWatchdogInterrupt(voidFuncPtr func);

/*
    Retrieve the contents of the DR register. 
*/
    uint32 getData();

private:

    adc_dev * _dev;
    voidFuncPtr _DMA_int;
    voidFuncPtr _ADC_int;
    voidFuncPtr _AWD_int;
    static constexpr float _AverageSlope = 4.3; // mV/oC   //4.0 to 4.6
    static constexpr float _V25 = 1.43; //Volts //1.34 - 1.52

};
