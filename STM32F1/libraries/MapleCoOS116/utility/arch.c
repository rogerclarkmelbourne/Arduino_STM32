/**
 *******************************************************************************
 * @file      arch.c
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief     This file provides InitTaskContext() and SysTick_Handler().
 *******************************************************************************
 * @copy
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met: 
 *  
 *      * Redistributions of source code must retain the above copyright 
 *  notice, this list of conditions and the following disclaimer. 
 *      * Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution. 
 *      * Neither the name of the <ORGANIZATION> nor the names of its 
 *  contributors may be used to endorse or promote products derived 
 *  from this software without specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * <h2><center>&copy; COPYRIGHT 2014 CooCox </center></h2>
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
#if CFG_CHIP_TYPE == 3
	context      = context - 18;
#endif
    *(context--) = (U32)0x01000000L;      /* xPSR	        */
	*(context--) = (U32)task;             /* Entry point of task.                         */
	*(context)   = (U32)0xFFFFFFFEL;
    context      = context - 5;
	*(context)   = (U32)param;            /* R0: argument */
	context      = context - 8;
#if CFG_CHIP_TYPE == 3
	context      = context - 16;
#endif
//    *(--context) = 0xfffffffd;          /* LR */
  	
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
    if(DlyList != Co_NULL)             /* Have task in delay list?               */
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
    if(TmrList != Co_NULL)             /* Have timer in working?                 */
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
	TaskSchedReq = Co_TRUE;
    OsSchedUnlock();
}
