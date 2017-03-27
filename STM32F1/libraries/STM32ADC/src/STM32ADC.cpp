#include "STM32ADC.h"
#include "boards.h"


/*
    Constructor
    Choose which ADC to use.
    Start it up...
*/
    STM32ADC::STM32ADC (adc_dev * dev){
        _dev = dev;
        //adc_calibrate(_dev);//get this out of the way. 
 
    }

    uint32 STM32ADC::getData() {
        return _dev->regs->DR;
    };


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
    void STM32ADC::setSampleRate(adc_smp_rate SampleRate){
        adc_set_sample_rate(_dev, SampleRate);
    }

/*
    Attach an interrupt to the ADC completion.
*/
    void STM32ADC::attachInterrupt(voidFuncPtr func, uint8 interrupt){
        adc_attach_interrupt(_dev,interrupt, func);
    }

/*
    This will enable the internal readings. Vcc and Temperature
*/
    void STM32ADC::enableInternalReading(){
        enable_internal_reading(_dev);
    }

/*
    This will read the Vcc and return something useful.
    Polling is being used.
*/
    float STM32ADC::readVcc(){
        unsigned int result = 0;
        float vcc = 0.0;
        result = adc_read(_dev, 17);

        vcc = (float)result * 1.1; //to be done later...
        return vcc;
    }

/*
    This will read the Temperature and return something useful.
    Polling is being used.
*/
    float STM32ADC::readTemp(){
        unsigned int result = 0;
        float temperature = 0.0;
        result = adc_read(_dev, 16);
        temperature = (float)((_V25-result)/_AverageSlope)+ 25.0; 
        return temperature;
    }

/*
    This function will set the number of Pins to sample and which PINS to convert. 
    This uses the Maple Pin numbers and not the ADC channel numbers. Do not confuse. 
*/
    void STM32ADC::setPins(uint8 *pins, uint8 length){
        //convert pins to channels.
        uint8 channels[length];
        unsigned int records[3] = {0,0,0};
        unsigned char i = 0, j = 0;
        
        for (unsigned char i = 0; i < length; i++) { //convert the channels from pins to ch.
            channels[i] = PIN_MAP[pins[i]].adc_channel;
        }

        //run away protection
        if (length > 16) length = 16;

        //write the length
        records[2] |= (length - 1) << 20;

        //i goes through records, j goes through variables.
        for (i = 0, j = 0; i < length; i++) {//go through the channel list.
            if (i!=0 && i%6 == 0) j++;//next variable, please!!
            records[j] |= (channels[i] << ((i%6)*5));
        }
        //update the registers inside with the scan sequence.
        _dev->regs->SQR1 = records[2];
        _dev->regs->SQR2 = records[1];
        _dev->regs->SQR3 = records[0];
    }

/*
    This function will set the number of channels to convert
    And which channels.
    This is the ADC channels and not the Maple Pins!!! Important!!
    Also, this will allow you to sample the AD and Vref channels.
*/
    void STM32ADC::setChannels(uint8 *channels, uint8 length){
        adc_set_reg_seq_channel(_dev, channels, length);
    }

/*
    This function will set the trigger to start the conversion
    Timer, SWStart, etc...
*/
    void STM32ADC::setTrigger(adc_extsel_event trigger){
      adc_set_extsel(_dev, trigger);
    }

/*
    this function will set the continuous conversion bit.
*/
    void STM32ADC::setContinuous(){
        _dev->regs->CR2 |= ADC_CR2_CONT;
    };

/*
    this function will reset the continuous bit.
*/
    void STM32ADC::resetContinuous(){
        _dev->regs->CR2 &= ~ADC_CR2_CONT;
    };

/*
    This will be used to start conversions
*/
    void STM32ADC::startConversion(){
        _dev->regs->CR2 |= ADC_CR2_SWSTART;
    }

/*
    This will set the Scan Mode on.
    This will use DMA.
*/
    void STM32ADC::setScanMode(){
        _dev->regs->CR1 |= ADC_CR1_SCAN;
    }

    void STM32ADC::calibrate() {
        adc_calibrate(_dev);
    }

/*
    This function is used to setup DMA with the ADC. 
    It will be independent of the mode used. It will either be used in continuous or scan mode
    or even both... go figure. :)  
    
    The reason why this is a uint16 is that I am not ready for dual mode. 
*/

    void STM32ADC::setDMA(uint16 * Buf, uint16 BufLen, uint32 dmaFlags, voidFuncPtr func) {
//initialize DMA
        dma_init(DMA1);
//if there is an int handler to be called... 
        if (func != NULL)
            dma_attach_interrupt(DMA1, DMA_CH1, func);
//enable ADC DMA transfer
        //adc_dma_enable(ADC1);
        _dev->regs->CR2 |= ADC_CR2_DMA;
//set it up... 
        dma_setup_transfer(DMA1, DMA_CH1, &ADC1->regs->DR, DMA_SIZE_16BITS, Buf, DMA_SIZE_16BITS, dmaFlags);// Receive buffer DMA
//how many are we making?? 
        dma_set_num_transfers(DMA1, DMA_CH1, BufLen);
//enable dma.
        dma_enable(DMA1, DMA_CH1); // Enable the channel and start the transfer.
    }

/*
    This function is used to setup DMA with the ADC. 
    It will be independent of the mode used. It will either be used in continuous or scan mode
    or even both... 
    This function is to be used with Dual ADC (the difference is to use 32bit buffers).
*/
    void STM32ADC::setDualDMA(uint32 * Buf, uint16 BufLen, uint32 Flags){
        dma_init(DMA1);
        adc_dma_enable(_dev);
        dma_setup_transfer(DMA1, DMA_CH1, &_dev->regs->DR, DMA_SIZE_32BITS,//(DMA_MINC_MODE | DMA_CIRC_MODE)
                     Buf, DMA_SIZE_32BITS, Flags);// Receive buffer DMA
        dma_set_num_transfers(DMA1, DMA_CH1, BufLen);
        dma_enable(DMA1, DMA_CH1); // Enable the channel and start the transfer.
    }

/*
    This will set the Scan Mode on.
    This will use DMA.
*/
    void STM32ADC::attachDMAInterrupt(voidFuncPtr func){
        _DMA_int = func;
        dma_attach_interrupt(DMA1, DMA_CH1, func);
    }

/*
    This will set an Analog Watchdog on a channel.
    It must be used with a channel that is being converted.
*/
    void STM32ADC::setAnalogWatchdog(uint8 channel, uint32 HighLimit, uint32 LowLimit){
        set_awd_low_limit(_dev, LowLimit);
        set_awd_high_limit(_dev, HighLimit);
        set_awd_channel(_dev, channel);
    }

/*
    check analog watchdog
    Poll the status on the watchdog. This will return and reset the bit.
*/
    uint8 STM32ADC::getAnalogWatchdog(){
        return 1;
    }

/*
    Attach an interrupt to the Watchdog...
    This can possibly be set together in one function and determine which peripheral
    it relates to.
*/
    void STM32ADC::attachAnalogWatchdogInterrupt(voidFuncPtr func){
        _AWD_int = func;

    }
