/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-July-2011
  * @brief   Generic media access Layer.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#pragma     data_alignment = 4
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_vcp.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
LINE_CODING linecoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };


//USART_InitTypeDef USART_InitStructure;

/* These are external variables imported from CDC core to be used for IN
   transfer management. */
extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern volatile int APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */
extern volatile int APP_Rx_ptr_out;

#define UsbRecBufferSize 2048
uint8_t UsbRecBuffer[UsbRecBufferSize];
volatile int UsbRecRead = 0;
volatile int UsbRecWrite = 0;
volatile int VCP_DTRHIGH = 0;
uint8_t UsbTXBlock = 1;

uint32_t VCPBytesAvailable(void) {
	return (UsbRecWrite - UsbRecRead + UsbRecBufferSize) % UsbRecBufferSize;
}

uint8_t VCPGetByte(void) {
	if(UsbRecWrite == UsbRecRead) {
		return 0;
	} else {
		uint8_t c = UsbRecBuffer[UsbRecRead++];
		if(UsbRecRead == UsbRecBufferSize) {
			UsbRecRead = 0;
		}
		return c;
	}
}

/* Private function prototypes -----------------------------------------------*/
static uint16_t VCP_Init     (void);
static uint16_t VCP_DeInit   (void);
static uint16_t VCP_Ctrl     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
uint16_t VCP_DataTx   (uint8_t* Buf, uint32_t Len);
static uint16_t VCP_DataRx   (uint8_t* Buf, uint32_t Len);

static uint16_t VCP_COMConfig(uint8_t Conf);

CDC_IF_Prop_TypeDef VCP_fops =
{
  VCP_Init,
  VCP_DeInit,
  VCP_Ctrl,
  VCP_DataTx,
  VCP_DataRx
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  VCP_Init
  *         Initializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Init(void)
{
  return USBD_OK;
}

/**
  * @brief  VCP_DeInit
  *         DeInitializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_DeInit(void)
{

  return USBD_OK;
}

/**
  * @brief  VCP_SetUSBTxBlocking
  *         Set USB blocking mode for output buffer overrun
  * @param  Mode: 0: non blocking, 1: blocking
  * @retval None
  */
void VCP_SetUSBTxBlocking(uint8_t Mode)
{
	UsbTXBlock = Mode;
}

/**
  * @brief  VCP_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{
  switch (Cmd)
  {
  case SEND_ENCAPSULATED_COMMAND:
    /* Not  needed for this driver */
    break;

  case GET_ENCAPSULATED_RESPONSE:
    /* Not  needed for this driver */
    break;

  case SET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case GET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case CLEAR_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case SET_LINE_CODING:
    linecoding.bitrate = (uint32_t)(Buf[0] | (Buf[1] << 8) | (Buf[2] << 16) | (Buf[3] << 24));
    linecoding.format = Buf[4];
    linecoding.paritytype = Buf[5];
    linecoding.datatype = Buf[6];
    /* Set the new configuration */
    VCP_COMConfig(OTHER_CONFIG);
    break;

  case GET_LINE_CODING:
    Buf[0] = (uint8_t)(linecoding.bitrate);
    Buf[1] = (uint8_t)(linecoding.bitrate >> 8);
    Buf[2] = (uint8_t)(linecoding.bitrate >> 16);
    Buf[3] = (uint8_t)(linecoding.bitrate >> 24);
    Buf[4] = linecoding.format;
    Buf[5] = linecoding.paritytype;
    Buf[6] = linecoding.datatype;
    break;

  case SET_CONTROL_LINE_STATE:
	linecoding.bitrate = (uint32_t)(Buf[0] | (Buf[1] << 8));
	if(Buf[0] & 1) {
		VCP_DTRHIGH = 1;
	}
    /* Not  needed for this driver */
    break;

  case SEND_BREAK:
    /* Not  needed for this driver */
    break;

  default:
    break;
  }

  return USBD_OK;
}

/**
  * @brief  VCP_DataTx
  *         CDC received data to be send over USB IN endpoint are managed in
  *         this function.
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else VCP_FAIL
  */
uint16_t VCP_DataTx (uint8_t* Buf, uint32_t Len)
{
	while(Len-- > 0) {
		if(UsbTXBlock) {
			while ((APP_Rx_ptr_in - APP_Rx_ptr_out + APP_RX_DATA_SIZE) % APP_RX_DATA_SIZE + 1 >= APP_RX_DATA_SIZE)
				;
		} else {
			if ((APP_Rx_ptr_in - APP_Rx_ptr_out + APP_RX_DATA_SIZE) % APP_RX_DATA_SIZE + 1 >= APP_RX_DATA_SIZE) {
				return USBD_BUSY;
			}
		}


		APP_Rx_Buffer[APP_Rx_ptr_in++] = *Buf++;
		  /* To avoid buffer overflow */
		  if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
		  {
		    APP_Rx_ptr_in = 0;
		  }
	}
	return USBD_OK;
}

typedef volatile unsigned long      vu32;

typedef struct {
  vu32 CPUID;
  vu32 ICSR;
  vu32 VTOR;
  vu32 AIRCR;
  vu32 SCR;
  vu32 CCR;
  vu32 SHPR[3];
  vu32 SHCSR;
  vu32 CFSR;
  vu32 HFSR;
  vu32 DFSR;
  vu32 MMFAR;
  vu32 BFAR;
  vu32 AFSR;
} SCB_TypeDef;

#define AIRCR_RESET         0x05FA0000
#define AIRCR_RESET_REQ     (AIRCR_RESET | (u32)0x04);
#define SCS_BASE   ((u32)0xE000E000)
#define SCB_BASE   (SCS_BASE + 0x0D00)

void systemHardReset(void) {
    SCB_TypeDef* rSCB = (SCB_TypeDef *) SCB_BASE;
    //typedef void (*funcPtr)(void); // not used

    /* Reset */
    rSCB->AIRCR = (u32)AIRCR_RESET_REQ;

    /* Should never get here */
    while (1) {
        asm volatile("nop");
    }
}

/**
  * @brief  VCP_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will block any OUT packet reception on USB endpoint
  *         until exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result
  *         in receiving more data while previous ones are still not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else VCP_FAIL
  */
static uint16_t VCP_DataRx (uint8_t* Buf, uint32_t Len)
{
	if(VCP_DTRHIGH) {
		if(Len >= 4) {
			if(Buf[0] == '1' && Buf[1] == 'E' && Buf[2] == 'A' && Buf[3] == 'F') {
				Len = 0;
				*(int*)0x20000BFC = 0x4AFC6BB2;
				systemHardReset();
			}
		}
	}
	VCP_DTRHIGH = 0;
	while(Len-- > 0) {
		UsbRecBuffer[UsbRecWrite] = *Buf++;
		if(UsbRecWrite == UsbRecBufferSize) {
			UsbRecWrite = 0;
		} else {
			UsbRecWrite ++;
		}
	}

  return USBD_OK;
}

/**
  * @brief  VCP_COMConfig
  *         Configure the COM Port with default values or values received from host.
  * @param  Conf: can be DEFAULT_CONFIG to set the default configuration or OTHER_CONFIG
  *         to set a configuration received from the host.
  * @retval None.
  */
static uint16_t VCP_COMConfig(uint8_t Conf)
{
#if 0
  if (Conf == DEFAULT_CONFIG)
  {
    /* EVAL_COM1 default configuration */
    /* EVAL_COM1 configured as follow:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - Parity Odd
    - Hardware flow control disabled
    - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_Odd;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure and enable the USART */
    STM_EVAL_COMInit(COM1, &USART_InitStructure);

    /* Enable the USART Receive interrupt */
    USART_ITConfig(EVAL_COM1, USART_IT_RXNE, ENABLE);
  }
  else
  {
    /* set the Stop bit*/
    switch (linecoding.format)
    {
    case 0:
      USART_InitStructure.USART_StopBits = USART_StopBits_1;
      break;
    case 1:
      USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
      break;
    case 2:
      USART_InitStructure.USART_StopBits = USART_StopBits_2;
      break;
    default :
      VCP_COMConfig(DEFAULT_CONFIG);
      return (USBD_FAIL);
    }

    /* set the parity bit*/
    switch (linecoding.paritytype)
    {
    case 0:
      USART_InitStructure.USART_Parity = USART_Parity_No;
      break;
    case 1:
      USART_InitStructure.USART_Parity = USART_Parity_Even;
      break;
    case 2:
      USART_InitStructure.USART_Parity = USART_Parity_Odd;
      break;
    default :
      VCP_COMConfig(DEFAULT_CONFIG);
      return (USBD_FAIL);
    }

    /*set the data type : only 8bits and 9bits is supported */
    switch (linecoding.datatype)
    {
    case 0x07:
      /* With this configuration a parity (Even or Odd) should be set */
      USART_InitStructure.USART_WordLength = USART_WordLength_8b;
      break;
    case 0x08:
      if (USART_InitStructure.USART_Parity == USART_Parity_No)
      {
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
      }
      else
      {
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
      }

      break;
    default :
      VCP_COMConfig(DEFAULT_CONFIG);
      return (USBD_FAIL);
    }

    USART_InitStructure.USART_BaudRate = linecoding.bitrate;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure and enable the USART */
    STM_EVAL_COMInit(COM1, &USART_InitStructure);
  }
#endif
  return USBD_OK;
}

/**
  * @brief  EVAL_COM_IRQHandler
  *
  * @param  None.
  * @retval None.
  */
void EVAL_COM_IRQHandler(void)
{
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
