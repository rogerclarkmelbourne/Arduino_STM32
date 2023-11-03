#ifndef _CAN_H_
#define _CAN_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include "libmaple_types.h"
#include <libmaple/libmaple_types.h>
#include <libmaple/gpio.h>
#include "libmaple/nvic.h"
#include <libmaple/rcc.h>
//#include "types.h"
#include "libmaple/usb.h"

#ifndef CAN_RX_QUEUE_SIZE
	#define CAN_RX_QUEUE_SIZE 16
#endif

/* peripheral addresses */
#define CAN1_BASE		((CAN_Port*)0x40006400)
#define CAN2_BASE		((CAN_Port*)0x40006800)


/* CAN Master Control Register bits */
#define CAN_MCR_INRQ	((uint32)0x00000001)	/* Initialization request */
#define CAN_MCR_SLEEP	((uint32)0x00000002)	/* Sleep mode request */
#define CAN_MCR_TXFP	((uint32)0x00000004)	/* Transmit FIFO priority */
#define CAN_MCR_RFLM	((uint32)0x00000008)	/* Receive FIFO locked mode */
#define CAN_MCR_NART	((uint32)0x00000010)	/* No automatic retransmission */
#define CAN_MCR_AWUM	((uint32)0x00000020)	/* Automatic wake up mode */
#define CAN_MCR_ABOM	((uint32)0x00000040)	/* Automatic bus-off management */
#define CAN_MCR_TTCM	((uint32)0x00000080)	/* Time triggered communication mode */
#define CAN_MCR_RESET	((uint32)0x00008000)	/* bxCAN software master reset */
#define CAN_MCR_DBF		((uint32)0x00010000)	/* Debug freeze */

/* CAN Master Status Register bits */
#define CAN_MSR_INAK	((uint32)0x00000001)	/* Initialization acknowledge */
#define CAN_MSR_SLAK	((uint32)0x00000002)	/* Sleep acknowledge */
#define CAN_MSR_ERRI	((uint32)0x00000004)	/* Error interrupt */
#define CAN_MSR_WKUI	((uint32)0x00000008)	/* Wake-up interrupt */
#define CAN_MSR_SLAKI	((uint32)0x00000010)	/* Sleep acknowledge interrupt */
#define CAN_MSR_TXM		((uint32)0x00000100)	/* Transmit mode */
#define CAN_MSR_RXM		((uint32)0x00000200)	/* Receive mode */
#define CAN_MSR_SAMP	((uint32)0x00000400)	/* Last sample point */
#define CAN_MSR_RX		((uint32)0x00000800)	/* CAN Rx signal */

/* CAN Transmit Status Register bits */
#define CAN_TSR_RQCP0	((uint32)0x00000001)    /* Request completed mailbox0 */
#define CAN_TSR_TXOK0	((uint32)0x00000002)    /* Transmission OK of mailbox0 */
#define CAN_TSR_ALST0	((uint32)0x00000004)	/* Arbitration Lost for Mailbox0 */
#define CAN_TSR_TERR0	((uint32)0x00000008)	/* Transmission Error of Mailbox0 */
#define CAN_TSR_ABRQ0	((uint32)0x00000080)    /* Abort request for mailbox0 */
#define CAN_TSR_RQCP1	((uint32)0x00000100)    /* Request completed mailbox1 */
#define CAN_TSR_TXOK1	((uint32)0x00000200)    /* Transmission OK of mailbox1 */
#define CAN_TSR_ALST1	((uint32)0x00000400)	/* Arbitration Lost for Mailbox1 */
#define CAN_TSR_TERR1	((uint32)0x00000800)	/* Transmission Error of Mailbox1 */
#define CAN_TSR_ABRQ1	((uint32)0x00008000)    /* Abort request for mailbox1 */
#define CAN_TSR_RQCP2	((uint32)0x00010000)    /* Request completed mailbox2 */
#define CAN_TSR_TXOK2	((uint32)0x00020000)    /* Transmission OK of mailbox2 */
#define CAN_TSR_ALST2	((uint32)0x00040000)	/* Arbitration Lost for mailbox 2 */
#define CAN_TSR_TERR2	((uint32)0x00080000)	/* Transmission Error of Mailbox 2 */
#define CAN_TSR_ABRQ2	((uint32)0x00800000)    /* Abort request for mailbox2 */
#define CAN_TSR_CODE	((uint32)0x03000000)	/* Mailbox Code */
#define CAN_TSR_TME		((uint32)0x1C000000)	/* TME[2:0] bits */
#define CAN_TSR_TME0	((uint32)0x04000000)    /* Transmit mailbox 0 empty */
#define CAN_TSR_TME1	((uint32)0x08000000)    /* Transmit mailbox 1 empty */
#define CAN_TSR_TME2	((uint32)0x10000000)    /* Transmit mailbox 2 empty */
#define CAN_TSR_LOW		((uint32)0xE0000000)	/* LOW[2:0] bits */
#define CAN_TSR_LOW0	((uint32)0x20000000)	/* Lowest Priority Flag for Mailbox 0 */
#define CAN_TSR_LOW1	((uint32)0x40000000)	/* Lowest Priority Flag for Mailbox 1 */
#define CAN_TSR_LOW2	((uint32)0x80000000)	/* Lowest Priority Flag for Mailbox 2 */

/* CAN Receive FIFO 0 Register bits */
#define CAN_RF0R_FMP0	((uint32)0x00000003)    /* FIFO 0 message pending */
#define CAN_RF0R_FULL0	((uint32)0x00000008)    /* FIFO 0 full */
#define CAN_RF0R_FOVR0	((uint32)0x00000010)    /* FIFO 0 overrun */
#define CAN_RF0R_RFOM0	((uint32)0x00000020)    /* Release FIFO 0 output mailbox */

/* CAN Receive FIFO 1 Register bits */
#define CAN_RF1R_FMP1	((uint32)0x00000003)    /* FIFO 1 message pending */
#define CAN_RF1R_FULL1	((uint32)0x00000008)    /* FIFO 1 full */
#define CAN_RF1R_FOVR1	((uint32)0x00000010)    /* FIFO 1 overrun */
#define CAN_RF1R_RFOM1	((uint32)0x00000020)    /* Release FIFO 1 output mailbox */

/* CAN Error Status Register bits */
#define CAN_ESR_EWGF	((uint32)0x00000001)    /* Error warning flag */
#define CAN_ESR_EPVF	((uint32)0x00000002)    /* Error passive flag */
#define CAN_ESR_BOFF	((uint32)0x00000004)    /* Bus-off flag */

/* CAN interrupt enable register (CAN_IER) */
#define CAN_IER_TMEIE	((uint32)0x00000001)	/* Transmit Mailbox Empty Interrupt Enable */
#define CAN_IER_FMPIE0	((uint32)0x00000002)	/* FIFO Message Pending Interrupt Enable */
#define CAN_IER_FFIE0	((uint32)0x00000004)	/* FIFO Full Interrupt Enable */
#define CAN_IER_FOVIE0	((uint32)0x00000008)	/* FIFO Overrun Interrupt Enable */
#define CAN_IER_FMPIE1	((uint32)0x00000010)	/* FIFO Message Pending Interrupt Enable */
#define CAN_IER_FFIE1	((uint32)0x00000020)	/* FIFO Full Interrupt Enable */
#define CAN_IER_FOVIE1	((uint32)0x00000040)	/* FIFO Overrun Interrupt Enable */
#define CAN_IER_EWGIE	((uint32)0x00000100)	/* Error Warning Interrupt Enable */
#define CAN_IER_EPVIE	((uint32)0x00000200)	/* Error Passive Interrupt Enable */
#define CAN_IER_BOFIE	((uint32)0x00000400)	/* Bus-Off Interrupt Enable */
#define CAN_IER_LECIE	((uint32)0x00000800)	/* Last Error Code Interrupt Enable */
#define CAN_IER_ERRIE	((uint32)0x00008000)	/* Error Interrupt Enable */
#define CAN_IER_WKUIE	((uint32)0x00010000)	/* Wakeup Interrupt Enable */
#define CAN_IER_SLKIE	((uint32)0x00020000)	/* Sleep Interrupt Enable */

/* CAN error status register (CAN_ESR) */
#define CAN_ESR_EWGF	((uint32)0x00000001)	/* Error Warning Flag */
#define CAN_ESR_EPVF	((uint32)0x00000002)	/* Error Passive Flag */
#define CAN_ESR_BOFF	((uint32)0x00000004)	/* Bus-Off Flag */

#define CAN_ESR_LEC		((uint32)0x00000070)	/* LEC[2:0] bits (Last Error Code) */
#define CAN_ESR_LEC_0	((uint32)0x00000010)	/* Bit 0 */
#define CAN_ESR_LEC_1	((uint32)0x00000020)	/* Bit 1 */
#define CAN_ESR_LEC_2	((uint32)0x00000040)	/* Bit 2 */

#define CAN_ESR_TEC		((uint32)0x00FF0000)	/* Least significant byte of the 9-bit Transmit Error Counter */
#define CAN_ESR_REC		((uint32)0xFF000000)	/* Receive Error Counter */

/* CAN bit timing register (CAN_BTR) */
#define CAN_BTR_SJW_POS	24
#define CAN_BTR_TS2_POS	20
#define CAN_BTR_TS1_POS	16

#define CAN_BTR_BRP		((uint32)0x000003FF)	/* Baud Rate Prescaler */
#define CAN_BTR_TS1 	((uint32)0x000F0000)	/* Time Segment 1 */
#define CAN_BTR_TS2 	((uint32)0x00700000)	/* Time Segment 2 */
#define CAN_BTR_SJW 	((uint32)0x03000000)	/* Resynchronization Jump Width */
#define CAN_BTR_LBKM	((uint32)0x40000000)	/* Loop Back Mode (Debug) */
#define CAN_BTR_SILM	((uint32)0x80000000)	/* Silent Mode */


/* CAN Mailbox Transmit Request */
#define CAN_TMIDxR_TXRQ	((uint32)0x00000001) /* Transmit mailbox request */

/* CAN Filter Master Register bits */
#define CAN_FMR_FINIT	((uint32)0x00000001) /* Filter init mode */


typedef enum CAN_GPIO_MAP {
	CAN_GPIO_PB8_PB9,		/* RX to PB8, TX to PB9 */
	CAN_GPIO_PD0_PD1		/* RX to PD0, TX to PD1 */
} CAN_GPIO_MAP;

typedef enum CAN_STATUS
{
	CAN_OK = 0,
	CAN_INIT_FAILED,
	CAN_INIT_E_FAILED,
	CAN_INIT_L_FAILED,
	CAN_TX_FAILED,
	CAN_TX_PENDING,
	CAN_NO_MB,
	CAN_FILTER_FULL
} CAN_STATUS;

typedef enum CAN_TX_MBX
{
	CAN_TX_MBX0 = 0,
	CAN_TX_MBX1 = 1,
	CAN_TX_MBX2 = 2,
	CAN_TX_NO_MBX = CAN_NO_MB
} CAN_TX_MBX;

#define CAN_MODE_NORMAL				((uint32)0x0)				/* normal mode */
#define CAN_MODE_LOOPBACK			(CAN_BTR_LBKM)				/* loopback mode */
#define CAN_MODE_SILENT				(CAN_BTR_SILM)				/* silent mode */
#define CAN_MODE_SILENT_LOOPBACK	(CAN_BTR_LBKM | CAN_BTR_SILM)	/* loopback combined with silent mode */

enum CAN_SPEED {
	CAN_SPEED_125,
	CAN_SPEED_250,
	CAN_SPEED_500,
	CAN_SPEED_1000,
};

/**
  *	CAN_identifier_type 
  */
#define CAN_ID_STD	((uint32)0x00)	/* Standard Id */
#define CAN_ID_EXT	((uint32)0x04)	/* Extended Id */

/**
  *	CAN_remote_transmission_request 
  */
#define CAN_RTR_DATA	((uint32)0x00)  /* Data frame */
#define CAN_RTR_REMOTE	((uint32)0x02)  /* Remote frame */

/**
  * CAN_receive_FIFO_number_constants
  */
typedef enum {
	CAN_FIFO0 = 0,
	CAN_FIFO1 = 1
} CAN_FIFO;

typedef enum {
	CAN_FILTER_32BIT	 = 0,
	CAN_FILTER_16BIT	 = 1
} CAN_FILTER_SCALE;

typedef enum {
	CAN_FILTER_MASK		= 0,
	CAN_FILTER_LIST		= 1
} CAN_FILTER_MODE;
/**
  * @brief Controller Area Network TxMailBox
  */
typedef struct
{
	volatile uint32 TIR;
	volatile uint32 TDTR;
	volatile uint32 TDLR;
	volatile uint32 TDHR;
} CAN_TxMailBox_Port;

/**
  * @brief Controller Area Network FIFOMailBox
  */
typedef struct
{
	volatile uint32 RIR;
	volatile uint32 RDTR;
	volatile uint32 RDLR;
	volatile uint32 RDHR;
} CAN_FIFOMailBox_Port;

/**
  * @brief Controller Area Network FilterRegister
  */
typedef struct
{
	volatile uint32 FR1;
	volatile uint32 FR2;
} CAN_FilterRegister_Port;

typedef struct {
    volatile uint32 MCR;	// CAN master control register (CAN_MCR)
    volatile uint32 MSR;	// CAN master status register (CAN_MSR)
    volatile uint32 TSR;	// CAN transmit status register (CAN_TSR)
	volatile uint32 RF0R;	// CAN receive FIFO 0 register (CAN_RF0R)
	volatile uint32 RF1R;	// CAN receive FIFO 1 register (CAN_RF1R)
	volatile uint32 IER;
  	volatile uint32 ESR;
  	volatile uint32 BTR;
		uint32	RESERVED0[88];
	CAN_TxMailBox_Port sTxMailBox[3];
	CAN_FIFOMailBox_Port sFIFOMailBox[2];
		uint32  RESERVED1[12];
	volatile uint32 FMR;
	volatile uint32 FM1R;
		uint32	RESERVED2;
	volatile uint32 FS1R;
		uint32	RESERVED3;
	volatile uint32	FFA1R;
		uint32	RESERVED4;
	volatile uint32	FA1R;
		uint32	RESERVED5[8];
	CAN_FilterRegister_Port sFilterRegister[14];
} CAN_Port;

/** 
  * @brief  CAN Tx message structure definition  
  */
typedef struct
{
	uint32 ID;		// CAN ID
	uint8 IDE;		// CAN_ID_STD for standard and CAN_ID_EXT for extended
	uint8 RTR;
	uint8 DLC;
	uint8 Data[8];
	uint8 FMI;
} CanMsg;

/* Functions */
CAN_STATUS can_init_enter(CAN_Port* CANx);
CAN_STATUS can_init_leave(CAN_Port* CANx);
CAN_STATUS can_init(CAN_Port *CANx, uint32 mode, uint8 speed);
CAN_STATUS can_deinit(CAN_Port* CANx);
CAN_STATUS can_filter(CAN_Port* CANx, uint8 filter_idx,  CAN_FIFO fifo_number, CAN_FILTER_SCALE scale, CAN_FILTER_MODE mode, uint32 fr1, uint32 fr2);
CAN_STATUS can_set_timing(CAN_Port* CANx, uint32 timing);
CAN_STATUS can_set_mode(CAN_Port* CANx, uint32 mode);
CAN_STATUS can_gpio_map(CAN_Port* CANx, CAN_GPIO_MAP map_mode);
CAN_STATUS can_status(void);
void can_cancel(CAN_Port* CANx, uint8 mbx);
void can_rx_queue_clear(void);
uint8 can_rx_available(void);
CanMsg* can_rx_queue_get(void);
CanMsg* can_read(CAN_Port* CANx, CAN_FIFO fifo, CanMsg* msg);
void can_rx_release(CAN_Port* CANx, CAN_FIFO fifo);
void can_rx_queue_free(void);
CAN_TX_MBX can_transmit(CAN_Port* CANx, CanMsg* msg);
CAN_STATUS can_tx_status(CAN_Port* CANx, CAN_TX_MBX mbx);

/**
  * @brief	Set pooling mode
  */
static inline void can_set_pool_mode(CAN_Port* CANx)
{
	CANx->IER &= ~(CAN_IER_FMPIE0 | CAN_IER_FMPIE1);
}

/**
  * @brief	Set interrupt mode
  */
static inline void can_set_irq_mode(CAN_Port* CANx)
{
	CANx->IER |= (CAN_IER_FMPIE0 | CAN_IER_FMPIE1);
}

static inline uint8 can_fifo_ready(CAN_Port* CANx, CAN_FIFO fifo)
{
	if (fifo == CAN_FIFO0)
		return (uint8)(CANx->RF0R & CAN_RF0R_FMP0);
	return (uint8)(CANx->RF1R & CAN_RF1R_FMP1);
}

#ifdef __cplusplus
}
#endif

#endif
