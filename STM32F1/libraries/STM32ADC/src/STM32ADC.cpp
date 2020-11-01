#include "STM32ADC.h"
#include "boards.h"


/*
    This will read the Vcc and return something useful.
    Polling is being used.
*/
    float STM32ADC::readVcc() {
        uint16_t result = adc_read(_dev, 17);
        float vcc = (float)result * 1.1; //to be done later...
        return vcc;
    }

/*
    This will read the Temperature and return something useful.
    Polling is being used.
*/
    float STM32ADC::readTemp() {
        uint16_t result = adc_read(_dev, 16);
        float Vsense = (3300.0*result)/4096;
        float temperature = ((_V25-Vsense)/_AverageSlope) + 25.0; 
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
        
        for (uint8_t i = 0; i < length; i++) { //convert the channels from pins to ch.
            channels[i] = PIN_MAP[pins[i]].adc_channel;
        }

        //run away protection
        if (length > 16) length = 16;

        //write the length
        records[2] |= (length - 1) << 20;

        //i goes through records, j goes through variables.
        for (uint8_t i = 0, j = 0; i < length; i++) {//go through the channel list.
            if (i!=0 && i%6 == 0) j++;//next variable, please!!
            records[j] |= (channels[i] << ((i%6)*5));
        }
        //update the registers inside with the scan sequence.
        _dev->regs->SQR1 = records[2];
        _dev->regs->SQR2 = records[1];
        _dev->regs->SQR3 = records[0];
    }

/*
    This function is used to setup DMA with the ADC. 
    It will be independent of the mode used. It will either be used in continuous or scan mode
    or even both... go figure. :)  
    
    The reason why this is a uint16 is that I am not ready for dual mode. 
*/

    void STM32ADC::setDMA(uint16 * Buf, uint32 dmaFlags, voidFuncPtr func)
	{
//initialize DMA
        dma_init(DMA1);
        dma_disable(DMA1, DMA_CH1);
//if there is an int handler to be called... 
        if (func != NULL)
            dma_attach_interrupt(DMA1, DMA_CH1, func);
//enable ADC DMA transfer
        adc_dma_enable(ADC1);
        //_dev->regs->CR2 |= ADC_CR2_DMA;
//set it up... 
        dma_setup_transfer(DMA1, DMA_CH1, &ADC1->regs->DR, DMA_SIZE_16BITS, Buf, DMA_SIZE_16BITS, dmaFlags);// Receive buffer DMA
    }

    void STM32ADC::startDMA(uint16 BufLen)
	{
        dma_disable(DMA1, DMA_CH1);
        dma_set_num_transfers(DMA1, DMA_CH1, BufLen);
        dma_enable(DMA1, DMA_CH1); // Enable the channel
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
    This will set an Analog Watchdog on a channel.
    It must be used with a channel that is being converted.
*/
	void STM32ADC::setAnalogWatchdog(uint8 channel, uint32 HighLimit, uint32 LowLimit)
	{
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
