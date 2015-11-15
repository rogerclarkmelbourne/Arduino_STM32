 /**
 *******************************************************************************
 * @file       core.c
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Core implementation code of CooCox CoOS kernel.	
 *******************************************************************************
 * @copy
 *
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


/*---------------------------- Variable Define -------------------------------*/
volatile U8     OSIntNesting  = 0;         /*!< Use to indicate interrupt nesting level*/
volatile U8     OSSchedLock   = 0;         /*!< Task Switch lock.                      */
volatile BOOL   TaskSchedReq  = Co_FALSE;


/**
 *******************************************************************************
 * @brief      Enter a ISR.						   
 * @param[in]  None	 
 * @param[out] None   
 * @retval     None	 
 *
 * @par Description
 * @details    This function is called to notify OS when enter to an ISR.
 *
 * @note       When you call API in ISR,you must call CoEnterISR() before your
 *             interrupt handler code,and call CoExitISR() after your handler
 *             code and before exiting from ISR.	 
 *******************************************************************************
 */
void CoEnterISR(void)
{
    Inc8(&OSIntNesting);                /* OSIntNesting increment             */
}


/**
 *******************************************************************************
 * @brief      Exit a ISR.	 
 * @param[in]  None	 
 * @param[out] None   
 * @retval     None		 
 *
 * @par Description
 * @details    This function is called when exit from a ISR.	  	
 *
 * @note 
 *******************************************************************************
 */
void CoExitISR(void)
{
    Dec8(&OSIntNesting);                /* OSIntNesting decrease              */
    if( OSIntNesting == 0)              /* Is OSIntNesting == 0?              */
    {
        if(TaskSchedReq == Co_TRUE)
        {
			OSSchedLock++;
            Schedule();                 /* Call task schedule                 */
			OSSchedLock--;
        }
    }
}



/**
 *******************************************************************************
 * @brief      Unlock schedule 	  
 * @param[in]  None		 
 * @param[out] None   
 * @retval     None		 
 *
 * @par Description
 * @details   This function is called to unlock schedule(i.e.enable schedule again) 		 
 *
 * @note 
 *******************************************************************************
 */
void OsSchedUnlock(void)
{
    if(OSSchedLock == 1)                /* Is OSSchedLock == 0?               */
    {
#if CFG_TASK_WAITTING_EN > 0
        if(IsrReq == Co_TRUE)
        {
            RespondSRQ();               /* Respond service request            */	
        }
#endif
        /* Judge task state change or higher PRI task coming in               */
        if(TaskSchedReq == Co_TRUE)
        {
            Schedule();                 /* Call task schedule                 */
        }
		OSSchedLock = 0;
    }
	else
	{
		OSSchedLock--; 	
	}
}


/**
 *******************************************************************************
 * @brief      Lock schedule 	 
 * @param[in]  None		 
 * @param[out] None   
 * @retval     None		 
 *
 * @par Description
 * @details    This function is called in application code to lock schedule.		 
 *
 * @note 
 *******************************************************************************
 */
void CoSchedLock(void)
{									    
    OsSchedLock();                      /* Lock schedule                      */
}


/**
 *******************************************************************************
 * @brief      Unlock schedule 	  
 * @param[in]  None		 
 * @param[out] None   
 * @retval     None		 
 *
 * @par Description
 * @details    This function is called in APP to unlock schedule.		 
 *
 * @note 
 *******************************************************************************
 */
void CoSchedUnlock(void)
{
    OsSchedUnlock();                    /* Unlock schedule                    */
}


/**
 *******************************************************************************
 * @brief      Initialize OS	  
 * @param[in]  None 	 
 * @param[out] None 
 * @retval     None 
 *
 * @par Description
 * @details   This function is called to initialize OS.
 *
 * @note      You must call this function first,before any other OS API function
 *					
 * @code      There is a example for useage of this function,as follows: 
 *        e.g.															 
 *            ...                   // Your target initial code. 
 *				
 *            OsInit();             // Initial OS. 				
 *            CreateTask(...);      // Create tasks.				
 *            ...
 *            OsStart();            // Start multitask.
 * @endcode	
 *******************************************************************************		
 */
void CoInitOS(void)
{
    InitSysTick();                /* Initialize system tick.                  */
    InitInt();                    /* Initialize PendSV,SVC,SysTick interrupt  */	
    CreateTCBList();              /* Create TCB list.                         */   
#if CFG_EVENT_EN > 0				    
    CreateEventList();            /* Create event control list.               */
#endif  
#if CFG_KHEAP_EN > 0
    CoCreateKheap();              /* Create kernel heap within user define    */
#endif   
    OsSchedLock();                /* Lock Schedule                            */ 
                                  /* Create first task -- IDLE task.          */ 
    CoCreateTask(                      CoIdleTask,
                                             Co_NULL,
                                  CFG_LOWEST_PRIO,
                 &idle_stk[CFG_IDLE_STACK_SIZE-1],
                              CFG_IDLE_STACK_SIZE
                 );
				                  /* Set PSP for CoIdleTask coming in */ 
	SetEnvironment(&idle_stk[CFG_IDLE_STACK_SIZE-1]);
}


/**
 *******************************************************************************
 * @brief      Start multitask	 
 * @param[in]  None 	 
 * @param[out] None 	 
 * @retval     None	 
 *
 * @par Description
 * @details    This function is called to start multitask.After it is called,
 *             OS start schedule task by priority or/and time slice.	
 * @note       This function must be called to start OS when you use CoOS,and must
 *             call after CoOsInit().
 *******************************************************************************
 */
void CoStartOS(void)
{
    TCBRunning  = &TCBTbl[0];           /* Get running task                     */
    TCBNext     = TCBRunning;           /* Set next scheduled task as running task */
    TCBRunning->state = TASK_RUNNING;   /* Set running task status to RUNNING   */
    RemoveFromTCBRdyList(TCBRunning);   /* Remove running task from READY list  */
    OsSchedUnlock();					/* Enable Schedule,call task schedule   */
}


/**
 *******************************************************************************
 * @brief      Get OS version	   
 * @param[in]  None	 
 * @param[out] None  
 * @retval     The value is version of OS mutipled by 100.		 
 *
 * @par Description
 * @details    This function is used to return the version number of CooCox OS.
 *             the return value corresponds to CooCox's version number multiplied
 *             by 100. In other words, version 1.02 would be returned as 102.         
 *******************************************************************************
 */
OS_VER CoGetOSVersion(void)
{
    return OS_VERSION;                  /* Get CooCox CoOS version            */
}

