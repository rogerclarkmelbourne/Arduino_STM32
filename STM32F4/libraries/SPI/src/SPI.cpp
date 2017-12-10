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
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief Wirish SPI implementation.
 */

#include "SPI.h"

//#define SPI_DEBUG

#include <libmaple/timer.h>
#include <libmaple/util.h>
#include <libmaple/rcc.h>

#include "wirish.h"
#include "boards.h"

#define DMA_TIMEOUT 100

#if CYCLES_PER_MICROSECOND != 168
/* TODO [0.2.0?] something smarter than this */
#warning "Unexpected clock speed; SPI frequency calculation will be incorrect"
#endif

struct spi_pins {
    uint8 nss;
    uint8 sck;
    uint8 miso;
    uint8 mosi;
};

static const spi_pins* dev_to_spi_pins(spi_dev *dev);

static void configure_gpios(spi_dev *dev, bool as_master);

static spi_baud_rate determine_baud_rate(spi_dev *dev, uint32_t freq);

#if (BOARD_NR_SPI >= 3) && !defined(STM32_HIGH_DENSITY)
#error "The SPI library is misconfigured: 3 SPI ports only available on high density STM32 devices"
#endif

static const spi_pins board_spi_pins[] __FLASH__ = {
#if BOARD_NR_SPI >= 1
    {BOARD_SPI1_NSS_PIN,
     BOARD_SPI1_SCK_PIN,
     BOARD_SPI1_MISO_PIN,
     BOARD_SPI1_MOSI_PIN},
#endif
#if BOARD_NR_SPI >= 2
    {BOARD_SPI2_NSS_PIN,
     BOARD_SPI2_SCK_PIN,
     BOARD_SPI2_MISO_PIN,
     BOARD_SPI2_MOSI_PIN},
#endif
#if BOARD_NR_SPI >= 3
    {BOARD_SPI3_NSS_PIN,
     BOARD_SPI3_SCK_PIN,
     BOARD_SPI3_MISO_PIN,
     BOARD_SPI3_MOSI_PIN},
#endif
};


/*
 * Constructor
 */

SPIClass::SPIClass(uint32 spi_num) {

	_currentSetting=&_settings[spi_num-1];// SPI channels are called 1 2 and 3 but the array is zero indexed

	
    switch (spi_num) {
#if BOARD_NR_SPI >= 1
    case 1:
        _currentSetting->spi_d = SPI1;
        break;
#endif
#if BOARD_NR_SPI >= 2
    case 2:
        _currentSetting->spi_d = SPI2;
        break;
#endif
#if BOARD_NR_SPI >= 3
    case 3:
        _currentSetting->spi_d = SPI3;
        break;
#endif
    default:
        ASSERT(0);
    }
	
	// Init things specific to each SPI device
	// clock divider setup is a bit of hack, and needs to be improved at a later date.
/*****************************************************************************/
//           DMA / Channel / Stream 
//            Rx             Tx
// SPI1:  2 / 3 / 0 (2) - 2 / 3 / 3 (5)
// SPI2:  1 / 0 / 3     - 1 / 0 / 4
// SPI3:  1 / 0 / 0 (2) - 1 / 0 / 5 (7)
/*****************************************************************************/
	_settings[0].spi_d = SPI1;
	_settings[0].clockDivider = determine_baud_rate(_settings[0].spi_d, _settings[0].clock);
#ifdef SPI_DMA
	_settings[0].spiDmaDev = DMA2;
	_settings[0].spiDmaChannel = DMA_CH3;
	_settings[0].spiRxDmaStream  = DMA_STREAM0; // alternative: DMA_STREAM2
	_settings[0].spiTxDmaStream  = DMA_STREAM3; // alternative: DMA_STREAM5
#endif
	_settings[1].spi_d = SPI2;
	_settings[1].clockDivider = determine_baud_rate(_settings[1].spi_d, _settings[1].clock);
#ifdef SPI_DMA
	_settings[1].spiDmaDev = DMA1;
	_settings[1].spiDmaChannel = DMA_CH0;
	_settings[1].spiRxDmaStream  = DMA_STREAM3; // alternative: -
	_settings[1].spiTxDmaStream  = DMA_STREAM4; // alternative: -
#endif
#if BOARD_NR_SPI >= 3
	_settings[2].spi_d = SPI3;
	_settings[2].clockDivider = determine_baud_rate(_settings[2].spi_d, _settings[2].clock);
#ifdef SPI_DMA
	_settings[2].spiDmaDev = DMA1;
	_settings[2].spiDmaChannel = DMA_CH0;
	_settings[2].spiRxDmaStream  = DMA_STREAM0; // alternative: DMA_STREAM2
	_settings[2].spiTxDmaStream  = DMA_STREAM5; // alternative: DMA_STREAM7
#endif
#endif

}

/*
 * Set up/tear down
 */
void SPIClass::updateSettings(void) {
	uint32 flags = ((_currentSetting->bitOrder == MSBFIRST ? SPI_FRAME_MSB : SPI_FRAME_LSB) | _currentSetting->dataSize | SPI_SW_SLAVE | SPI_SOFT_SS);
	#ifdef SPI_DEBUG
	Serial.print("spi_master_enable("); Serial.print(_currentSetting->clockDivider); Serial.print(","); Serial.print(_currentSetting->dataMode); Serial.print(","); Serial.print(flags); Serial.println(")");
	#endif
	spi_master_enable(_currentSetting->spi_d, (spi_baud_rate)_currentSetting->clockDivider, (spi_mode)_currentSetting->dataMode, flags);
}

void SPIClass::begin(void) {
    spi_init(_currentSetting->spi_d);
    configure_gpios(_currentSetting->spi_d, 1);
    updateSettings();
#ifdef SPI_USE_DMA_BUFFER
	dmaSendBufferInit();
#endif
//Serial.println("SPI class begin - end");
}

void SPIClass::beginSlave(void) {
    spi_init(_currentSetting->spi_d);
    configure_gpios(_currentSetting->spi_d, 0);
    uint32 flags = ((_currentSetting->bitOrder == MSBFIRST ? SPI_FRAME_MSB : SPI_FRAME_LSB) | _currentSetting->dataSize | SPI_RX_ONLY);
	#ifdef SPI_DEBUG
	Serial.print("spi_slave_enable("); Serial.print(_currentSetting->dataMode); Serial.print(","); Serial.print(flags); Serial.println(")");
	#endif
    spi_slave_enable(_currentSetting->spi_d, (spi_mode)_currentSetting->dataMode, flags);
}

void SPIClass::end(void) {
    if (!spi_is_enabled(_currentSetting->spi_d)) {
        return;
    }

    // Follows RM0008's sequence for disabling a SPI in master/slave
    // full duplex mode.
    while (spi_is_rx_nonempty(_currentSetting->spi_d)) {
        // FIXME [0.1.0] remove this once you have an interrupt based driver
        volatile uint16 rx __attribute__((unused)) = spi_rx_reg(_currentSetting->spi_d);
    }
    while (!spi_is_tx_empty(_currentSetting->spi_d))
        ;
    while (spi_is_busy(_currentSetting->spi_d))
        ;
    spi_peripheral_disable(_currentSetting->spi_d);
}

/* Roger Clark added  3 functions */
void SPIClass::setClockDivider(uint32_t clockDivider)
{
	#ifdef SPI_DEBUG
	Serial.print("Clock divider set to "); Serial.println(clockDivider);
	#endif
	_currentSetting->clockDivider = clockDivider;
	uint32 cr1 = _currentSetting->spi_d->regs->CR1 & ~(SPI_CR1_BR);
	_currentSetting->spi_d->regs->CR1 = cr1 | (clockDivider & SPI_CR1_BR);
}

void SPIClass::setBitOrder(BitOrder bitOrder)
{
	#ifdef SPI_DEBUG
	Serial.print("Bit order set to "); Serial.println(bitOrder);
	#endif
	_currentSetting->bitOrder = bitOrder;
	uint32 cr1 = _currentSetting->spi_d->regs->CR1 & ~(SPI_CR1_LSBFIRST);
	if ( bitOrder==LSBFIRST )	cr1 |= SPI_CR1_LSBFIRST;
	_currentSetting->spi_d->regs->CR1 = cr1;
}

/*	Victor Perez. Added to test changing datasize from 8 to 16 bit modes on the fly.
*	Input parameter should be SPI_CR1_DFF set to 0 or 1 on a 32bit word.
*	
*/
void SPIClass::setDataSize(uint32 datasize)
{
	_currentSetting->dataSize = datasize;
	uint32 cr1 = _currentSetting->spi_d->regs->CR1 & ~(SPI_CR1_DFF);
	uint8 en = spi_is_enabled(_currentSetting->spi_d);
	spi_peripheral_disable(_currentSetting->spi_d);
	_currentSetting->spi_d->regs->CR1 = cr1 | (datasize & SPI_CR1_DFF) | en;
}

void SPIClass::setDataMode(uint8_t dataMode)
{
/* Notes.  As far as I can tell, the AVR numbers for dataMode appear to match the numbers required by the STM32

From the AVR doc http://www.atmel.com/images/doc2585.pdf section 2.4

SPI Mode 	CPOL 	CPHA 	Shift SCK-edge 	Capture SCK-edge
0 			0 		0 		Falling 		Rising
1 			0 		1 		Rising 			Falling
2 			1 		0 		Rising 			Falling
3 			1 		1 		Falling 		Rising
 
 
On the STM32 it appears to be

bit 1 - CPOL : Clock polarity
    (This bit should not be changed when communication is ongoing)
    0 : CLK to 0 when idle
    1 : CLK to 1 when idle
 
bit 0 - CPHA : Clock phase
    (This bit should not be changed when communication is ongoing)
    0 : The first clock transition is the first data capture edge
    1 : The second clock transition is the first data capture edge
 
If someone finds this is not the case or sees a logic error with this let me know ;-) 
 */
	#ifdef SPI_DEBUG
	Serial.print("Data mode set to "); Serial.println(dataMode);
	#endif
	_currentSetting->dataMode = dataMode;
	uint32 cr1 = _currentSetting->spi_d->regs->CR1 & ~(SPI_CR1_CPOL|SPI_CR1_CPHA);
	_currentSetting->spi_d->regs->CR1 = cr1 | (dataMode & (SPI_CR1_CPOL|SPI_CR1_CPHA));
}

void SPIClass::beginTransaction(uint8_t pin, SPISettings settings)
{
	#ifdef SPI_DEBUG
	Serial.println("SPIClass::beginTransaction");
	#endif
	setBitOrder(settings.bitOrder);
	setDataMode(settings.dataMode);
	setDataSize(settings.dataSize);
	setClockDivider(determine_baud_rate(_currentSetting->spi_d, settings.clock));
	begin();
}

void SPIClass::beginTransactionSlave(SPISettings settings)
{
	#ifdef SPI_DEBUG
	Serial.println(F("SPIClass::beginTransactionSlave"));
	#endif
	setBitOrder(settings.bitOrder);
	setDataMode(settings.dataMode);
	setDataSize(settings.dataSize);
	beginSlave();
}

void SPIClass::endTransaction(void)
{
	#ifdef SPI_DEBUG
	Serial.println("SPIClass::endTransaction");
	#endif
	//digitalWrite(_SSPin,HIGH);
#if false
// code from SAM core
	uint8_t mode = interruptMode;
	if (mode > 0) {
		if (mode < 16) {
			if (mode & 1) PIOA->PIO_IER = interruptMask[0];
			if (mode & 2) PIOB->PIO_IER = interruptMask[1];
			if (mode & 4) PIOC->PIO_IER = interruptMask[2];
			if (mode & 8) PIOD->PIO_IER = interruptMask[3];
		} else {
			if (interruptSave) interrupts();
		}
	}
#endif
}


/*
 * I/O
 */

uint16 SPIClass::read(void)
{
	while ( spi_is_rx_nonempty(_currentSetting->spi_d)==0 ) ;
	return (uint16)spi_rx_reg(_currentSetting->spi_d);
}

void SPIClass::read(uint8 *buf, uint32 len)
{
	if ( len == 0 ) return;
	spi_rx_reg(_currentSetting->spi_d);		// clear the RX buffer in case a byte is waiting on it.
	spi_reg_map * regs = _currentSetting->spi_d->regs;
	// start sequence: write byte 0
	regs->DR = 0x00FF;						// write the first byte
	// main loop
	while ( (--len) ) {
		while( !(regs->SR & SPI_SR_TXE) );		// wait for TXE flag
		noInterrupts();							// go atomic level - avoid interrupts to surely get the previously received data
		regs->DR = 0x00FF;						// write the next data item to be transmitted into the SPI_DR register. This clears the TXE flag.
		while ( !(regs->SR & SPI_SR_RXNE) );	// wait till data is available in the DR register
		*buf++ = (uint8)(regs->DR);				// read and store the received byte. This clears the RXNE flag.
		interrupts();							// let systick do its job
	}
	// read remaining last byte
	while ( !(regs->SR & SPI_SR_RXNE) );	// wait till data is available in the Rx register
	*buf++ = (uint8)(regs->DR);				// read and store the received byte
}

void SPIClass::write(uint16 data)
{
	/* Added for 16bit data Victor Perez. Roger Clark 
	 * Improved speed by just directly writing the single byte to the SPI data reg and wait for completion,
	 * by taking the Tx code from transfer(byte)
	 * This almost doubles the speed of this function.
	 */
	spi_tx_reg(_currentSetting->spi_d, data); // write the data to be transmitted into the SPI_DR register (this clears the TXE flag)
	while (spi_is_tx_empty(_currentSetting->spi_d) == 0); // "5. Wait until TXE=1 ..."
	while (spi_is_busy(_currentSetting->spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI." 
}

void SPIClass::write(uint16 data, uint32 n)
{
	// Added by stevstrong: Repeatedly send same data by the specified number of times
	spi_reg_map * regs = _currentSetting->spi_d->regs;
	while ( (n--)>0 ) {
		regs->DR = data; // write the data to be transmitted into the SPI_DR register (this clears the TXE flag)
		while ( (regs->SR & SPI_SR_TXE)==0 ) ; // wait till Tx empty
	}
	while ( (regs->SR & SPI_SR_BSY) != 0); // wait until BSY=0 before returning 
}

void SPIClass::write(const void *data, uint32 length)
{
	spi_dev * spi_d = _currentSetting->spi_d;
	spi_tx(spi_d, (void*)data, length); // data can be array of bytes or words
	while (spi_is_tx_empty(spi_d) == 0); // "5. Wait until TXE=1 ..."
	while (spi_is_busy(spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI."
}

uint8 SPIClass::transfer(uint8 byte) const
{
	spi_dev * spi_d = _currentSetting->spi_d;
	spi_rx_reg(spi_d); // read any previous data
	spi_tx_reg(spi_d, byte); // Write the data item to be transmitted into the SPI_DR register
	while (spi_is_tx_empty(spi_d) == 0); // "5. Wait until TXE=1 ..."
	while (spi_is_busy(spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI."
	return (uint8)spi_rx_reg(spi_d); // "... and read the last received data."
}

uint16_t SPIClass::transfer16(uint16_t wr_data) const
{
	spi_dev * spi_d = _currentSetting->spi_d;
	spi_rx_reg(spi_d); // read any previous data
	spi_tx_reg(spi_d, wr_data); // "2. Write the first data item to be transmitted into the SPI_DR register (this clears the TXE flag)."
	while (spi_is_tx_empty(spi_d) == 0); // "5. Wait until TXE=1 ..."
	while (spi_is_busy(spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI."
	return (uint16)spi_rx_reg(spi_d); // "... and read the last received data."
}

#ifdef SPI_DMA
/*  Roger Clark and Victor Perez, 2015
*	Performs a DMA SPI transfer with at least a receive buffer.
*	If a TX buffer is not provided, FF is sent over and over for the length of the transfer. 
*	On exit TX buffer is not modified, and RX buffer contains the received data.
*	Still in progress.
*/
uint8 SPIClass::dmaTransfer(const void * transmitBuf, void * receiveBuf, uint16 length)
{
	if (length == 0) return 0;

	uint8 b = 0;

    dma_init(_currentSetting->spiDmaDev);
//	dma_attach_interrupt(DMA1, DMA_CH3, &SPIClass::DMA1_CH3_Event);
	// RX
	dma_xfer_size dma_bit_size = (_currentSetting->dataSize==SPI_DATA_SIZE_16BIT) ? DMA_SIZE_16BITS : DMA_SIZE_8BITS;
	dma_setup_transfer(	_currentSetting->spiDmaDev,
						_currentSetting->spiRxDmaStream,
						_currentSetting->spiDmaChannel,
						dma_bit_size,
						&_currentSetting->spi_d->regs->DR,	// peripheral address
						receiveBuf,							// memory bank 0 address
						NULL,								// memory bank 1 address
						(DMA_MINC_MODE | DMA_FROM_PER | DMA_PRIO_VERY_HIGH)		// flags
	);
	dma_set_num_transfers(_currentSetting->spiDmaDev, _currentSetting->spiRxDmaStream, length);
	dma_set_fifo_flags(_currentSetting->spiDmaDev, _currentSetting->spiRxDmaStream, 0);
	dma_clear_isr_bits(_currentSetting->spiDmaDev, _currentSetting->spiRxDmaStream);

	// TX
	uint32 flags = (DMA_MINC_MODE | DMA_FROM_MEM); // | DMA_TRNS_CMPLT);
	if ( transmitBuf==0 ) {
		static uint8_t ff = 0XFF;
		transmitBuf = &ff;
		flags &= ~((uint32)DMA_MINC_MODE); // remove increment mode
	}
	dma_setup_transfer(	_currentSetting->spiDmaDev,
						_currentSetting->spiTxDmaStream,
						_currentSetting->spiDmaChannel,
						dma_bit_size,
						&_currentSetting->spi_d->regs->DR,	// peripheral address
						(volatile void*)transmitBuf,						// memory bank 0 address
						NULL,								// memory bank 1 address
						flags
	);
	dma_set_num_transfers(_currentSetting->spiDmaDev, _currentSetting->spiTxDmaStream, length);
	dma_set_fifo_flags(_currentSetting->spiDmaDev, _currentSetting->spiTxDmaStream, 0);
	dma_clear_isr_bits(_currentSetting->spiDmaDev, _currentSetting->spiTxDmaStream);
	// software enable sequence, see AN4031, chapter 4.3
	dma_enable(_currentSetting->spiDmaDev, _currentSetting->spiRxDmaStream);// enable receive
	dma_enable(_currentSetting->spiDmaDev, _currentSetting->spiTxDmaStream);// enable transmit
	spi_rx_reg(_currentSetting->spi_d); //Clear the RX buffer in case a byte is waiting on it.
	spi_rx_dma_enable(_currentSetting->spi_d);
	spi_tx_dma_enable(_currentSetting->spi_d);	// must be the last enable to avoid DMA error flag
	
    uint32_t m = millis();
	while ((dma_get_isr_bits(_currentSetting->spiDmaDev, _currentSetting->spiTxDmaStream) & DMA_ISR_TCIF)==0 ) {// wait for completion flag to be set
		if ((millis() - m) > DMA_TIMEOUT) { b = 2; break; }
    }

	while (spi_is_tx_empty(_currentSetting->spi_d) == 0); // "5. Wait until TXE=1 ..."
	while (spi_is_busy(_currentSetting->spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI." 
	// software disable sequence, see AN4031, chapter 4.1
	spi_tx_dma_disable(_currentSetting->spi_d);
	spi_rx_dma_disable(_currentSetting->spi_d);
	dma_disable(_currentSetting->spiDmaDev, _currentSetting->spiTxDmaStream);
	dma_disable(_currentSetting->spiDmaDev, _currentSetting->spiRxDmaStream);
    return b;
}

/*  Roger Clark and Victor Perez, 2015
*	Performs a DMA SPI send using a TX buffer.
*	On exit TX buffer is not modified.
*	Still in progress.
*	2016 - stevstrong - reworked to automatically detect bit size from SPI setting
*/
uint8 SPIClass::dmaSend(const void * transmitBuf, uint16 length, bool minc)
{
	if (length == 0) return 0;
	uint8 b = 0;
	dma_init(_currentSetting->spiDmaDev);
	// TX
	dma_xfer_size dma_bit_size = (_currentSetting->dataSize==SPI_DATA_SIZE_16BIT) ? DMA_SIZE_16BITS : DMA_SIZE_8BITS;
	dma_setup_transfer(	_currentSetting->spiDmaDev,
						_currentSetting->spiTxDmaStream,
						_currentSetting->spiDmaChannel,
						dma_bit_size,
						&_currentSetting->spi_d->regs->DR,	// peripheral address
						(volatile void*)transmitBuf,						// memory bank 0 address
						NULL,								// memory bank 1 address
						( (DMA_MINC_MODE*minc) | DMA_FROM_MEM ) //| DMA_TRNS_CMPLT ) // flags
	);// Transmit buffer DMA 
	dma_set_num_transfers(_currentSetting->spiDmaDev, _currentSetting->spiTxDmaStream, length);
	dma_set_fifo_flags(_currentSetting->spiDmaDev, _currentSetting->spiTxDmaStream, 0);
	dma_clear_isr_bits(_currentSetting->spiDmaDev, _currentSetting->spiTxDmaStream);
	dma_enable(_currentSetting->spiDmaDev, _currentSetting->spiTxDmaStream);// enable transmit
	spi_tx_dma_enable(_currentSetting->spi_d);

	uint32_t m = millis();
	while ((dma_get_isr_bits(_currentSetting->spiDmaDev, _currentSetting->spiTxDmaStream) & DMA_ISR_TCIF)==0 ) {// wait for completion flag to be set
		if ((millis() - m) > DMA_TIMEOUT) { b = 2; break; }
    }

	while (spi_is_tx_empty(_currentSetting->spi_d) == 0); // "5. Wait until TXE=1 ..."
	while (spi_is_busy(_currentSetting->spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI." 
	spi_tx_dma_disable(_currentSetting->spi_d);
	dma_disable(_currentSetting->spiDmaDev, _currentSetting->spiTxDmaStream);
	return b;
}
#endif



void SPIClass::attachInterrupt(void) {
	// Should be enableInterrupt()
}

void SPIClass::detachInterrupt(void) {
	// Should be disableInterrupt()
}

/*
 * Pin accessors
 */

uint8 SPIClass::misoPin(void) {
    return dev_to_spi_pins(_currentSetting->spi_d)->miso;
}

uint8 SPIClass::mosiPin(void) {
    return dev_to_spi_pins(_currentSetting->spi_d)->mosi;
}

uint8 SPIClass::sckPin(void) {
    return dev_to_spi_pins(_currentSetting->spi_d)->sck;
}

uint8 SPIClass::nssPin(void) {
    return dev_to_spi_pins(_currentSetting->spi_d)->nss;
}

/*
 * Deprecated functions
 */

uint8 SPIClass::send(uint8 data) {
    this->write(data);
    return 1;
}

uint8 SPIClass::send(uint8 *buf, uint32 len) {
    this->write(buf, len);
    return len;
}

uint8 SPIClass::recv(void) {
    return this->read();
}


/*
 * Auxiliary functions
 */

static const spi_pins* dev_to_spi_pins(spi_dev *dev) {
    switch (dev->clk_id) {
#if BOARD_NR_SPI >= 1
    case RCC_SPI1: return board_spi_pins;
#endif
#if BOARD_NR_SPI >= 2
    case RCC_SPI2: return board_spi_pins + 1;
#endif
#if BOARD_NR_SPI >= 3
    case RCC_SPI3: return board_spi_pins + 2;
#endif
    default:       return NULL;
    }
}

static void disable_pwm(const stm32_pin_info *i) {
    if (i->timer_device) {
        timer_set_mode(i->timer_device, i->timer_channel, TIMER_DISABLED);
    }
}

static void configure_gpios(spi_dev *dev, bool as_master) {
    const spi_pins *pins = dev_to_spi_pins(dev);

    if (!pins) {
        return;
    }

    const stm32_pin_info *nssi = &PIN_MAP[pins->nss];
    const stm32_pin_info *scki = &PIN_MAP[pins->sck];
    const stm32_pin_info *misoi = &PIN_MAP[pins->miso];
    const stm32_pin_info *mosii = &PIN_MAP[pins->mosi];

    disable_pwm(nssi);
    disable_pwm(scki);
    disable_pwm(misoi);
    disable_pwm(mosii);

    #ifdef SPI_DEBUG
    Serial.print("SPI configure_gpios / (nss="); 
    Serial.print(pins->nss); Serial.print(", sck="); Serial.print(pins->sck);
    Serial.print(", miso="); Serial.print(pins->miso); 
    Serial.print(", mosi="); Serial.print(pins->mosi);
    Serial.println(")");
    #endif

    spi_config_gpios(dev, as_master, pins->nss, pins->sck, pins->miso, pins->mosi);
}

static const spi_baud_rate baud_rates[8] __FLASH__ = {
    SPI_BAUD_PCLK_DIV_2,
    SPI_BAUD_PCLK_DIV_4,
    SPI_BAUD_PCLK_DIV_8,
    SPI_BAUD_PCLK_DIV_16,
    SPI_BAUD_PCLK_DIV_32,
    SPI_BAUD_PCLK_DIV_64,
    SPI_BAUD_PCLK_DIV_128,
    SPI_BAUD_PCLK_DIV_256,
};

/*
 * Note: This assumes you're on a LeafLabs-style board
 * (CYCLES_PER_MICROSECOND == 72, APB2 at 72MHz, APB1 at 36MHz).
 */
static spi_baud_rate determine_baud_rate(spi_dev *dev, uint32_t freq) {
	uint32_t clock = 0, i;
	#ifdef SPI_DEBUG
	Serial.print("determine_baud_rate("); Serial.print(freq); Serial.println(")");
	#endif
    switch (rcc_dev_clk(dev->clk_id))
    {
    	case RCC_APB2: clock = STM32_PCLK2; break; // 72 Mhz
    	case RCC_APB1: clock = STM32_PCLK1; break; // 36 Mhz
		case RCC_AHB1: break;
		default: break;
    }
    clock /= 2;
    i = 0;
    while (i < 7 && freq < clock) {
      clock /= 2;
      i++;
    }
	return baud_rates[i];
}


SPIClass SPI(3); // needed for external libs
