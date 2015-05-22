/**
 *******************************************************************************
 * @file     cpu.c
 * @version    V1.12    
 * @date       2010.03.01
 * @brief    This file provides InitTaskContext() and SysTick_Handler().
 *******************************************************************************
 * @copy
 *	WRITE COPY INFORMATION USE CAPITAL LETTER
 *
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 

/*---------------------------- Include ---------------------------------------*/
#include <coocox.h>
U64     OSTickCnt = 0;                  /*!< Current system tick counter      */ 																			 

/**
 ******************************************************************************
 * @brief      Initial task context	  
 * @param[in]  task    Entry point of task.
 * @param[in]  param   The parameter pass to task.	
 * @param[in]  pstk    The pointer to stack top.	 
 * @param[out] None  
 * @retval     Returns location of new stack top.		 
 *
 * @par Description
 * @details    This function is called to initialize the stack frame of the 
 *             task being created.
 ******************************************************************************
 */
OS_STK *InitTaskContext(FUNCPtr task,void *param,OS_STK *pstk)
{
    OS_STK *context;
	context  = pstk;
    *(context--) = (U32)0x01000000L;      /* xPSR	        */
	*(context--) = (U32)task;             /* Entry point of task.                         */
	*(context)   = (U32)0xFFFFFFFEL;
    context      = context - 5;
	*(context)   = (U32)param;            /* R0: argument */
	context      = context - 8;
  	
    return (context);                   /* Returns location of new stack top. */
}


 
/**
 *******************************************************************************
 * @brief      System tick interrupt handler.			 
 * @param[in]  None	 
 * @param[out] None  	 
 * @retval     None
 *		 
 * @par Description
 * @details    This is system tick interrupt headler.		 
 * @note       CoOS may schedule when exiting this ISR. 
 *******************************************************************************
 */ 
void CoSysTick_Handler(void)
{
    OSSchedLock++;                  /* Lock scheduler.                        */
    OSTickCnt++;                    /* Increment systerm time.                */
#if CFG_TASK_WAITTING_EN >0    
    if(DlyList != NULL)             /* Have task in delay list?               */
    {
        if(DlyList->delayTick > 1)  /* Delay time > 1?                        */
        {
			DlyList->delayTick--;   /* Decrease delay time of the list head.  */         
        }
		else
		{
			DlyList->delayTick = 0;
			isr_TimeDispose();       /* Call hander for delay time list        */
		}
    }
#endif
    
#if CFG_TMR_EN > 0	
    if(TmrList != NULL)             /* Have timer in working?                 */
    {
        if(TmrList->tmrCnt > 1)     /* Timer time > 1?                        */
        {
			TmrList->tmrCnt--;      /* Decrease timer time of the list head.  */        
        }
		else
		{
			TmrList->tmrCnt = 0;
			isr_TmrDispose();         /* Call hander for timer list             */
		}
    }	
#endif
	TaskSchedReq = TRUE;
    OsSchedUnlock();
}
