/**
 *******************************************************************************
 * @file       sem.c
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      Semaphore management implementation code of CooCox CoOS kernel.	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
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
    pecb = CreatEvent(EVENT_TYPE_SEM,sortType,NULL);
    if(pecb == NULL)                    /* If failed to create event block    */
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
		OsSchedUnlock();
        pecb->eventCounter--;         /* Decrement semaphore only if positive */
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
    if(pecb->eventCounter > 0) /* If semaphore is positive,resource available */       
    {	
        pecb->eventCounter--;         /* Decrement semaphore only if positive */
        return E_OK;	
    }
    else                                /* Resource is not available          */
    {
        curTCB = TCBRunning;
        if(timeout == 0)                /* If time-out is not configured      */
        {
            EventTaskToWait(pecb,curTCB); /* Block task until event occurs    */
            curTCB->pmail = NULL;           
            return E_OK;
        }
        else                            /* If time-out is configured          */
        {
            OsSchedLock();
            
            /* Block task until event or timeout occurs                       */
            EventTaskToWait(pecb,curTCB);
            InsertDelayList(curTCB,timeout);
            
            OsSchedUnlock();
            if (curTCB->pmail == NULL)  /* If pmail is NULL, time-out occurred*/
            {
              return E_TIMEOUT;	
            }                               
            else                  /* Event occurred or event have been deleted*/    
            {
                curTCB->pmail = NULL;
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
        if(InsertInSRQ(SEM_REQ,id,NULL) == FALSE) 
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
