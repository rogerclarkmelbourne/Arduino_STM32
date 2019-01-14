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

#include <libmaple/sdio.h>
#include <libmaple/gpio.h>
#include <boards.h>
#include "wirish.h"


//#include <libmaple/libmaple.h>
//#include <libmaple/rcc.h>
//#include <series/gpio.h>
//#include "wirish.h"
//#include "boards.h"
//

#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)

sdio_dev * SDIO = SDIO_BASE;

#define DELAY_LONG 20
#define DELAY_SHORT 2

uint8_t dly = DELAY_LONG; // microseconds delay after accessing registers

/*
 * SDIO convenience routines
 */
void sdio_gpios_init(void)
{
	gpio_set_mode(PIN_MAP[BOARD_SDIO_D0].gpio_device, PIN_MAP[BOARD_SDIO_D0].gpio_bit, GPIO_AF_OUTPUT_PP);
/*	gpio_set_mode(PIN_MAP[BOARD_SDIO_D1].gpio_device, PIN_MAP[BOARD_SDIO_D1].gpio_bit, GPIO_AF_OUTPUT_PP);
	gpio_set_mode(PIN_MAP[BOARD_SDIO_D2].gpio_device, PIN_MAP[BOARD_SDIO_D2].gpio_bit, GPIO_AF_OUTPUT_PP);
	gpio_set_mode(PIN_MAP[BOARD_SDIO_D3].gpio_device, PIN_MAP[BOARD_SDIO_D3].gpio_bit, GPIO_AF_OUTPUT_PP);
*/
    gpio_set_mode(PIN_MAP[BOARD_SDIO_D1].gpio_device, PIN_MAP[BOARD_SDIO_D1].gpio_bit, GPIO_INPUT_PU);
    gpio_set_mode(PIN_MAP[BOARD_SDIO_D2].gpio_device, PIN_MAP[BOARD_SDIO_D2].gpio_bit, GPIO_INPUT_PU);
    gpio_set_mode(PIN_MAP[BOARD_SDIO_D3].gpio_device, PIN_MAP[BOARD_SDIO_D3].gpio_bit, GPIO_INPUT_PU);
	gpio_set_mode(PIN_MAP[BOARD_SDIO_CLK].gpio_device, PIN_MAP[BOARD_SDIO_CLK].gpio_bit, GPIO_AF_OUTPUT_PP);
	gpio_set_mode(PIN_MAP[BOARD_SDIO_CMD].gpio_device, PIN_MAP[BOARD_SDIO_CMD].gpio_bit, GPIO_AF_OUTPUT_PP);
	/*
	 *  Todo just remove it, not needed for F1.
	 */
     /*
	gpio_set_af_mode(BOARD_SDIO_D0, 12);
	gpio_set_af_mode(BOARD_SDIO_D1, 12);
	gpio_set_af_mode(BOARD_SDIO_D2, 12);
	gpio_set_af_mode(BOARD_SDIO_D3, 12);
	gpio_set_af_mode(BOARD_SDIO_CLK, 12);
	gpio_set_af_mode(BOARD_SDIO_CMD, 12);
    */
}

void sdio_gpios_deinit(void)
{
	gpio_set_mode(PIN_MAP[BOARD_SDIO_D0].gpio_device, PIN_MAP[BOARD_SDIO_D0].gpio_bit, GPIO_INPUT_PU);
	gpio_set_mode(PIN_MAP[BOARD_SDIO_D1].gpio_device, PIN_MAP[BOARD_SDIO_D1].gpio_bit, GPIO_INPUT_PU);
	gpio_set_mode(PIN_MAP[BOARD_SDIO_D2].gpio_device, PIN_MAP[BOARD_SDIO_D2].gpio_bit, GPIO_INPUT_PU);
	gpio_set_mode(PIN_MAP[BOARD_SDIO_D3].gpio_device, PIN_MAP[BOARD_SDIO_D3].gpio_bit, GPIO_INPUT_PU);
	gpio_set_mode(PIN_MAP[BOARD_SDIO_CLK].gpio_device, PIN_MAP[BOARD_SDIO_CLK].gpio_bit, GPIO_INPUT_PU);
	gpio_set_mode(PIN_MAP[BOARD_SDIO_CMD].gpio_device, PIN_MAP[BOARD_SDIO_CMD].gpio_bit, GPIO_INPUT_PU);

	/*
	 *  Todo just remove it, not needed for F1.
	 */
     /*
	gpio_set_af_mode(BOARD_SDIO_D0, 0);
	gpio_set_af_mode(BOARD_SDIO_D1, 0);
	gpio_set_af_mode(BOARD_SDIO_D2, 0);
	gpio_set_af_mode(BOARD_SDIO_D3, 0);
	gpio_set_af_mode(BOARD_SDIO_CLK, 0);
	gpio_set_af_mode(BOARD_SDIO_CMD, 0);
     */
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
	delay_us(DELAY_LONG);
}

void sdio_power_off(void)
{
    SDIO->POWER = SDIO_POWER_PWRCTRL_OFF;
// After a data write, data cannot be written to this register for three SDIOCLK clock periods
// plus two PCLK2 clock periods.
	delay_us(DELAY_LONG);
}

void sdio_set_clock(uint32_t clk)
{
	/*
	 * limit the SDIO master clock to 8/3 of PCLK2.See RM 22.3
	 * Also limited to no more than 48Mhz
	 */
    clk = min(clk,(SDIOCLK/3)*8);
    clk = min(clk,36000000);

	if (clk<1000000) dly = DELAY_LONG;
	else dly = DELAY_SHORT;

	/*
	 *  round up divider, so we don't run the card over the speed supported.

	 */
	uint32 div = SDIOCLK/clk + (SDIOCLK % clk != 0) - 2;


	sdio_disable();
	//Serial.println(div,DEC);
	SDIO->CLKCR = (SDIO->CLKCR & (~(SDIO_CLKCR_CLKDIV|SDIO_CLKCR_BYPASS))) | SDIO_CLKCR_PWRSAV | SDIO_CLKCR_HWFC_EN | SDIO_CLKCR_CLKEN  | (div & SDIO_CLKCR_CLKDIV);
	delay_us(dly);
}

void sdio_set_dbus_width(uint16_t bus_w)
{
	SDIO->CLKCR = (SDIO->CLKCR & (~SDIO_CLKCR_WIDBUS)) | bus_w;
    gpio_set_mode(PIN_MAP[BOARD_SDIO_D1].gpio_device, PIN_MAP[BOARD_SDIO_D1].gpio_bit, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PIN_MAP[BOARD_SDIO_D2].gpio_device, PIN_MAP[BOARD_SDIO_D2].gpio_bit, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PIN_MAP[BOARD_SDIO_D3].gpio_device, PIN_MAP[BOARD_SDIO_D3].gpio_bit, GPIO_AF_OUTPUT_PP);
	delay_us(dly);
}

void sdio_set_dblock_size(uint8_t dbsize)
{
	SDIO->DCTRL = (SDIO->DCTRL&(~SDIO_DCTRL_DBLOCKSIZE)) | ((dbsize&0xF)<<4);
	delay_us(dly);
}

void sdio_enable(void)
{
	SDIO->CLKCR |= SDIO_CLKCR_CLKEN;
	delay_us(dly);
}

void sdio_disable(void)
{
	SDIO->CLKCR ^= SDIO_CLKCR_CLKEN;
	delay_us(dly);
}

/**
 * @brief Configure and enable the SDIO device.
 */
void sdio_begin(void)
{

	sdio_init();
	sdio_power_on();
  sdio_gpios_init();
	// Set initial SCK rate.
	sdio_set_clock(400000);
	delay_us(200); // generate 80 pulses at 400kHz
}

/**
 * @brief Disables the SDIO device.
 */
void sdio_end(void)
{
  while ( sdio_cmd_xfer_ongoing() );
  sdio_disable();
  sdio_gpios_deinit();
	sdio_power_off();
    rcc_clk_disable(RCC_SDIO);
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

#endif // defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
