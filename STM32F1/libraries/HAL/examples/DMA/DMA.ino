#include <HAL.h>
#define LED PB1
int c=0;
#define LED_PIN GPIO_PIN_1
#define LED_PORT GPIOB


DMA_HandleTypeDef     DmaHandle;
#define BUFFER_SIZE 32
static const uint32_t aSRC_Const_Buffer[BUFFER_SIZE]= {
                                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};

static uint32_t aDST_Buffer[BUFFER_SIZE];

static void DMA_Config(void)
{   
  /*## -1- Enable DMA2 clock #################################################*/
 // __DMA2_CLK_ENABLE();

  /*##-2- Select the DMA functional Parameters ###############################*/
  DmaHandle.Init.Channel = DMA_CHANNEL;                     /* DMA_CHANNEL_0                    */                     
  DmaHandle.Init.Direction = DMA_MEMORY_TO_MEMORY;          /* M2M transfer mode                */           
  DmaHandle.Init.PeriphInc = DMA_PINC_ENABLE;               /* Peripheral increment mode Enable */                 
  DmaHandle.Init.MemInc = DMA_MINC_ENABLE;                  /* Memory increment mode Enable     */                   
  DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; /* Peripheral data alignment : Word */    
  DmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;    /* memory data alignment : Word     */     
  DmaHandle.Init.Mode = DMA_NORMAL;                         /* Normal DMA mode                  */  
  DmaHandle.Init.Priority = DMA_PRIORITY_HIGH;              /* priority level : high            */  
  DmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;           /* FIFO mode disabled               */        
  DmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;  
  DmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst                     */  
  DmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst                 */
  
  /*##-3- Select the DMA instance to be used for the transfer : DMA2_Stream0 #*/
  DmaHandle.Instance = DMA_STREAM;

  /*##-4- Select Callbacks functions called after Transfer complete and Transfer error */
  DmaHandle.XferCpltCallback  = TransferComplete;
  DmaHandle.XferErrorCallback = TransferError;
  
  /*##-5- Initialize the DMA stream ##########################################*/
  if(HAL_DMA_Init(&DmaHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();  
  }
  
  /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
  /* Set Interrupt Group Priority */ 
  HAL_NVIC_SetPriority(DMA_STREAM_IRQ, 0, 0);
  
  /* Enable the DMA STREAM global Interrupt */
  HAL_NVIC_EnableIRQ(DMA_STREAM_IRQ);

  /*##-7- Start the DMA transfer using the interrupt mode ####################*/
  /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
  /* Enable All the DMA interrupts */
  if(HAL_DMA_Start_IT(&DmaHandle, (uint32_t)&aSRC_Const_Buffer, (uint32_t)&aDST_Buffer, BUFFER_SIZE) != HAL_OK)
  {
    /* Transfer Error */
    Error_Handler();  
  }           
}

static void Error_Handler()
{
	Serial.println("Error");
}
/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt 
  *         is generated
  * @retval None
  */
static void TransferComplete(DMA_HandleTypeDef *DmaHandle)
{
  /* Turn LED1 on: Transfer correct */
  Serial.println("TransferComplete");
}

/**
  * @brief  DMA conversion error callback
  * @note   This function is executed when the transfer error interrupt 
  *         is generated during DMA transfer
  * @retval None
  */
static void TransferError(DMA_HandleTypeDef *DmaHandle)
{
  /* Turn LED2 on: Transfer Error */
  Serial.println("TransferError");
}



void setup() 
{
//  pinMode(LED,OUTPUT);
  
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitStruct.Pin = LED_PIN;	
 	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 	GPIO_InitStruct.Pull = GPIO_NOPULL;	
	
  	HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct); 
	
	HAL_GPIO_WritePin(LED_PORT,GPIO_PIN_1,GPIO_PIN_RESET);// Turn off the LED
  
	Serial.begin(115200);
	
	
	DMA_Config(); 	
}


void loop()
{
	Serial.println(c++);

	HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_SET);

	delay(500);

	HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_RESET);

	delay(100);

	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);

	delay(1000);

	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);

	delay(250);
	
}
