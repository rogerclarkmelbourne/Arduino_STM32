/**
 *******************************************************************************
 * @file       time.c
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      time management implementation code of CooCox CoOS kernel.	
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

#if CFG_TASK_WAITTING_EN > 0

/*---------------------------- Variable Define -------------------------------*/
P_OSTCB DlyList   = Co_NULL;               /*!< Header pointer to the DELAY list.*/


/**
 *******************************************************************************
 * @brief      Insert into DELAY list			 
 *   
 * @param[in]  ptcb    Task that want to insert into DELAY list. 
 * @param[in]  ticks   Delay system ticks.	 
 * @param[out] None   
 * @retval     None.	 	 
 *
 * @par Description
 * @details    This function is called to insert task into DELAY list.
 *******************************************************************************
 */
void InsertDelayList(P_OSTCB ptcb,U32 ticks)
{
    S32 deltaTicks;
    P_OSTCB dlyNext;
    
    if(ticks == 0)                      /* Is delay tick == 0?                */
        return;                         /* Yes,do nothing,return              */
    if(DlyList == Co_NULL)                 /* Is no item in DELAY list?          */
    {
        ptcb->delayTick = ticks;        /* Yes,set this as first item         */
        DlyList         = ptcb;
    }
    else
    {	
        /* No,find correct place ,and insert the task */
        dlyNext    = DlyList; 
        deltaTicks = ticks;             /* Get task delay ticks               */
        
        /* Find correct place */
        while(dlyNext != Co_NULL)
        {		
            /* Get delta ticks with previous item */ 
            deltaTicks -= dlyNext->delayTick;  
            if(deltaTicks < 0)          /* Is delta ticks<0?                  */
            {	  
                /* Yes,get correct place */
                if(dlyNext->TCBprev != Co_NULL)   /* Is head item of DELAY list? */
                {							   
                    dlyNext->TCBprev->TCBnext = ptcb;   /* No,insert into     */ 
                    ptcb->TCBprev             = dlyNext->TCBprev;
                    ptcb->TCBnext             = dlyNext;
                    dlyNext->TCBprev          = ptcb;
                }
                else                    /* Yes,set task as first item         */
                {							   
                    ptcb->TCBnext    = DlyList;
                    DlyList->TCBprev = ptcb;
                    DlyList          = ptcb;
                }
                ptcb->delayTick           = ptcb->TCBnext->delayTick+deltaTicks;
                ptcb->TCBnext->delayTick -= ptcb->delayTick; 
                break;
            }
            /* Is last item in DELAY list? */
            else if((deltaTicks >= 0) && (dlyNext->TCBnext == Co_NULL) )
            {								   
                ptcb->TCBprev    = dlyNext; /* Yes,insert into                */
                dlyNext->TCBnext = ptcb;	
                ptcb->delayTick  = deltaTicks;
                break;
            }
            dlyNext = dlyNext->TCBnext; /* Get the next item in DELAY list    */
        }
    }

    ptcb->state  = TASK_WAITING;        /* Set task status as TASK_WAITING    */
    TaskSchedReq = Co_TRUE;
}


/**
 *******************************************************************************
 * @brief      Remove from the DELAY list			  
 * @param[in]  ptcb   Task that want to remove from the DELAY list. 
 * @param[out] None   
 * @retval     None	 	 
 *
 * @par Description
 * @details    This function is called to remove task from the DELAY list.
 *******************************************************************************
 */
void RemoveDelayList(P_OSTCB ptcb)
{
    
    /* Is there only one item in the DELAY list?   */
    if((ptcb->TCBprev == Co_NULL) && ( ptcb->TCBnext == Co_NULL))
    {
        DlyList = Co_NULL;	                /* Yes,set DELAY list as Co_NULL         */
    }
    else if(ptcb->TCBprev == Co_NULL)      /* Is the first item in DELAY list?   */
    {   
	    /* Yes,remove task from the DELAY list,and reset the list             */
        DlyList	                  = ptcb->TCBnext;
        ptcb->TCBnext->delayTick += ptcb->delayTick;
        ptcb->TCBnext->TCBprev    = Co_NULL;
        ptcb->TCBnext             = Co_NULL;
        
    }
    else if(ptcb->TCBnext == Co_NULL)      /* Is the last item in DELAY list?    */
    {									
        ptcb->TCBprev->TCBnext = Co_NULL;  /* Yes,remove task form DELAY list    */
        ptcb->TCBprev          = Co_NULL;
    }
    else                                /* No, remove task from DELAY list    */
    {									
        ptcb->TCBprev->TCBnext    = ptcb->TCBnext;	
        ptcb->TCBnext->TCBprev    = ptcb->TCBprev;	
        ptcb->TCBnext->delayTick += ptcb->delayTick;
        ptcb->TCBnext     	      = Co_NULL;
        ptcb->TCBprev             = Co_NULL;
    }
    ptcb->delayTick = INVALID_VALUE;  /* Set task delay tick value as invalid */		
}

/**
 *******************************************************************************
 * @brief      Get current ticks			 
 * @param[in]  None	 
 * @param[out] None   
 * @retval     Return current system tick counter.	 	 
 *
 * @par Description
 * @details    This function is called to obtain current system tick counter.
 *******************************************************************************
 */
U64 CoGetOSTime(void)
{
    return OSTickCnt;                   /* Get system time(tick)              */
}


/**
 *******************************************************************************
 * @brief      Delay current task for specify ticks number	  
 * @param[in]  ticks    Specify system tick number which will delay.			 
 * @param[out] None  
 * @retval     E_CALL   Error call in ISR.
 * @retval     E_OK     The current task was insert to DELAY list successful,it
 *                      will delay specify time.		 
 * @par Description
 * @details    This function delay specify ticks for current task.
 *
 * @note       This function be called in ISR,do nothing and return immediately.
 *******************************************************************************	
 */
StatusType CoTickDelay(U32 ticks)
{
    if(OSIntNesting >0)	                /* Is call in ISR?                    */
    {
        return E_CALL;                  /* Yes,error return                   */
    }
    
    if(ticks == INVALID_VALUE)          /* Is tick==INVALID_VALUE?            */
    {
        return E_INVALID_PARAMETER;     /* Yes,error return                   */
    }	
    if(ticks == 0)                      /* Is tick==0?                        */
    {
        return E_OK;                    /* Yes,do nothing ,return OK          */
    }
    if(OSSchedLock != 0)                /* Is OS lock?                        */
    {
        return E_OS_IN_LOCK;            /* Yes,error return                   */
    }	
    OsSchedLock();                      /* Lock schedule                      */
    InsertDelayList(TCBRunning,ticks);	/* Insert task in DELAY list          */
    OsSchedUnlock();                /* Unlock schedule,and call task schedule */
    return E_OK;                        /* Return OK                          */
}


/**
 *******************************************************************************
 * @brief      Reset task delay ticks	 
 * @param[in]  ptcb    Task that want to insert into DELAY list.
 * @param[in]  ticks   Specify system tick number which will delay .			 
 * @param[out] None  
 * @retval     E_CALL               Error call in ISR.
 * @retval     E_INVALID_ID         Invalid task id.
 * @retval     E_NOT_IN_DELAY_LIST  Task not in delay list.
 * @retval     E_OK                 The current task was inserted to DELAY list 
 *                                  successful,it will delay for specify time.		 
 * @par Description
 * @details    This function delay specify ticks for current task.
 ******************************************************************************* 	
 */
StatusType CoResetTaskDelayTick(OS_TID taskID,U32 ticks)
{
    P_OSTCB ptcb;
	

#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(taskID >= CFG_MAX_USER_TASKS + SYS_TASK_NUM)
    {
        return E_INVALID_ID;
    }
#endif

	ptcb = &TCBTbl[taskID];
#if CFG_PAR_CHECKOUT_EN >0 
    if(ptcb->stkPtr == Co_NULL)
    {
        return E_INVALID_ID;
    }
#endif

    if(ptcb->delayTick == INVALID_VALUE)  /* Is tick==INVALID_VALUE?          */
    {
        return E_NOT_IN_DELAY_LIST;       /* Yes,error return                 */
    }
    OsSchedLock();                        /* Lock schedule                    */
    RemoveDelayList(ptcb);                /* Remove task from the DELAY list  */
    
    if(ticks == 0)                        /* Is delay tick==0?                */
    {
        InsertToTCBRdyList(ptcb);         /* Insert task into the DELAY list  */
    }
    else
    {									
        InsertDelayList(ptcb,ticks);      /* No,insert task into DELAY list   */
    }
    OsSchedUnlock();                /* Unlock schedule,and call task schedule */
    return E_OK;                          /* Return OK                        */
}


/**
 *******************************************************************************
 * @brief      Delay current task for detail time	   
 * @param[in]  hour      Specify the number of hours.
 * @param[in]  minute    Specify the number of minutes.
 * @param[in]  sec       Specify the number of seconds.
 * @param[in]  millsec   Specify the number of millseconds.	 
 * @param[out] None  
 * @retval     E_CALL               Error call in ISR.
 * @retval     E_INVALID_PARAMETER  Parameter passed was invalid,delay fail.
 * @retval     E_OK                 The current task was inserted to DELAY list
 *                                  successful,it will delay for specify time.							 
 * @par Description
 * @details    This function delay specify time for current task.	 
 *
 * @note       If this function called in ISR,do nothing and return immediately.
 *******************************************************************************
 */
#if CFG_TIME_DELAY_EN >0
StatusType  CoTimeDelay(U8 hour,U8 minute,U8 sec,U16 millsec)
{
    U32	ticks;
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(OSIntNesting > 0)
    {
        return E_CALL;
    }
    if((minute > 59)||(sec > 59)||(millsec > 999))
        return E_INVALID_PARAMETER;
#endif
    if(OSSchedLock != 0)                /* Is OS lock?                        */
    {
        return E_OS_IN_LOCK;            /* Yes,error return                   */
    }	
    
    /* Get tick counter from time */
    ticks = ((hour*3600) + (minute*60) + (sec)) * (CFG_SYSTICK_FREQ)\
            + (millsec*CFG_SYSTICK_FREQ + 500)/1000;
    
    CoTickDelay(ticks);                 /* Call tick delay                    */
    return E_OK;                        /* Return OK                          */
}
#endif




/**
 *******************************************************************************
 * @brief      Dispose time delay	 
 * @param[in]  None	 
 * @param[out] None 
 * @retval     None 
 *
 * @par Description
 * @details    This function is called to dispose time delay of all task.  
 *******************************************************************************
 */
void TimeDispose(void)
{  
    P_OSTCB	dlyList;
    
    dlyList = DlyList;                  /* Get first item of DELAY list       */
    while((dlyList != Co_NULL) && (dlyList->delayTick == 0) )
    {	
    
#if CFG_EVENT_EN > 0
        if(dlyList->eventID != INVALID_ID) /* Is task in event waiting list?  */
        {								   
            RemoveEventWaittingList(dlyList); /* Yes,remove task from list    */	
        }
#endif

#if CFG_FLAG_EN  > 0
        if(dlyList->pnode != Co_NULL)          /* Is task in flag waiting list?  */
        {
            RemoveLinkNode(dlyList->pnode); /* Yes,remove task from list      */	
        }
#endif
        dlyList->delayTick = INVALID_VALUE; /* Set delay tick value as invalid*/
        DlyList = dlyList->TCBnext; /* Get next item as the head of DELAY list*/
        dlyList->TCBnext   = Co_NULL;

		InsertToTCBRdyList(dlyList);        /* Insert task into READY list    */
        
        dlyList = DlyList;                /* Get the first item of DELAY list */
        if(dlyList != Co_NULL)                 /* Is DELAY list as Co_NULL?         */
        {
            dlyList->TCBprev = Co_NULL;        /* No,initialize the first item   */
        }
    }
}


/**
 *******************************************************************************
 * @brief      Dispose time delay in ISR	  
 * @param[in]  None	 
 * @param[out] None 
 * @retval     None 
 *
 * @par Description
 * @details    This function is called in systick interrupt to dispose time delay   
 *             of all task.
 *******************************************************************************
 */
void isr_TimeDispose(void)
{
    if(OSSchedLock > 1)                 /* Is schedule lock?                  */
    {
        IsrReq = Co_TRUE;
        TimeReq = Co_TRUE;                 /* Yes,set time request Co_TRUE          */
    }
    else
    {
        TimeDispose();                  /* No,call handler                    */
    }
}


#endif
