/******************************************************************************
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
 * @file sdio.h
 * @brief Secure digital input/output interface.
 */

#ifndef _SDIO_H_
#define _SDIO_H_

#include <libmaple/libmaple_types.h>
#include <libmaple/stm32.h>
#include <libmaple/gpio.h>


/*
#include <libmaple/rcc.h>
#include <libmaple/nvic.h>

//#include <boards.h>
#include <stdint.h>
//#include <wirish.h>
 */

#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)

/*
 * DMA controller and channel used in STM32F103
 */

#define SDIO_DMA_DEV        DMA2
#define SDIO_DMA_CHANNEL    DMA_CH4
/*
#ifdef __cplusplus
extern "C" {
#endif
*/

/*
 * Register maps and devices
 */

// SDIO register map type
typedef struct sdio_reg_map {
    __IO uint32 POWER;    // 0x00
    __IO uint32 CLKCR;    // 0x04
    __IO uint32 ARG;      // 0x08
    __IO uint32 CMD;      // 0x0C
    __IO uint32 RESPCMD;  // 0x10 (0x3F)
    __IO const uint32 RESP[4]; // 0x14 - contain the card status, which is part of the received response.
    __IO uint32 DTIMER;   // 0x24 - contains the data timeout period, in card bus clock periods.
    __IO uint32 DLEN;     // 0x28 (0x01FF FFFF) - contains the number of data bytes to be transferred
    __IO uint32 DCTRL;    // 0x2C
    __IO uint32 DCOUNT;   // 0x30 (0x01FF FFFF)
    __IO uint32 STA;      // 0x34
    __IO uint32 ICR;      // 0x38
    __IO uint32 MASK;     // 0x3C
    const uint32 RESERVED1[2];
    __IO uint32 FIFOCNT;  // 0x48 (0x01FF FFFF)
    const uint32 RESERVED2[13];
    __IO uint32 FIFO;     // 0x80
} sdio_reg_map;
#define sdio_dev sdio_reg_map

/** SDIO register map base pointer */
#define SDIO_BASE                       ((struct sdio_reg_map*)0x40018000)

extern sdio_dev * SDIO;

/*
 * Register bit definitions
 */

/* NOR/PSRAM chip-select control registers */

// SDIO_POWER register bits
// At least seven HCLK clock periods are needed between two write accesses to this register.
// After a data write, data cannot be written to this register for three SDIOCLK clock periods
// plus two PCLK2 clock periods.
#define SDIO_POWER_PWRCTRL_OFF         0x00
#define SDIO_POWER_PWRCTRL_ON          0x03

// SDIO_CLKCR register bits
// Controls the SDIO_CK output clock.
// After a data write, data cannot be written to this register for three SDIOCLK clock periods
// plus two PCLK2 clock periods. SDIO_CK can also be stopped during the read wait interval
// for SD I/O cards: in this case the SDIO_CLKCR register does not control SDIO_CK.
#define SDIO_CLKCR_HWFC_EN          (1<<14) // HW Flow Control enable - DON'T USE in F4!!! (see errata sheet 2.12.1)
                                            // Overrun errors (Rx mode) and FIFO underrun (Tx mode)
											// should be managed by the application software.
#define SDIO_CLKCR_NEGEDGE          (1<<13) // SDIO_CK de-phasing selection bit - DON'T USE!!! (see errata sheet 2.12.4)
#define SDIO_CLKCR_WIDBUS           (3<<11) // Data bus width
#define SDIO_CLKCR_WIDBUS_1BIT      (0<<11) // 1 bit (SDIO_D0 used)
#define SDIO_CLKCR_WIDBUS_4BIT      (1<<11) // 4-bit (SDIO_D[3:0] used)
#define SDIO_CLKCR_BYPASS           (1<<10) // Clock divider bypass enable bit - SDIO_CK = SDIOCLK, CLKDIV not relevant.
#define SDIO_CLKCR_PWRSAV           (1<<9)  // 0: SDIO_CK clock is always enabled, 1: SDIO_CK is only enabled when the bus is active
#define SDIO_CLKCR_CLKEN            (1<<8)  // Clock enable
#define SDIO_CLKCR_CLKDIV           (0xFF)  // SDIO_CK = SDIOCLK / [CLKDIV + 2]
#define SDIOCLK 					PCLK2 // SDIO master clock frequency

// SDIO_CMD register bits
// After a data write, data cannot be written to this register for three SDIOCLK clock periods
// plus two PCLK2 clock periods.
// MultiMediaCards can send two kinds of response: short responses, 48 bits long, or long
// responses,136 bits long. SD card and SD I/O card can send only short responses, the
// argument can vary according to the type of response: the software will distinguish the type
// of response according to the sent command. CE-ATA devices send only short responses.
#define SDIO_CMD_ATACMD          (1<<14)
#define SDIO_CMD_NIEN            (1<<13)
#define SDIO_CMD_ENCMDCOMPL      (1<<12)
#define SDIO_CMD_SDIOSUSPEND     (1<<11)
#define SDIO_CMD_CPSMEN          (1<<10)
#define SDIO_CMD_WAITPEND        (1<<9)
#define SDIO_CMD_WAITINT         (1<<8)
#define SDIO_CMD_WAITRESP        (3<<6)
#define SDIO_CMD_WAIT_NO_RESP    (0<<6)
#define SDIO_CMD_WAIT_SHORT_RESP (1<<6)
#define SDIO_CMD_WAIT_LONG_RESP  (3<<6)
#define SDIO_CMD_CMDINDEX        (0x3F)

// SDIO_DLEN register bits
// For a block data transfer, the value in the data length register must be a multiple of the block
// size (see SDIO_DCTRL). A data transfer must be written to the data timer register and the
// data length register before being written to the data control register.
// For an SDIO multibyte transfer the value in the data length register must be between 1 and 512.
#define SDIO_DLEN_DATALENGTH     (0x01FFFFFF)

// SDIO_DCTRL register bits
// Controls the data path state machine (DPSM).
// After a data write, data cannot be written to this register for three SDIOCLK clock periods
// plus two PCLK2 clock periods.
#define SDIO_DCTRL_SDIOEN        (1<<11)  // the DPSM performs an SD I/O-card-specific operation.
#define SDIO_DCTRL_RWMODE        (1<<10)  // 0: Read Wait control stopping SDIO_D2, 1:Read Wait control using SDIO_CK
#define SDIO_DCTRL_RWSTOP        (1<<9)   // 0: Read wait in progress if RWSTART bit is set, 1: Enable for read wait stop if RWSTART bit is set
#define SDIO_DCTRL_RWSTART       (1<<8)   // read wait operation starts
#define SDIO_DCTRL_DBLOCKSIZE    (0xF<<4) // Define the data block length when the block data transfer mode is selected: 2^N bytes
#define SDIO_BLOCKSIZE_64        (6<<4)
#define SDIO_BLOCKSIZE_512       (9<<4)
#define SDIO_DCTRL_DMAEN         (1<<3)   // DMA enable
#define SDIO_DCTRL_DTMODE        (1<<2)   // Data transfer mode selection: 0: Block data transfer, 1: Stream or SDIO multi-byte data transfer
#define SDIO_DCTRL_DTDIR         (1<<1)   // Data transfer direction selection: 0: From controller to card, 1: From card to controller.
#define SDIO_DIR_TX              (0<<1)
#define SDIO_DIR_RX              (1<<1)
#define SDIO_DCTRL_DTEN          (1<<0)   // Start data transfer. Depending on the direction bit, DTDIR,
                                          // the DPSM moves to the Wait_S, Wait_R state or Readwait if RW Start is set immediately at
										  // the beginning of the transfer. It is not necessary to clear the enable bit after the end of a data
										  // transfer but the SDIO_DCTRL must be updated to enable a new data transfer
// The meaning of the DTMODE bit changes according to the value of the SDIOEN bit:
// When DTEN=0 and DTMODE=1, the MultiMediaCard stream mode is enabled.
// When DTEN=1 and DTMODE=1, the peripheral enables an SDIO multi-byte transfer.

// SDIO_STA register bits
#define SDIO_STA_CEATAEND        (1<<23) // CE-ATA command completion signal received for CMD61
#define SDIO_STA_SDIOIT          (1<<22) // SDIO interrupt received
#define SDIO_STA_RXDAVL          (1<<21) // Data available in receive FIFO
#define SDIO_STA_TXDAVL          (1<<20) // Data available in transmit FIFO
#define SDIO_STA_RXFIFOE         (1<<19) // Receive FIFO empty
#define SDIO_STA_TXFIFOE         (1<<18) // Transmit FIFO empty (2 words)
#define SDIO_STA_RXFIFOF         (1<<17) // Receive FIFO full (2 words before the FIFO is full.)
#define SDIO_STA_TXFIFOF         (1<<16) // Transmit FIFO full
#define SDIO_STA_RXFIFOHF        (1<<15) // Receive FIFO half full: there are at least 8 words in the FIFO
#define SDIO_STA_TXFIFOHE        (1<<14) // Transmit FIFO half empty: at least 8 words can be written into the FIFO
#define SDIO_STA_RXACT           (1<<13) // Data receive in progress
#define SDIO_STA_TXACT           (1<<12) // Data transmit in progress
#define SDIO_STA_CMDACT          (1<<11) // Command transfer in progress
#define SDIO_STA_DBCKEND         (1<<10) // Data block sent/received (CRC check passed)
#define SDIO_STA_STBITERR        (1<<9)  // Start bit not detected on all data signals in wide bus mode
#define SDIO_STA_DATAEND         (1<<8)  // Data end (data counter SDIOCOUNT is zero)
#define SDIO_STA_CMDSENT         (1<<7)  // Command sent (no response required)
#define SDIO_STA_CMDREND         (1<<6)  // Command response received (CRC check passed)
#define SDIO_STA_RXOVERR         (1<<5)  // Received FIFO overrun error
#define SDIO_STA_TXUNDERR        (1<<4)  // Transmit FIFO underrun error
#define SDIO_STA_DTIMEOUT        (1<<3)  // Data timeout
#define SDIO_STA_CTIMEOUT        (1<<2)  // Command response timeout. The Command TimeOut period has a fixed value of 64 SDIO_CK clock periods.
#define SDIO_STA_DCRCFAIL        (1<<1)  // Data block sent/received (CRC check failed)
#define SDIO_STA_CCRCFAIL        (1<<0)  // Command response received (CRC check failed)

#define SDIO_STA_CMD_ERROR_FLAGS (SDIO_STA_CTIMEOUT | SDIO_STA_CCRCFAIL)
#define SDIO_STA_TRX_ERROR_FLAGS (SDIO_STA_STBITERR | SDIO_STA_RXOVERR | SDIO_STA_TXUNDERR | SDIO_STA_DTIMEOUT | SDIO_STA_DCRCFAIL)
#define SDIO_STA_TRX_ACT_FLAGS   (SDIO_STA_RXACT|SDIO_STA_TXACT)

// SDIO_ICR register bits (WO - write only)
#define SDIO_ICR_CEATAENDC       (1<<23) // clear CEATAEND  flag
#define SDIO_ICR_SDIOITC         (1<<22) // clear SDIOIT    flag
#define SDIO_ICR_DBCKENDC        (1<<10) // clear DBCKENDC  flag
#define SDIO_ICR_STBITERRC       (1<<9)  // clear STBITERRC flag
#define SDIO_ICR_DATAENDC        (1<<8)  // clear DATAENDC  flag
#define SDIO_ICR_CMDSENTC        (1<<7)  // clear CMDSENTC  flag
#define SDIO_ICR_CMDRENDC        (1<<6)  // clear CMDREND   flag
#define SDIO_ICR_RXOVERRC        (1<<5)  // clear RXOVERR   flag
#define SDIO_ICR_TXUNDERRC       (1<<4)  // clear TXUNDERR  flag
#define SDIO_ICR_DTIMEOUTC       (1<<3)  // clear DTIMEOUT  flag
#define SDIO_ICR_CTIMEOUTC       (1<<2)  // clear CTIMEOUT  flag
#define SDIO_ICR_DCRCFAILC       (1<<1)  // clear DCRCFAIL  flag
#define SDIO_ICR_CCRCFAILC       (1<<0)  // clear CCRCFAIL  flag

#define SDIO_ICR_CMD_FLAGS   (SDIO_ICR_CEATAENDC | SDIO_ICR_SDIOITC | SDIO_ICR_CMDSENTC | SDIO_ICR_CMDRENDC | SDIO_ICR_CTIMEOUTC | SDIO_ICR_CCRCFAILC)
#define SDIO_ICR_DATA_FLAGS  (SDIO_ICR_DBCKENDC | SDIO_ICR_STBITERRC | SDIO_ICR_DATAENDC | SDIO_ICR_RXOVERRC | SDIO_ICR_TXUNDERRC | SDIO_ICR_DTIMEOUTC | SDIO_ICR_DCRCFAILC)

// SDIO_MASK register bits
// Determines which status flags generate an interrupt request by setting the corresponding bit to 1b.
#define SDIO_MASK_CEATAENDIE        (1<<23) // enable CEATAEND interrupt 
#define SDIO_MASK_SDIOITIE          (1<<22) // enable SDIOIT   interrupt
#define SDIO_MASK_RXDAVLIE          (1<<21) // enable RXDAVL   interrupt
#define SDIO_MASK_TXDAVLIE          (1<<20) // enable TXDAVL   interrupt
#define SDIO_MASK_RXFIFOEIE         (1<<19) // enable RXFIFOE  interrupt
#define SDIO_MASK_TXFIFOEIE         (1<<18) // enable TXFIFOE  interrupt
#define SDIO_MASK_RXFIFOFIE         (1<<17) // enable RXFIFOF  interrupt
#define SDIO_MASK_TXFIFOFIE         (1<<16) // enable TXFIFOF  interrupt
#define SDIO_MASK_RXFIFOHFIE        (1<<15) // enable RXFIFOHF interrupt
#define SDIO_MASK_TXFIFOHEIE        (1<<14) // enable TXFIFOHE interrupt
#define SDIO_MASK_RXACTIE           (1<<13) // enable RXACT    interrupt
#define SDIO_MASK_TXACTIE           (1<<12) // enable TXACT    interrupt
#define SDIO_MASK_CMDACTIE          (1<<11) // enable CMDACT   interrupt
#define SDIO_MASK_DBCKENDIE         (1<<10) // enable DBCKENDC interrupt
#define SDIO_MASK_STBITERRIE        (1<<9)  // enable STBITERR interrupt
#define SDIO_MASK_DATAENDIE         (1<<8)  // enable DATAENDC interrupt
#define SDIO_MASK_CMDSENTIE         (1<<7)  // enable CMDSENTC interrupt
#define SDIO_MASK_CMDRENDIE         (1<<6)  // enable CMDREND  interrupt
#define SDIO_MASK_RXOVERRIE         (1<<5)  // enable RXOVERR  interrupt
#define SDIO_MASK_TXUNDERRIE        (1<<4)  // enable TXUNDERR interrupt
#define SDIO_MASK_DTIMEOUTIE        (1<<3)  // enable DTIMEOUT interrupt
#define SDIO_MASK_CTIMEOUTIE        (1<<2)  // enable CTIMEOUT interrupt
#define SDIO_MASK_DCRCFAILIE        (1<<1)  // enable DCRCFAIL interrupt
#define SDIO_MASK_CCRCFAILIE        (1<<0)  // enable CCRCFAIL interrupt


void sdio_enable(void);
void sdio_disable(void);
void sdio_begin(void);
uint8_t sdio_cmd_send(uint16_t cmd_index_resp_type, uint32_t arg);
void sdio_set_clock(uint32_t clk);
void sdio_set_dbus_width(uint16_t bus_w);
void sdio_set_dblock_size(uint8_t dbsize);
//void sdio_trx_enable(uint8_t dir);
inline void sdio_trx_enable(void)
{
	SDIO->DCTRL |= SDIO_DCTRL_DTEN; // enable data transfer
}

inline uint32_t sdio_cmd_xfer_ongoing(void) { return (SDIO->STA&SDIO_STA_CMDACT); }
inline uint32_t sdio_cmd_complete(void) { return (SDIO->STA&SDIO_STA_CMDSENT); }

inline void sdio_setup_transfer(uint32_t dtimer, uint32_t dlen, uint16_t flags)
{
	SDIO->ICR = SDIO_ICR_DATA_FLAGS; // clear data access relevant flags
	SDIO->DTIMER = dtimer;
	SDIO->DLEN = dlen;
	SDIO->DCTRL = flags;// | SDIO_DCTRL_DTEN; // enable data transfer
}

/*
#ifdef __cplusplus
} // extern "C"
#endif
*/

#endif /* (STM32_HIGH_DENSITY) || (STM32_XL_DENSITY) */

#endif
