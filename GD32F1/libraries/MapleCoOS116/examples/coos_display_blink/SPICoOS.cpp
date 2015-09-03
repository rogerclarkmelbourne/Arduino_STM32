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

#include "SPICoOS.h"

//#define SPI_DEBUG

#include <libmaple/timer.h>
#include <libmaple/util.h>
#include <libmaple/rcc.h>
#include <MapleCoOS116.h>

#include "wirish.h"
#include "boards.h"

//#include "HardwareSerial.h"

#if CYCLES_PER_MICROSECOND != 72
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
    switch (spi_num) {
#if BOARD_NR_SPI >= 1
    case 1:
        this->spi_d = SPI1;
        break;
#endif
#if BOARD_NR_SPI >= 2
    case 2:
        this->spi_d = SPI2;
        break;
#endif
#if BOARD_NR_SPI >= 3
    case 3:
        this->spi_d = SPI3;
        break;
#endif
    default:
        ASSERT(0);
    }
	
	//pinMode(BOARD_SPI_DEFAULT_SS,OUTPUT);
}

/*
 * Set up/tear down
 */

void SPIClass::begin(void) {
    if (dataMode >= 4) {
        ASSERT(0);
        return;
    }
    uint32 flags = ((bitOrder == MSBFIRST ? SPI_FRAME_MSB : SPI_FRAME_LSB) | SPI_DFF_8_BIT | SPI_SW_SLAVE | SPI_SOFT_SS);
    spi_init(spi_d);
    configure_gpios(spi_d, 1);
	#ifdef SPI_DEBUG
	Serial.print("spi_master_enable("); Serial.print(clockDivider); Serial.print(","); Serial.print(dataMode); Serial.print(","); Serial.print(flags); Serial.println(")");
	#endif
    spi_master_enable(spi_d, (spi_baud_rate)clockDivider, (spi_mode)dataMode, flags);
	sem_dma1free = CoCreateSem (1, 1, EVENT_SORT_TYPE_FIFO); // Create semaphore for CoOS multitasking.
}

void SPIClass::beginSlave(void) {
    if (dataMode >= 4) {
        ASSERT(0);
        return;
    }
    uint32 flags = ((bitOrder == MSBFIRST ? SPI_FRAME_MSB : SPI_FRAME_LSB) | SPI_DFF_8_BIT | SPI_SW_SLAVE);
    spi_init(spi_d);
    configure_gpios(spi_d, 0);
	#ifdef SPI_DEBUG
	Serial.print("spi_slave_enable("); Serial.print(dataMode); Serial.print(","); Serial.print(flags); Serial.println(")");
	#endif
    spi_slave_enable(spi_d, (spi_mode)dataMode, flags);
}

void SPIClass::end(void) {
    if (!spi_is_enabled(this->spi_d)) {
        return;
    }

    // Follows RM0008's sequence for disabling a SPI in master/slave
    // full duplex mode.
    while (spi_is_rx_nonempty(this->spi_d)) {
        // FIXME [0.1.0] remove this once you have an interrupt based driver
        volatile uint16 rx __attribute__((unused)) = spi_rx_reg(this->spi_d);
    }
    while (!spi_is_tx_empty(this->spi_d))
        ;
    while (spi_is_busy(this->spi_d))
        ;
    spi_peripheral_disable(this->spi_d);
}

/* Roger Clark added  3 functions */
void SPIClass::setClockDivider(uint32_t clockDivider)
{
	#ifdef SPI_DEBUG
	Serial.print("Clock divider set to "); Serial.println(clockDivider);
	#endif
	this->clockDivider = clockDivider;
	this->begin();
}

void SPIClass::setBitOrder(BitOrder bitOrder)
{
	#ifdef SPI_DEBUG
	Serial.print("Bit order set to "); Serial.println(bitOrder);
	#endif
	this->bitOrder = bitOrder;
	this->begin();
}

/*	Victor Perez. Added to test changing datasize from 8 to 16 bit modes on the fly.
*	Input parameter should be SPI_CR1_DFF set to 0 or 1 on a 32bit word.
*	Requires an added function spi_data_size on  STM32F1 / cores / maple / libmaple / spi.c 
*/
void SPIClass::setDataSize(uint32 datasize) {
//	spi_data_size(this->spi_d, ds);
		uint32 cr1 = this->spi_d->regs->CR1; 
		datasize &= SPI_CR1_DFF;
		cr1 &= ~(SPI_CR1_DFF); 
		cr1 |= datasize;  
		this->spi_d->regs->CR1 = cr1; 
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
	this->dataMode = dataMode;
	this->begin();
}	


void SPIClass::beginTransaction(uint8_t pin, SPISettings settings)
{
	#ifdef SPI_DEBUG
	Serial.println("SPIClass::beginTransaction");
	#endif
	//_SSPin=pin;
	//pinMode(_SSPin,OUTPUT);
	//digitalWrite(_SSPin,LOW);
	setBitOrder(settings.bitOrder);
	setDataMode(settings.dataMode);
	setClockDivider(determine_baud_rate(spi_d, settings.clock));
	begin();
#if 0
// code from SAM core	
	uint8_t mode = interruptMode;
	if (mode > 0) {
		if (mode < 16) {
			if (mode & 1) PIOA->PIO_IDR = interruptMask[0];
			if (mode & 2) PIOB->PIO_IDR = interruptMask[1];
			if (mode & 4) PIOC->PIO_IDR = interruptMask[2];
			if (mode & 8) PIOD->PIO_IDR = interruptMask[3];
		} else {
			interruptSave = interruptsStatus();
			noInterrupts();
		}
	}
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(pin);
	bitOrder[ch] = settings.border;
	SPI_ConfigureNPCS(spi, ch, settings.config);
	//setBitOrder(pin, settings.border);
	//setDataMode(pin, settings.datamode);
	//setClockDivider(pin, settings.clockdiv);
#endif
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

uint8 SPIClass::read(void) {
    uint8 buf[1];
    this->read(buf, 1);
    return buf[0];
}

void SPIClass::read(uint8 *buf, uint32 len) {
    uint32 rxed = 0;
    while (rxed < len) {
        while (!spi_is_rx_nonempty(this->spi_d))
            ;
        buf[rxed++] = (uint8)spi_rx_reg(this->spi_d);
    }
}

void SPIClass::write(uint16 data) {
  //  this->write(&data, 1);

	/* Added for 16bit data Victor Perez. Roger Clark 
	 * Improved speed by just directly writing the single byte to the SPI data reg and wait for completion,	 * by taking the Tx code from transfer(byte)
	 * The original method, of calling write(*data, length) .
	 * This almost doubles the speed of this function.
	 */
  
	spi_tx_reg(this->spi_d, data); // "2. Write the first data item to be transmitted into the SPI_DR register (this clears the TXE flag)."
	while (spi_is_tx_empty(this->spi_d) == 0); // "5. Wait until TXE=1 ..."
	while (spi_is_busy(this->spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI." 
}

void SPIClass::write(uint8 byte) {
  //  this->write(&byte, 1);

	/* Roger Clark 
	 * Improved speed by just directly writing the single byte to the SPI data reg and wait for completion,	 * by taking the Tx code from transfer(byte)
	 * The original method, of calling write(*data, length) .
	 * This almost doubles the speed of this function.
	 */
  
	spi_tx_reg(this->spi_d, byte); // "2. Write the first data item to be transmitted into the SPI_DR register (this clears the TXE flag)."
	while (spi_is_tx_empty(this->spi_d) == 0); // "5. Wait until TXE=1 ..."
	while (spi_is_busy(this->spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI." 
}

void SPIClass::write(const uint8 *data, uint32 length) {
    uint32 txed = 0;
    while (txed < length) {
        txed += spi_tx(this->spi_d, data + txed, length - txed);
    }
	while (spi_is_tx_empty(this->spi_d) == 0); // "4. After writing the last data item into the SPI_DR register, wait until TXE=1 ..."
	while (spi_is_busy(this->spi_d) != 0); // "... then wait until BSY=0, this indicates that the transmission of the last data is complete."
}

uint8 SPIClass::transfer(uint8 byte) {
	uint8 b;
	spi_tx_reg(this->spi_d, byte); // "2. Write the first data item to be transmitted into the SPI_DR register (this clears the TXE flag)."
  	while (spi_is_rx_nonempty(this->spi_d) == 0); // "4. Wait until RXNE=1 ..."
  	b = spi_rx_reg(this->spi_d); // "... and read the last received data."
  	while (spi_is_tx_empty(this->spi_d) == 0); // "5. Wait until TXE=1 ..."
  	while (spi_is_busy(this->spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI."
    return b;
}
/*  Roger Clark and Victor Perez, 2015
*	Performs a DMA SPI transfer with at least a receive buffer.
*	If a TX buffer is not provided, FF is sent over and over for the lenght of the transfer. 
*	On exit TX buffer is not modified, and RX buffer cotains the received data.
*	Still in progress.
*/
uint8 SPIClass::dmaTransfer(uint8 *transmitBuf, uint8 *receiveBuf, uint16 length) {
	if (length == 0) return 0;
	uint8 b;
	if (spi_is_rx_nonempty(this->spi_d) == 1) b = spi_rx_reg(this->spi_d); //Clear the RX buffer in case a byte is waiting on it.
	dma1_ch3_Active=true;
    dma_init(DMA1);
	dma_attach_interrupt(DMA1, DMA_CH3, &SPIClass::DMA1_CH3_Event);
	
	// RX
	spi_rx_dma_enable(SPI1);
	dma_setup_transfer(DMA1, DMA_CH2, &SPI1->regs->DR, DMA_SIZE_8BITS,
                     receiveBuf, DMA_SIZE_8BITS, (DMA_MINC_MODE | DMA_TRNS_CMPLT));// receive buffer DMA
	dma_set_num_transfers(DMA1, DMA_CH2, length);
	
	// TX
	spi_tx_dma_enable(SPI1);	
	if (!transmitBuf) {
	static uint8_t ff = 0XFF;
	transmitBuf = &ff;
	dma_setup_transfer(DMA1, DMA_CH3, &SPI1->regs->DR, DMA_SIZE_8BITS,
                       transmitBuf, DMA_SIZE_8BITS, (DMA_FROM_MEM | DMA_TRNS_CMPLT));// Transmit FF repeatedly
	}
	else {
    dma_setup_transfer(DMA1, DMA_CH3, &SPI1->regs->DR, DMA_SIZE_8BITS,
                       transmitBuf, DMA_SIZE_8BITS, (DMA_MINC_MODE |  DMA_FROM_MEM | DMA_TRNS_CMPLT));// Transmit buffer DMA 
	}
	dma_set_num_transfers(DMA1, DMA_CH3, length);

        CoPendSem (sem_dma1free, 0);
	dma_enable(DMA1, DMA_CH2);// enable receive
	dma_enable(DMA1, DMA_CH3);// enable transmit
	
//	while (dma1_ch3_Active);
//	if (receiveBuf) {
	CoPendSem (sem_dma1free, 100);
        CoPostSem (sem_dma1free);
	/*
    uint32_t m = millis();
    while (dma1_ch3_Active) {
      if ((millis() - m) > 100)  {
        dma1_ch3_Active = 0;
		b = 2;
		break;
      }
    }
	*/

//	}
	while (spi_is_tx_empty(this->spi_d) == 0); // "5. Wait until TXE=1 ..."
	while (spi_is_busy(this->spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI." 
    dma_disable(DMA1, DMA_CH3);
	dma_disable(DMA1, DMA_CH2);
	spi_rx_dma_disable(SPI1);
	spi_tx_dma_disable(SPI1);
    return b;
}

/*  Roger Clark and Victor Perez, 2015
*	Performs a DMA SPI send using a TX buffer.
*	On exit TX buffer is not modified.
*	Still in progress.
*/
uint8 SPIClass::dmaSend(uint8 *transmitBuf, uint16 length, bool minc  = 1) {
	if (length == 0) return 0;
	uint32 flags = ((DMA_MINC_MODE * minc) |  DMA_FROM_MEM | DMA_TRNS_CMPLT);
	uint8 b;
	dma1_ch3_Active=true;
    dma_init(DMA1);
	dma_attach_interrupt(DMA1, DMA_CH3, &SPIClass::DMA1_CH3_Event);

	// TX
	spi_tx_dma_enable(SPI1);	
    dma_setup_transfer(DMA1, DMA_CH3, &SPI1->regs->DR, DMA_SIZE_8BITS,
                       transmitBuf, DMA_SIZE_8BITS, flags);// Transmit buffer DMA 
	dma_set_num_transfers(DMA1, DMA_CH3, length);

	if (length > 32) {
        CoPendSem (sem_dma1free, 0);
        dma_enable(DMA1, DMA_CH3);// enable transmit
	CoPendSem (sem_dma1free, 10); //Check if the semaphore was release by the DMA ISR
        CoPostSem (sem_dma1free);
        }
        else {
	dma_enable(DMA1, DMA_CH3);// enable transmit
        while (dma1_ch3_Active);
        }
        
	while (spi_is_rx_nonempty(this->spi_d) == 0); // "4. Wait until RXNE=1 ..."
  	b = spi_rx_reg(this->spi_d); // "... and read the last received data."
	while (spi_is_tx_empty(this->spi_d) == 0); // "5. Wait until TXE=1 ..."
	while (spi_is_busy(this->spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI." 
	dma_disable(DMA1, DMA_CH3);
	spi_tx_dma_disable(SPI1);
    return b;
}

uint8 SPIClass::dmaSend(uint16 *transmitBuf, uint16 length, bool minc  = 1) {
	if (length == 0) return 0;
	uint32 flags = ((DMA_MINC_MODE * minc) |  DMA_FROM_MEM | DMA_TRNS_CMPLT);
	uint8 b;
	dma1_ch3_Active=true;
    dma_init(DMA1);
	dma_attach_interrupt(DMA1, DMA_CH3, &SPIClass::DMA1_CH3_Event);

	// TX
	spi_tx_dma_enable(SPI1);	
    dma_setup_transfer(DMA1, DMA_CH3, &SPI1->regs->DR, DMA_SIZE_16BITS,
                       transmitBuf, DMA_SIZE_16BITS, flags);// Transmit buffer DMA 
	dma_set_num_transfers(DMA1, DMA_CH3, length);

	if (length > 32) {
        CoPendSem (sem_dma1free, 0);
        dma_enable(DMA1, DMA_CH3);// enable transmit
	CoPendSem (sem_dma1free, 10); //Check if the semaphore was release by the DMA ISR
        CoPostSem (sem_dma1free);
        }   
        else {
	dma_enable(DMA1, DMA_CH3);// enable transmit
        while (dma1_ch3_Active);
        }

	while (spi_is_rx_nonempty(this->spi_d) == 0); // "4. Wait until RXNE=1 ..."
  	b = spi_rx_reg(this->spi_d); // "... and read the last received data."
	while (spi_is_tx_empty(this->spi_d) == 0); // "5. Wait until TXE=1 ..."
	while (spi_is_busy(this->spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI." 
	dma_disable(DMA1, DMA_CH3);
	spi_tx_dma_disable(SPI1);
    return b;
}


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
    return dev_to_spi_pins(this->spi_d)->miso;
}

uint8 SPIClass::mosiPin(void) {
    return dev_to_spi_pins(this->spi_d)->mosi;
}

uint8 SPIClass::sckPin(void) {
    return dev_to_spi_pins(this->spi_d)->sck;
}

uint8 SPIClass::nssPin(void) {
    return dev_to_spi_pins(this->spi_d)->nss;
}

/*
 * Deprecated functions
 */

uint8 SPIClass::send(uint8 data) {
    uint8 buf[] = {data};
    return this->send(buf, 1);
}

uint8 SPIClass::send(uint8 *buf, uint32 len) {
    uint32 txed = 0;
    uint8 ret = 0;
    while (txed < len) {
        this->write(buf[txed++]);
        ret = this->read();
    }
    return ret;
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

    spi_config_gpios(dev, as_master, nssi->gpio_device, nssi->gpio_bit,
                     scki->gpio_device, scki->gpio_bit, misoi->gpio_bit,
                     mosii->gpio_bit);
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
    }
    clock /= 2;
    i = 0;
    while (i < 7 && freq < clock) {
      clock /= 2;
      i++;
    }
	return baud_rates[i];
}

SPIClass SPI(1);
