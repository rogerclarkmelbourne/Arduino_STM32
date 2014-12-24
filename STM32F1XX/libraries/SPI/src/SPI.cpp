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

#include <libmaple/timer.h>
#include <libmaple/util.h>
#include <libmaple/rcc.h>

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

static void enable_device(spi_dev *dev,
                          bool as_master,
                          SPIFrequency frequency,
                          spi_cfg_flag endianness,
                          spi_mode mode);

static const spi_pins board_spi_pins[] __FLASH__ = {
    {BOARD_SPI1_NSS_PIN,
     BOARD_SPI1_SCK_PIN,
     BOARD_SPI1_MISO_PIN,
     BOARD_SPI1_MOSI_PIN},
    {BOARD_SPI2_NSS_PIN,
     BOARD_SPI2_SCK_PIN,
     BOARD_SPI2_MISO_PIN,
     BOARD_SPI2_MOSI_PIN},
#ifdef STM32_HIGH_DENSITY
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
    case 1:
        this->spi_d = SPI1;
        break;
    case 2:
        this->spi_d = SPI2;
        break;
#ifdef STM32_HIGH_DENSITY
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

void SPIClass::begin(uint32_t frequency, uint8_t bitOrder, uint8_t mode) {
    if (mode >= 4) {
        ASSERT(0);
        return;
    }
	//Serial.print(frequency);Serial.print(",");Serial.print(bitOrder);Serial.print(",");Serial.println(mode);// debugging
    spi_cfg_flag end = bitOrder == MSBFIRST ? SPI_FRAME_MSB : SPI_FRAME_LSB;
    spi_mode m = (spi_mode)mode;
    enable_device(this->spi_d, true, (SPIFrequency)frequency, end, m);

}


void SPIClass::begin(void) {


    this->begin(_clockDividerToFrequenyMap[_settings.clockDivider],_settings.bitOrder,_settings.dataMode);//originally SPI_1_125MHZ,MSBFIRST,0);
}

void SPIClass::beginSlave(uint32 bitOrder, uint32 mode) {
    if (mode >= 4) {
        ASSERT(0);
        return;
    }
    spi_cfg_flag end = bitOrder == MSBFIRST ? SPI_FRAME_MSB : SPI_FRAME_LSB;
    spi_mode m = (spi_mode)mode;
    enable_device(this->spi_d, false, (SPIFrequency)0, end, m);
}

void SPIClass::beginSlave(void) {
    this->beginSlave(_settings.bitOrder, _settings.dataMode);
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
//	Serial.print("Clock divider set to ");	Serial.println(clockDivider);// debugging
	_settings.clockDivider = clockDivider;
	this->begin();
}
void SPIClass::setBitOrder(uint8_t bitOrder)
{
	_settings.bitOrder = bitOrder;
	this->begin();
}
void SPIClass::setdataMode(uint8_t dataMode)
{
	_settings.dataMode = dataMode;
	this->begin();
}	


void SPIClass::beginTransaction(uint8_t pin, SPISettings settings)
{
	_SSPin=pin;
	pinMode(_SSPin,OUTPUT);
//	digitalWrite(_SSPin,LOW);
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
//	digitalWrite(_SSPin,HIGH);
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

void SPIClass::write(uint8 byte) {
    this->write(&byte, 1);
}

void SPIClass::write(const uint8 *data, uint32 length) {
    uint32 txed = 0;
    while (txed < length) {
        txed += spi_tx(this->spi_d, data + txed, length - txed);
    }
}

uint8 SPIClass::transfer(uint8 byte) {
    this->write(byte);
    return this->read();
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

static void configure_gpios(spi_dev *dev, bool as_master);
static spi_baud_rate determine_baud_rate(spi_dev *dev, SPIFrequency freq);

static const spi_pins* dev_to_spi_pins(spi_dev *dev) {
    switch (dev->clk_id) {
    case RCC_SPI1: return board_spi_pins;
    case RCC_SPI2: return board_spi_pins + 1;
#ifdef STM32_HIGH_DENSITY
    case RCC_SPI3: return board_spi_pins + 2;
#endif
    default:       return NULL;
    }
}

/* Enables the device in master or slave full duplex mode.  If you
 * change this code, you must ensure that appropriate changes are made
 * to SPIClass::end(). */
static void enable_device(spi_dev *dev,
                          bool as_master,
                          SPIFrequency freq,
                          spi_cfg_flag endianness,
                          spi_mode mode) {
    spi_baud_rate baud = determine_baud_rate(dev, freq);
    uint32 cfg_flags = (endianness | SPI_DFF_8_BIT | SPI_SW_SLAVE |
                        (as_master ? SPI_SOFT_SS : 0));

    spi_init(dev);
    configure_gpios(dev, as_master);
    if (as_master) {
        spi_master_enable(dev, baud, mode, cfg_flags);
    } else {
        spi_slave_enable(dev, mode, cfg_flags);
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

static const spi_baud_rate baud_rates[MAX_SPI_FREQS] __FLASH__ = {
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
static spi_baud_rate determine_baud_rate(spi_dev *dev, SPIFrequency freq) {
    if (rcc_dev_clk(dev->clk_id) == RCC_APB2 && freq == SPI_140_625KHZ) {
        /* APB2 peripherals are too fast for 140.625 KHz */
        ASSERT(0);
        return (spi_baud_rate)~0;
    }
    return (rcc_dev_clk(dev->clk_id) == RCC_APB2 ?
            baud_rates[freq + 1] :
            baud_rates[freq]);
}

SPIClass SPI(1);
