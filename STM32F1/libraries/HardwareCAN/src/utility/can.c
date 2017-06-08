//#include "libmaple.h"
#include "can.h"
//#include "rcc.h"
//#include "gpio.h"
//#include "nvic.h"
//#include "usb.h"

/**
  *	CAN_interrupts
  */

#define CAN_IT_RQCP0	((uint32)0x00000005) /* Request completed mailbox 0 */
#define CAN_IT_RQCP1	((uint32)0x00000006) /* Request completed mailbox 1 */
#define CAN_IT_RQCP2	((uint32)0x00000007) /* Request completed mailbox 2 */
#define CAN_IT_TME		((uint32)0x00000001) /* Transmit mailbox empty */
#define CAN_IT_FMP0		((uint32)0x00000002) /* FIFO 0 message pending */
#define CAN_IT_FF0		((uint32)0x00000004) /* FIFO 0 full */
#define CAN_IT_FOV0		((uint32)0x00000008) /* FIFO 0 overrun */
#define CAN_IT_FMP1		((uint32)0x00000010) /* FIFO 1 message pending */
#define CAN_IT_FF1		((uint32)0x00000020) /* FIFO 1 full */
#define CAN_IT_FOV1		((uint32)0x00000040) /* FIFO 1 overrun */
#define CAN_IT_EWG		((uint32)0x00000100) /* Error warning */
#define CAN_IT_EPV		((uint32)0x00000200) /* Error passive */
#define CAN_IT_BOF		((uint32)0x00000400) /* Bus-off */
#define CAN_IT_LEC		((uint32)0x00000800) /* Last error code */
#define CAN_IT_ERR		((uint32)0x00008000) /* Error */
#define CAN_IT_WKU		((uint32)0x00010000) /* Wake-up */
#define CAN_IT_SLK		((uint32)0x00020000) /* Sleep */

/* Time out for INAK bit */
#define CAN_INAK_TimeOut	((uint32)0x0000FFFF)

/* Time out for SLAK bit */
#define CAN_SLAK_TimeOut	((uint32)0x0000FFFF)

#define CAN_CONTROL_MASK	(CAN_MCR_TTCM | CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_NART | CAN_MCR_RFLM | CAN_MCR_TXFP)
#define CAN_TIMING_MASK		(CAN_BTR_SJW | CAN_BTR_TS2 | CAN_BTR_TS1 | CAN_BTR_BRP)
#define CAN_MODE_MASK		(CAN_BTR_LBKM | CAN_BTR_SILM)

struct can_speed_info {
    const uint32 btr;
};

#define CAN_CLOCK	(36000000UL / 18UL)

static const struct can_speed_info can_speed_table[] = {
	[CAN_SPEED_125]		= { .btr = (
		(( 4-1) << CAN_BTR_SJW_POS) |
		((12-1) << CAN_BTR_TS1_POS) |
		(( 5-1) << CAN_BTR_TS2_POS) |
		(CAN_CLOCK / 125000UL - 1)
		)},
	[CAN_SPEED_250]		= { .btr = (
		(( 4-1) << CAN_BTR_SJW_POS) |
		((12-1) << CAN_BTR_TS1_POS) |
		(( 5-1) << CAN_BTR_TS2_POS) |
		(CAN_CLOCK / 250000UL - 1)
		)},
	[CAN_SPEED_500]		= { .btr = (
		(( 4-1) << CAN_BTR_SJW_POS) |
		((12-1) << CAN_BTR_TS1_POS) |
		(( 5-1) << CAN_BTR_TS2_POS) |
		(CAN_CLOCK / 500000UL - 1)
		)},
	[CAN_SPEED_1000]	= { .btr = (
		(( 4-1) << CAN_BTR_SJW_POS) |
		((12-1) << CAN_BTR_TS1_POS) |
		(( 5-1) << CAN_BTR_TS2_POS) |
		(CAN_CLOCK / 1000000UL - 1)
		)}
};

CAN_STATUS status;
CanMsg can_rx_queue[CAN_RX_QUEUE_SIZE];

uint8 can_rx_head;
uint8 can_rx_tail;
uint8 can_rx_count;
uint8 can_rx_lost;
uint8 can_active = 0;

/**
  * @brief  Return last operation status
  */
CAN_STATUS can_status(void)
{
	return status;
}

/**
  * @brief  Enter initialization mode
  */
CAN_STATUS can_init_enter(CAN_Port* CANx)
{
	volatile uint32 wait_ack = 0 ;
    
	status = CAN_OK;
	if ((CANx->MSR & CAN_MSR_INAK) == 0)	// Check for initialization mode already set
	{
		CANx->MCR |= CAN_MCR_INRQ;			// Request initialisation

		wait_ack = 0;						// Wait the acknowledge
		while ((wait_ack != CAN_INAK_TimeOut) && ((CANx->MSR & CAN_MSR_INAK) == 0))
			wait_ack++;
		if ((CANx->MSR & CAN_MSR_INAK) == 0)
			status = CAN_INIT_E_FAILED;		// Timeout
	}
	return status;
}

/**
  * @brief  Leave initialization mode
  */
CAN_STATUS can_init_leave(CAN_Port* CANx)
{
	volatile uint32 wait_ack = 0 ;

	status = CAN_OK;
	if ((CANx->MSR & CAN_MSR_INAK) != 0)	// Check for initialization mode already reset
	{
		CANx->MCR &= ~CAN_MCR_INRQ;			// Clear Request initialization

		wait_ack = 0;						// Wait the acknowledge
		while ((wait_ack != CAN_INAK_TimeOut) && ((CANx->MSR & CAN_MSR_INAK) != 0))
			wait_ack++;
		if ((CANx->MSR & CAN_MSR_INAK) != 0)
			status = CAN_INIT_L_FAILED;
	}
	return status;
}

/**
  * @brief  Deinitializes the CAN peripheral registers to their default reset values.
  */
CAN_STATUS can_deinit(CAN_Port* CANx)
{
	if (CANx == CAN1_BASE)
	{
		nvic_irq_disable(NVIC_USB_LP_CAN_RX0);	// Disable interrupts
		nvic_irq_disable(NVIC_USB_HP_CAN_TX);
		rcc_reset_dev(RCC_CAN);
		rcc_clk_disable(RCC_CAN);
		can_active = 0;
	}
	return (status = CAN_OK);
}

/**
  * @brief  Initialize CAN registers
  */
/*
 *		Bits in control parameter:
 *			CAN_MCR_TTCM	time triggered communication mode
 *			CAN_MCR_ABOM	automatic bus-off management
 *			CAN_MCR_AWUM	automatic wake-up mode
 *			CAN_MCR_NART	no automatic retransmission
 *			CAN_MCR_RFLM	receive FIFO locked mode
 *			CAN_MCR_TXFP	transmit FIFO priority
 */
CAN_STATUS can_init(CAN_Port* CANx, uint32 control, uint8 speed)
{
	status = CAN_INIT_FAILED;			// default result status
										// initialize receive message queue
	can_rx_head = can_rx_tail = can_rx_count = can_rx_lost = 0;

	rcc_reset_dev(RCC_USB);			//! X893
	rcc_clk_disable(RCC_USB);		//! X893
//	line_dtr_rts = 0;				//! X893
	rcc_clk_enable(RCC_AFIO);			// enable clocks for AFIO
	rcc_clk_enable(RCC_CAN);			// and CAN
	rcc_reset_dev(RCC_CAN);				// reset CAN interface

	can_active = 1;						// set CAN active flag (for interrupt handler

	CANx->MCR &= ~CAN_MCR_SLEEP;		// reset CAN sleep mode (default after reset)

	if (can_init_enter(CANx) != CAN_OK)	// enter CAN initialization mode
		return status;					// error, so return

	CANx->MCR &= ~CAN_CONTROL_MASK;		// set mode bits
	CANx->MCR |= (control & CAN_CONTROL_MASK);

	CANx->BTR &= ~CAN_TIMING_MASK;			// Set the bit timing register
	CANx->BTR |= (can_speed_table[speed].btr & CAN_TIMING_MASK);

	nvic_irq_enable(NVIC_USB_LP_CAN_RX0);	// Enable interrupts
    
	nvic_irq_enable(NVIC_USB_HP_CAN_TX);

	CANx->IER = (CAN_IER_FMPIE0 | CAN_IER_FMPIE1 | CAN_IER_TMEIE);

	if (can_init_leave(CANx) == CAN_OK)
	{
		while (!(CANx->TSR & CAN_TSR_TME0));	// Transmit mailbox 0 is empty
		while (!(CANx->TSR & CAN_TSR_TME1));	// Transmit mailbox 0 is empty
		while (!(CANx->TSR & CAN_TSR_TME2));	// Transmit mailbox 0 is empty
	} 
	return status;
}

/**
  * @brief  Set timing calues (CAN_BTR)
  */
CAN_STATUS can_set_timing(CAN_Port* CANx, uint32 timing)
{
	if (can_init_enter(CANx) == CAN_OK)
	{
		CANx->BTR = ((CANx->BTR & ~CAN_TIMING_MASK) | (timing & CAN_TIMING_MASK));
		can_init_leave(CANx);
	}
	return status;
}

/**
  * @brief  Set CAN mode
  * @param CANx pointer to CAN port
  * @param mode CAN mode
  */
CAN_STATUS can_set_mode(CAN_Port* CANx, uint32 mode)
{
	if (can_init_enter(CANx) == CAN_OK)
	{
		CANx->BTR &= ~CAN_MODE_MASK;
		CANx->BTR |= (mode & CAN_MODE_MASK);
		can_init_leave(CANx);
	}
	return status;
}

/**
  * @brief  Set CAN to GPIO mapping
  */
CAN_STATUS can_gpio_map(CAN_Port* CANx, CAN_GPIO_MAP map_mode)
{
	rcc_clk_enable(RCC_AFIO);

	status = CAN_INIT_FAILED;
	if( CANx == CAN1_BASE)
	{
		switch(map_mode)
		{
		case CAN_GPIO_PB8_PB9:
			rcc_clk_enable(RCC_GPIOB);
			afio_remap(AFIO_MAPR_CAN_REMAP_PB8_PB9);
			gpio_set_mode(GPIOB, 8, GPIO_INPUT_FLOATING);
			gpio_set_mode(GPIOB, 9, GPIO_AF_OUTPUT_PP);
			break;
#if NR_GPIO_PORTS >= 4
		case CAN_GPIO_PD0_PD1:
			rcc_clk_enable(RCC_GPIOD);
			afio_remap(AFIO_MAPR_CAN_REMAP_PD0_PD1);
			gpio_set_mode(GPIOD, 0, GPIO_INPUT_FLOATING);
			gpio_set_mode(GPIOD, 1, GPIO_AF_OUTPUT_PP);
			break;
#endif
		default:
			return status;
		}
		status = CAN_OK;
	}
	return status;
}

CAN_STATUS can_filter(CAN_Port* CANx, uint8 filter_idx, CAN_FIFO fifo, CAN_FILTER_SCALE scale, CAN_FILTER_MODE mode, uint32 fr1, uint32 fr2)
{
	uint32 mask = ((uint32)0x00000001) << filter_idx;

	CANx->FMR |= CAN_FMR_FINIT;		// Initialization mode for the filter
	CANx->FA1R &= ~mask;			// Deactivation filter


	if (scale == CAN_FILTER_32BIT)
		CANx->FS1R |= mask;
	else
		CANx->FS1R &= ~mask;

	CANx->sFilterRegister[filter_idx].FR1 = fr1;
	CANx->sFilterRegister[filter_idx].FR2 = fr2;

	if (mode == CAN_FILTER_MASK)
		CANx->FM1R &= ~mask;
	else
		CANx->FM1R |= mask;

	if (fifo == CAN_FIFO0)
		CANx->FFA1R &= ~mask;
	else
		CANx->FFA1R |= mask;

	CANx->FA1R |= mask;
	CANx->FMR &= ~CAN_FMR_FINIT;
	return CAN_OK;
}

/**
  * @brief  Initiates the transmission of a message.
  * @param	CANx: where x can be 1 to select the CAN peripheral.
  * @param	msg: pointer to a structure which contains CAN Id, CAN DLC and CAN datas.
  * @retval : The number of the mailbox that is used for transmission or CAN_NO_MB if there is no empty mailbox.
  */
CAN_TX_MBX can_transmit(CAN_Port* CANx, CanMsg* msg)
{
	CAN_TX_MBX mbx;
	uint32 data;

	/* Select one empty transmit mailbox */
	if (CANx->TSR & CAN_TSR_TME0)
		mbx = CAN_TX_MBX0;
	else if (CANx->TSR & CAN_TSR_TME1)
		mbx = CAN_TX_MBX1;
	else if (CANx->TSR & CAN_TSR_TME2)
		mbx = CAN_TX_MBX2;
	else
	{
		status = CAN_NO_MB;
		return CAN_TX_NO_MBX;
	}

    /* Set up the Id */
    if (msg->IDE == CAN_ID_STD)
		data = (msg->ID << 21);
    else
		data = (msg->ID << 3) | CAN_ID_EXT;

	data |= ((uint32)msg->RTR);

	/* Set up the DLC */
    CANx->sTxMailBox[mbx].TDTR = (uint32)(msg->DLC & 0x0F);

    /* Set up the data field */
    CANx->sTxMailBox[mbx].TDLR = (
		((uint32)msg->Data[3] << 24) | 
		((uint32)msg->Data[2] << 16) |
		((uint32)msg->Data[1] << 8) | 
		((uint32)msg->Data[0])
		);
    CANx->sTxMailBox[mbx].TDHR = (
		((uint32)msg->Data[7] << 24) |
		((uint32)msg->Data[6] << 16) |
		((uint32)msg->Data[5] << 8) |
		((uint32)msg->Data[4])
		);
    /* Request transmission */
    CANx->sTxMailBox[mbx].TIR = (data | CAN_TMIDxR_TXRQ);
	status = CAN_OK;

	return mbx;
}

/**
  * Checks the transmission of a message.
  * @param CANx: where x can be 1 to select the CAN peripheral.
  * @param mbx: the number of the mailbox that is used for transmission.
  * @retval : CAN_TX_OK if the CAN driver transmits the message, CAN_TX_FAILED in an other case.
  */
CAN_STATUS can_tx_status(CAN_Port* CANx, CAN_TX_MBX mbx)
{
	/* RQCP, TXOK and TME bits */
	uint8 state = 0;

	switch (mbx)
	{
	case CAN_TX_MBX0:
		state |= (uint8)((CANx->TSR & CAN_TSR_RQCP0) << 2);
		state |= (uint8)((CANx->TSR & CAN_TSR_TXOK0) >> 0);
		state |= (uint8)((CANx->TSR & CAN_TSR_TME0) >> 26);
		break;
	case CAN_TX_MBX1:
		state |= (uint8)((CANx->TSR & CAN_TSR_RQCP1) >> 6);
		state |= (uint8)((CANx->TSR & CAN_TSR_TXOK1) >> 8);
		state |= (uint8)((CANx->TSR & CAN_TSR_TME1) >> 27);
		break;
	case CAN_TX_MBX2:
		state |= (uint8)((CANx->TSR & CAN_TSR_RQCP2) >> 14);
		state |= (uint8)((CANx->TSR & CAN_TSR_TXOK2) >> 16);
		state |= (uint8)((CANx->TSR & CAN_TSR_TME2) >> 28);
		break;
	default:
		status = CAN_TX_FAILED;
		return status;
	}

	// state = RQCP TXOK TME
	switch (state)
	{
	/* transmit pending  */
	case 0x0:
		status = CAN_TX_PENDING;
		break;
	/* transmit failed  */
	case 0x5:
		status = CAN_TX_FAILED;
		break;
	/* transmit succedeed  */
	case 0x7:
		status = CAN_OK;
		break;
	default:
		status = CAN_TX_FAILED;
		break;
	}
	return status;
}

/**
  * @brief  Cancels a transmit request.
  * @param CANx: where x can be 1 to select the CAN peripheral.
  * @param mbx: Mailbox number.
  * @retval : None.
  */
void can_cancel(CAN_Port* CANx, uint8 mbx)
{
	/* abort transmission */
	switch (mbx)
	{
	case 0:
		CANx->TSR |= CAN_TSR_ABRQ0;
		break;
	case 1:
		CANx->TSR |= CAN_TSR_ABRQ1;
		break;
	case 2:
		CANx->TSR |= CAN_TSR_ABRQ2;
		break;
	default:
		break;
	}
}

void can_rx_queue_clear(void)
{
	nvic_irq_disable(NVIC_USB_LP_CAN_RX0);
	can_rx_head = can_rx_tail = can_rx_count = can_rx_lost = 0;
	nvic_irq_enable(NVIC_USB_LP_CAN_RX0);
}

uint8 can_rx_available(void)
{
	return can_rx_count;
}

CanMsg* can_rx_queue_get(void)
{
	if (can_rx_count == 0)
		return NULL;
	return &(can_rx_queue[can_rx_tail]);
}

void can_rx_queue_free(void)
{
	if (can_rx_count > 0)
	{
		nvic_irq_disable(NVIC_USB_LP_CAN_RX0);			// JMD problème d'atomicité
		can_rx_tail = (can_rx_tail == (CAN_RX_QUEUE_SIZE - 1)) ? 0 : (can_rx_tail + 1);
		--can_rx_count;
		nvic_irq_enable(NVIC_USB_LP_CAN_RX0);			// fin JMD problème d'atomicité
	}
}

CanMsg* can_read(CAN_Port* CANx, CAN_FIFO fifo, CanMsg* msg)
{
	uint32 data = CANx->sFIFOMailBox[fifo].RIR;

	/* Get the Id */
	if (data & CAN_ID_EXT)
	{
		msg->ID = 0x1FFFFFFF & (data >> 3);
		msg->IDE = (uint8)CAN_ID_EXT;
	}
	else
	{
		msg->ID = 0x000007FF & (data >> 21);
		msg->IDE = (uint8)CAN_ID_STD;
	}

	msg->RTR = (uint8)(CAN_RTR_REMOTE & data);
	msg->DLC = (uint8)(0x0F & CANx->sFIFOMailBox[fifo].RDTR);
	msg->FMI = (uint8)(0xFF & (CANx->sFIFOMailBox[fifo].RDTR >> 8));

	/* Get the data field */
	data = CANx->sFIFOMailBox[fifo].RDLR;
	uint8* p = msg->Data;
	*p++ = (uint8)0xFF & data;
	*p++ = (uint8)0xFF & (data >> 8);
	*p++ = (uint8)0xFF & (data >> 16);
	*p++ = (uint8)0xFF & (data >> 24);

	data = CANx->sFIFOMailBox[fifo].RDHR;
	*p++ = (uint8)0xFF & data;
	*p++ = (uint8)0xFF & (data >> 8);
	*p++ = (uint8)0xFF & (data >> 16);
	*p++ = (uint8)0xFF & (data >> 24);

	return msg;
}

void can_rx_release(CAN_Port* CANx, CAN_FIFO fifo)
{
	if (fifo == CAN_FIFO0)
		CANx->RF0R |= (CAN_RF0R_RFOM0);	// Release FIFO0
	else
		CANx->RF1R |= (CAN_RF1R_RFOM1);	// Release FIFO1
}

void can_rx_read(CAN_Port* CANx, CAN_FIFO fifo)
{
	if (can_rx_count < CAN_RX_QUEUE_SIZE)		// read the message
	{
		CanMsg* msg = &can_rx_queue[can_rx_head];
		can_read(CANx, fifo, msg);
		can_rx_head = (can_rx_head == (CAN_RX_QUEUE_SIZE - 1)) ? 0 : (can_rx_head + 1);
		can_rx_count++;
	}
	else
		can_rx_lost = 1;						// no place in queue, ignore package

	can_rx_release(CANx, fifo);
}

uint8 CAN_RX0_IRQ_Handler(void)
{
	if (can_active)
	{
		while ((CAN1_BASE->RF0R & CAN_RF0R_FMP0) != 0)
			can_rx_read(CAN1_BASE, CAN_FIFO0);		// message pending FIFO0
		while ((CAN1_BASE->RF1R & CAN_RF1R_FMP1) != 0)
			can_rx_read(CAN1_BASE, CAN_FIFO1);		// message pending FIFO1
	}
	return can_active;								// return CAN active flag to USB handler
}

void USB_HP_CAN_TX_IRQHandler (void)
{
	if (can_active)
	{
		if (CAN1_BASE->TSR & CAN_TSR_RQCP0)
			CAN1_BASE->TSR |= CAN_TSR_RQCP0;		// reset request complete mbx 0
		if (CAN1_BASE->TSR & CAN_TSR_RQCP1)
			CAN1_BASE->TSR |= CAN_TSR_RQCP1;		// reset request complete mbx 1
		if (CAN1_BASE->TSR & CAN_TSR_RQCP2)
			CAN1_BASE->TSR |= CAN_TSR_RQCP2;		// reset request complete mbx 2
	}
}
