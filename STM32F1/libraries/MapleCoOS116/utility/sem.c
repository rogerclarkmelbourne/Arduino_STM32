/**
 *******************************************************************************
 * @file       sem.c
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Semaphore management implementation code of CooCox CoOS kernel.	
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

#if CFG_SEM_EN >0

/**
 *******************************************************************************
 * @brief      Create a semaphore	  
 * @param[in]  initCnt   Semaphore valid counter.
 * @param[in]  maxCnt    Semaphore max initialize counter.
 * @param[in]  sortType  Semaphore sort type.		 
 * @param[out] None
 * @retval     E_CREATE_FAIL   Create semaphore fail.
 * @retval     others          Create semaphore successful.
 *
 * @par Description
 * @details    This function is called to create a semaphore. 
 *******************************************************************************
 */
OS_EventID CoCreateSem(U16 initCnt,U16 maxCnt,U8 sortType)
{
    P_ECB pecb;
#if CFG_PAR_CHECKOUT_EN >0
    if(initCnt > maxCnt)    
    {
        return E_CREATE_FAIL;           /* Invalid 'initCnt' or 'maxCnt'      */	
    }
    
    if ((sortType != EVENT_SORT_TYPE_FIFO) && (sortType != EVENT_SORT_TYPE_PRIO))
    {
        return E_CREATE_FAIL;           /* Illegal sort type,return error     */
    }
#endif	
    
    /* Create a semaphore type event control block                            */
    pecb = CreatEvent(EVENT_TYPE_SEM,sortType,Co_NULL);
    if(pecb == Co_NULL)                    /* If failed to create event block    */
    {
        return E_CREATE_FAIL;
    }
    pecb->eventCounter        = initCnt;/* Initialize event block             */
    pecb->initialEventCounter = maxCnt;
    return (pecb->id);                  /* Return event id                    */
}

 
/**
 *******************************************************************************
 * @brief      Delete a semaphore	   
 * @param[in]  id    Event ID which to be deleted.
 * @param[in]  opt   Delete option.	 
 * @arg        == OPT_DEL_ANYWAY    Delete semaphore always   
 * @arg        == OPT_DEL_NO_PEND	Delete semaphore only when no task pending on.
 * @param[out] None   
 * @retval     E_INVALID_ID         Invalid event ID.
 * @retval     E_INVALID_PARAMETER  Invalid parameter.
 * @retval     E_TASK_WAITTING      Tasks waitting for the event,delete fail.
 * @retval     E_OK                 Event deleted successful. 	 
 *
 * @par Description
 * @details    This function is called to delete a semaphore. 
 *
 * @note 
 *******************************************************************************
 */
StatusType CoDelSem(OS_EventID id,U8 opt)
{
    P_ECB pecb;

#if CFG_PAR_CHECKOUT_EN >0
    if(id >= CFG_MAX_EVENT)	                 
    {
        return E_INVALID_ID;
    }
#endif

    pecb = &EventTbl[id];

#if CFG_PAR_CHECKOUT_EN >0
    if(pecb->eventType != EVENT_TYPE_SEM)  
    {
        return E_INVALID_ID;             /* The event type is not semaphore   */	
    }	
#endif

    return (DeleteEvent(pecb,opt));/* Delete the semaphore event control block*/
}


/**
 *******************************************************************************
 * @brief      Accept a semaphore without waitting 	  
 * @param[in]  id      Event ID   	 
 * @param[out] None  
 * @retval     E_INVALID_ID    Invalid event ID.
 * @retval     E_SEM_EMPTY     No semaphore exist.
 * @retval     E_OK            Get semaphore successful. 	
 *
 * @par Description
 * @details    This function is called accept a semaphore without waitting. 
 *******************************************************************************
 */
StatusType CoAcceptSem(OS_EventID id)
{
    P_ECB pecb;
#if CFG_PAR_CHECKOUT_EN >0
    if(id >= CFG_MAX_EVENT)	                 
    {
        return E_INVALID_ID;
    }
#endif

	pecb = &EventTbl[id];
#if CFG_PAR_CHECKOUT_EN >0
    if( pecb->eventType != EVENT_TYPE_SEM)   
    {
        return E_INVALID_ID;	
    }
#endif
	OsSchedLock();
    if(pecb->eventCounter > 0) /* If semaphore is positive,resource available */
    {	
        pecb->eventCounter--;         /* Decrement semaphore only if positive */
		OsSchedUnlock();
        return E_OK;	
    }
    else                                /* Resource is not available          */
    {	
		OsSchedUnlock();
        return E_SEM_EMPTY;
    }	
}

 
/**
 *******************************************************************************
 * @brief       wait for a semaphore	   
 * @param[in]   id       Event ID.	
 * @param[in]   timeout  The longest time for writting semaphore.
 * @para        0        
 * @para        0x1~0xff 	 
 * @param[out]  None  
 * @retval      E_CALL         Error call in ISR.   
 * @retval      E_INVALID_ID   Invalid event ID.	
 * @retval      E_TIMEOUT      Semaphore was not received within the specified 
 *                             'timeout' time.
 * @retval      E_OK           The call was successful and your task owns the 
 *                             resource,or the event you are waiting for occurred.	
 * 
 * @par Description
 * @details    This function is called to waits for a semaphore. 
 * @note       IF this function is called in ISR,nothing to do and return immediately.
 *******************************************************************************
 */
StatusType CoPendSem(OS_EventID id,U32 timeout)
{
    P_ECB 	 pecb;
    P_OSTCB  curTCB;
    if(OSIntNesting > 0)                /* If the caller is ISR               */
    {
        return E_CALL;
    }
#if CFG_PAR_CHECKOUT_EN >0
    if(id >= CFG_MAX_EVENT)	            
    {
        return E_INVALID_ID;
    }
#endif

	  pecb = &EventTbl[id];
#if CFG_PAR_CHECKOUT_EN >0
    if(pecb->eventType != EVENT_TYPE_SEM)     
    {
       return E_INVALID_ID;	
    }
#endif
    if(OSSchedLock != 0)                /* Schdule is locked?                 */
    {
        return E_OS_IN_LOCK;            /* Yes,error return                   */
    }
    OsSchedLock();
    if(pecb->eventCounter > 0) /* If semaphore is positive,resource available */       
    {	
        pecb->eventCounter--;         /* Decrement semaphore only if positive */
        OsSchedUnlock();
        return E_OK;	
    }
    else                                /* Resource is not available          */
    {
        curTCB = TCBRunning;
        if(timeout == 0)                /* If time-out is not configured      */
        {
            EventTaskToWait(pecb,curTCB); /* Block task until event occurs    */ 
            pecb->eventCounter--;             
            curTCB->pmail = Co_NULL;
            OsSchedUnlock();
            return E_OK;
        }
        else                            /* If time-out is configured          */
        {
            /* Block task until event or timeout occurs                       */
            EventTaskToWait(pecb,curTCB);
            InsertDelayList(curTCB,timeout);
            
            OsSchedUnlock();
            if (curTCB->pmail == Co_NULL)  /* If pmail is Co_NULL, time-out occurred*/
            {
              return E_TIMEOUT;	
            }                               
            else                  /* Event occurred or event have been deleted*/    
            {
                OsSchedLock();
                curTCB->pmail = Co_NULL;
                pecb->eventCounter--; 
                OsSchedUnlock();
                return E_OK;	
            }				
        }		
    }
}


/**
 *******************************************************************************
 * @brief       Post a semaphore	 
 * @param[in]   id   id of event control block associated with the desired semaphore.	 	 
 * @param[out]  None   
 * @retval      E_INVALID_ID   Parameter id passed was invalid event ID.
 * @retval      E_SEM_FULL     Semaphore full. 
 * @retval      E_OK           Semaphore had post successful.
 *
 * @par Description
 * @details    This function is called to post a semaphore to corresponding event. 
 *
 * @note 
 *******************************************************************************
 */
StatusType CoPostSem(OS_EventID id)
{
    P_ECB pecb;
#if CFG_PAR_CHECKOUT_EN >0
    if(id >= CFG_MAX_EVENT)	                  
    {
        return E_INVALID_ID;
    }
#endif

    pecb = &EventTbl[id];
#if CFG_PAR_CHECKOUT_EN >0
    if(pecb->eventType != EVENT_TYPE_SEM) /* Invalid event control block type */
    {
        return E_INVALID_ID;	
    }
#endif

    /* Make sure semaphore will not overflow */
    if(pecb->eventCounter == pecb->initialEventCounter) 
    {
        return E_SEM_FULL;    /* The counter of Semaphore reach the max number*/
    }
    OsSchedLock();
    pecb->eventCounter++;     /* Increment semaphore count to register event  */
    EventTaskToRdy(pecb);     /* Check semaphore event waiting list           */
    OsSchedUnlock();
    return E_OK;
		
}


/**
 *******************************************************************************
 * @brief       Post a semaphore in ISR	 
 * @param[in]   id    identifier of event control block associated with the 
 *                    desired semaphore.	 	 
 * @param[out]  None  
 * @retval      E_INVALID_ID        Parameter id passed was invalid event ID.
 * @retval      E_NO_TASK_WAITTING  There are one more tasks waitting for the event. 
 * @retval      E_OK                Semaphore had signaled successful.
 *
 * @par Description
 * @details    This function is called in ISR to post a semaphore to corresponding
 *             event. 
 * @note 
 *******************************************************************************
 */
#if CFG_MAX_SERVICE_REQUEST > 0
StatusType isr_PostSem(OS_EventID id)
{
    if(OSSchedLock > 0)         /* If scheduler is locked,(the caller is ISR) */      
    {
        /* Initiate a post service handling request */
        if(InsertInSRQ(SEM_REQ,id,Co_NULL) == Co_FALSE)
        {
            return E_SEV_REQ_FULL;        /* If service request queue is full */
        }			
        else                              /* Operate successfully             */
        {
            return E_OK;                        
        }
    }
    else
    {
        return(CoPostSem(id));            /* Post semaphore                   */
    }
}
#endif

#endif
