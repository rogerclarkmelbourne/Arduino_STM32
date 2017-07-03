/******************************************************************************
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
 * @file libmaple/sdio.c
 * @author stevstrong
 * @brief Secure digital input/output interface.
 */

#include "sdio.h"
#include "gpio.h"
#include "wirish_time.h"

#if defined(BOARD_generic_f407v)

sdio_dev * SDIO = SDIO_BASE;

#define DELAY_LONG 10
#define DELAY_SHORT 1

uint8_t dly = DELAY_LONG; // microseconds delay after accessing registers

/*
 * SDIO convenience routines
 */
static void sdio_gpios_init(void)
{
	gpio_set_mode(BOARD_SDIO_D0, GPIO_AF_OUTPUT_PP_PU);
	gpio_set_mode(BOARD_SDIO_D1, GPIO_AF_OUTPUT_PP_PU);
	gpio_set_mode(BOARD_SDIO_D2, GPIO_AF_OUTPUT_PP_PU);
	gpio_set_mode(BOARD_SDIO_D3, GPIO_AF_OUTPUT_PP_PU);
	gpio_set_mode(BOARD_SDIO_CLK, GPIO_AF_OUTPUT_PP);
	gpio_set_mode(BOARD_SDIO_CMD, GPIO_AF_OUTPUT_PP_PU);
	//
	gpio_set_af_mode(BOARD_SDIO_D0, 12);
	gpio_set_af_mode(BOARD_SDIO_D1, 12);
	gpio_set_af_mode(BOARD_SDIO_D2, 12);
	gpio_set_af_mode(BOARD_SDIO_D3, 12);
	gpio_set_af_mode(BOARD_SDIO_CLK, 12);
	gpio_set_af_mode(BOARD_SDIO_CMD, 12);
}

static void sdio_gpios_deinit(void)
{
	gpio_set_mode(BOARD_SDIO_D0, GPIO_INPUT_FLOATING);
	gpio_set_mode(BOARD_SDIO_D1, GPIO_INPUT_FLOATING);
	gpio_set_mode(BOARD_SDIO_D2, GPIO_INPUT_FLOATING);
	gpio_set_mode(BOARD_SDIO_D3, GPIO_INPUT_FLOATING);
	gpio_set_mode(BOARD_SDIO_CLK, GPIO_INPUT_FLOATING);
	gpio_set_mode(BOARD_SDIO_CMD, GPIO_INPUT_FLOATING);
	//
	gpio_set_af_mode(BOARD_SDIO_D0, 0);
	gpio_set_af_mode(BOARD_SDIO_D1, 0);
	gpio_set_af_mode(BOARD_SDIO_D2, 0);
	gpio_set_af_mode(BOARD_SDIO_D3, 0);
	gpio_set_af_mode(BOARD_SDIO_CLK, 0);
	gpio_set_af_mode(BOARD_SDIO_CMD, 0);
}

/**
 * @brief Initialize and reset the SDIO device.
 */
void sdio_init(void)
{
    rcc_clk_enable(RCC_SDIO);
    rcc_reset_dev(RCC_SDIO);
}

void sdio_power_on(void)
{
    SDIO->POWER = SDIO_POWER_PWRCTRL_ON;
// After a data write, data cannot be written to this register for three SDIOCLK clock periods
// plus two PCLK2 clock periods.
	delayMicroseconds(DELAY_LONG);
}

void sdio_power_off(void)
{
    SDIO->POWER = SDIO_POWER_PWRCTRL_OFF;
// After a data write, data cannot be written to this register for three SDIOCLK clock periods
// plus two PCLK2 clock periods.
	delayMicroseconds(DELAY_LONG);
}

/**
 * @brief Enable the SDIO peripheral

void sdio_peripheral_enable(void) {
    bb_peri_set_bit(SDIO->CR1, SPI_CR1_SPE_BIT, 1);
}
 */
/**
 * @brief Disable a SPI peripheral

void sdio_peripheral_disable(spi_dev *dev) {
    bb_peri_set_bit(&dev->regs->CR1, SPI_CR1_SPE_BIT, 0);
}
 */
void sdio_set_clock(uint32_t clk)
{
	if (clk>24000000UL) clk = 24000000UL; // limit the SDIO master clock to 24MHz

	if (clk<1000000) dly = DELAY_LONG;
	else dly = DELAY_SHORT;

	sdio_disable();
	SDIO->CLKCR = (SDIO->CLKCR & (~(SDIO_CLKCR_CLKDIV|SDIO_CLKCR_BYPASS))) | SDIO_CLKCR_CLKEN | (((SDIOCLK/clk)-2)&SDIO_CLKCR_CLKDIV);
	delayMicroseconds(dly);
}

void sdio_set_dbus_width(uint16_t bus_w)
{
	SDIO->CLKCR = (SDIO->CLKCR & (~SDIO_CLKCR_WIDBUS)) | bus_w;
	delayMicroseconds(dly);
}

void sdio_set_dblock_size(uint8_t dbsize)
{
	SDIO->DCTRL = (SDIO->DCTRL&(~SDIO_DCTRL_DBLOCKSIZE)) | ((dbsize&0xF)<<4);
	delayMicroseconds(dly);
}

void sdio_enable(void)
{
	SDIO->CLKCR |= SDIO_CLKCR_CLKEN;
	delayMicroseconds(dly);
}

void sdio_disable(void)
{
	SDIO->CLKCR ^= SDIO_CLKCR_CLKEN;
	delayMicroseconds(dly);
}

/**
 * @brief Configure and enable the SDIO device.
 */
void sdio_begin(void)
{
	sdio_gpios_init();
	sdio_init();
	sdio_power_on();
	// Set initial SCK rate.
	sdio_set_clock(400000);
	delayMicroseconds(200); // generate 80 pulses at 400kHz
}

/**
 * @brief Disables the SDIO device.
 */
void sdio_end(void)
{
	sdio_disable();
	while ( sdio_cmd_xfer_ongoing() );
	sdio_power_off();
    rcc_clk_disable(RCC_SDIO);
	sdio_gpios_deinit();
}

/**
 * @brief Send command by the SDIO device.
 */
uint8_t sdio_cmd_send(uint16_t cmd_index_resp_type, uint32_t arg)
{
	uint8_t retries = 10; // in case of errors
	do { // retry command if errors detected
		// clear interrupt flags - IMPORTANT!!!
		SDIO->ICR = SDIO_ICR_CMD_FLAGS;
		// write command
		SDIO->ARG = arg;
		SDIO->CMD = (uint32_t)(SDIO_CMD_CPSMEN | cmd_index_resp_type );
		while ( (SDIO->STA&SDIO_STA_CMDACT) ) ; // wait for actual command transfer to finish
		// wait for response, if the case
		if ( cmd_index_resp_type&(SDIO_CMD_WAIT_SHORT_RESP|SDIO_CMD_WAIT_LONG_RESP) ) {
			while ( !(SDIO->STA&(SDIO_STA_CMDREND|SDIO_STA_CMD_ERROR_FLAGS)) ) ;
		} else break; // no response required
		if ( SDIO->STA&(SDIO_STA_CMDREND|SDIO_STA_CTIMEOUT) )
			break; // response received or timeout
		// ignore CRC error for CMD5 and ACMD41
		if ( ((cmd_index_resp_type&SDIO_CMD_CMDINDEX)==5) || ((cmd_index_resp_type&SDIO_CMD_CMDINDEX)==41) )
			break;
	} while ( (--retries) );
	return (uint8_t)retries;
}

#endif // BOARD_generic_f407v
