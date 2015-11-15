/**
 *******************************************************************************
 * @file       mbox.c
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Mailbox management implementation code of CooCox CoOS kernel.	
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


#if CFG_MAILBOX_EN > 0


/**
 *******************************************************************************
 * @brief      Create a mailbox	 
 * @param[in]  sortType     Mail box waiting list sort type.			 
 * @param[out] None  
 * @retval     E_CREATE_FAIL   Create mailbox fail.
 * @retval     others          Create mailbox successful.		 
 *
 * @par Description
 * @details    This function is called to create a mailbox. 
 * @note 
 *******************************************************************************
 */
OS_EventID CoCreateMbox(U8 sortType)
{
    P_ECB pecb;
    
    /* Create a mailbox type event control block                              */
    pecb = CreatEvent(EVENT_TYPE_MBOX,sortType,Co_NULL);
    if(pecb == Co_NULL)                    /* If failed to create event block    */
    {
        return E_CREATE_FAIL;
    }
    pecb->eventCounter = 0;
    return (pecb->id);      /* Create a mailbox successfully, return event ID */		
}



/**
 *******************************************************************************
 * @brief      Delete a mailbox	   
 * @param[in]  id     Event ID.
 * @param[in]  opt    Delete option.	 
 * @param[out] None   	 
 * @retval     E_INVALID_ID         Invalid event ID.
 * @retval     E_INVALID_PARAMETER  Invalid parameter.
 * @retval     E_TASK_WAITTING      Tasks waitting for the event,delete fail.
 * @retval     E_OK                 Event deleted successful. 
 *
 * @par Description
 * @details    This function is called to delete a mailbox.	 
 * @note 
 *******************************************************************************
 */
StatusType CoDelMbox(OS_EventID id,U8 opt)
{
    P_ECB pecb;
    
#if CFG_PAR_CHECKOUT_EN >0
    if(id >= CFG_MAX_EVENT)               /* Judge id is valid or not?        */ 
    {
        return E_INVALID_ID;              /* Id is invalid ,return error      */
    }
#endif
    pecb = &EventTbl[id];
#if CFG_PAR_CHECKOUT_EN >0
    if(pecb->eventType != EVENT_TYPE_MBOX)/* Validate event control block type*/    
    {
        return E_INVALID_ID;              /* The event is not mailbox         */	
    }
#endif	
    return (DeleteEvent(pecb,opt)); /* Delete the mailbox event control block */
}



/**
 *******************************************************************************
 * @brief      Accept a mailbox	 
 * @param[in]  id    Event ID.  	 
 * @param[out] perr  A pointer to error code.  
 * @retval     Co_NULL
 * @retval     A pointer to mailbox accepted.			 
 *
 * @par Description
 * @details    This function is called to accept a mailbox. 
 * @note 
 *******************************************************************************
 */
void* CoAcceptMail(OS_EventID id,StatusType* perr)
{
    P_ECB pecb;
    void* pmail;
#if CFG_PAR_CHECKOUT_EN >0
    if(id >= CFG_MAX_EVENT)	                
    {
        *perr = E_INVALID_ID;             /* Invalid 'id'                     */
        return Co_NULL;
    }
#endif
    pecb = &EventTbl[id];
    
#if CFG_PAR_CHECKOUT_EN >0
    if(pecb->eventType != EVENT_TYPE_MBOX)/* Invalid event control block type */
    {
        *perr = E_INVALID_ID;	
        return Co_NULL;
    }
#endif
	OsSchedLock();
    if(pecb->eventCounter == 1)             /* If there is already a message  */
    {
        *perr = E_OK;
        pmail = pecb->eventPtr;             /* Get the message                */
        pecb->eventPtr     = Co_NULL;          /* Clear the mailbox              */
        pecb->eventCounter = 0;
		OsSchedUnlock();
        return pmail;                       /* Return the message received    */		
    }
    else                                    /* If the mailbox is empty        */
    {	
		OsSchedUnlock();
        *perr = E_MBOX_EMPTY;               /* Mailbox is empty,return Co_NULL   */
        return Co_NULL;
    }
}



/**
 *******************************************************************************
 * @brief      Wait for a mailbox	 
 * @param[in]  id       Event ID.	 
 * @param[in]  timeout  The longest time for writting mail.	    
 * @param[out] perr     A pointer to error code.	  
 * @retval     Co_NULL
 * @retval     A pointer to mailbox accept.
 *
 * @par Description
 * @details    This function is called to wait a mailbox.	 
 * @note 
 *******************************************************************************
 */
void* CoPendMail(OS_EventID id,U32 timeout,StatusType* perr)
{
    P_ECB pecb;
    void* pmail;
    P_OSTCB  curTCB;
     
    if(OSIntNesting > 0)                /* If the caller is ISR               */
    {
        *perr = E_CALL;
        return Co_NULL;
    }
    
#if CFG_PAR_CHECKOUT_EN >0
    if(id >= CFG_MAX_EVENT)              
    {
        *perr = E_INVALID_ID;           /* Invalid 'id',retrun error          */
        return Co_NULL;
    }
#endif

    pecb = &EventTbl[id];
#if CFG_PAR_CHECKOUT_EN >0
    if(pecb->eventType != EVENT_TYPE_MBOX)
    {
        *perr = E_INVALID_ID;       /* Invalid event type,not EVENT_TYPE_MBOX */
        return Co_NULL;
    }
#endif

    if(OSSchedLock != 0)                /* Judge schedule is locked or not?   */
    {	
        *perr = E_OS_IN_LOCK;           /* Schedule is locked                 */								 
        return Co_NULL;                    /* return Co_NULL                        */
    }
    OsSchedLock();
    if( pecb->eventCounter == 1)        /* If there is already a message      */
    {
        *perr = E_OK;
        pmail = pecb->eventPtr;         /* Get the message                    */
        pecb->eventPtr     = Co_NULL;      /* Clear the mailbox                  */
        pecb->eventCounter = 0;
        OsSchedUnlock();
        return pmail;                   /* Return the message received        */
    }
    else                       /* If message is not available, task will pend */ 
    {
    	OsSchedUnlock();
        curTCB = TCBRunning;
        if(timeout == 0)                /* If time-out is not configured      */
        {
            EventTaskToWait(pecb,curTCB); /* Block task until event occurs    */
            *perr = E_OK;
            
            /* Have recived a message or the mailbox have been deleted        */
            OsSchedLock();
            pmail = curTCB->pmail;    
            pecb->eventCounter = 0;               
            curTCB->pmail = Co_NULL;
            OsSchedUnlock(); 
            return pmail;               /* Return received message or Co_NULL    */
        }
        else                            /* If time-out is configured          */
        {
            OsSchedLock();
            
            /* Block task until event or timeout occurs                       */
            EventTaskToWait(pecb,curTCB);   
            InsertDelayList(curTCB,timeout);
            OsSchedUnlock();
            if( curTCB->pmail == Co_NULL)  /* Time-out occurred                  */
            {
                *perr = E_TIMEOUT;
                return Co_NULL;
            }
            else    /* Have recived a message or the mailbox have been deleted*/
            {
                *perr = E_OK;
                
                OsSchedLock();
                pmail = curTCB->pmail;            
                pecb->eventCounter = 0;   
                curTCB->pmail = Co_NULL;
                OsSchedUnlock(); 
                
                return pmail;           /* Return received message or Co_NULL    */
            }			
        }	
    }
}

 
/**
 *******************************************************************************
 * @brief      Post a mailbox	  
 * @param[in]  id      Event ID.
 * @param[in]  pmail   Pointer to mail that want to send.		 
 * @param[out] None   
 * @retval     E_INVALID_ID	
 * @retval     E_OK		 
 *
 * @par Description
 * @details    This function is called to post a mail. 
 * @note 
 *******************************************************************************
 */
StatusType CoPostMail(OS_EventID id,void* pmail)
{
    P_ECB pecb;
#if CFG_PAR_CHECKOUT_EN >0
    if(id >= CFG_MAX_EVENT)	                
    {
        return E_INVALID_ID;            /* Invalid id,return error            */
    }
#endif

    pecb = &EventTbl[id];
#if CFG_PAR_CHECKOUT_EN >0
    if(pecb->eventType != EVENT_TYPE_MBOX)/* Validate event control block type*/
    {
        return E_INVALID_ID;              /* Event is not mailbox,return error*/
    }
#endif

    if(pecb->eventCounter == 0)   /* If mailbox doesn't already have a message*/	
    {
        OsSchedLock();
        pecb->eventPtr     = pmail;       /* Place message in mailbox         */
        pecb->eventCounter = 1;
        EventTaskToRdy(pecb);             /* Check waiting list               */
        OsSchedUnlock();
        return E_OK;	
    }
    else                          /* If there is already a message in mailbox */              
    {
        return E_MBOX_FULL;       /* Mailbox is full,and return "E_MBOX_FULL" */
    }
}

/**
 *******************************************************************************
 * @brief      Post a mailbox in ISR	    
 * @param[in]  id      Event ID.
 * @param[in]  pmail   Pointer to mail that want to send.		 
 * @param[out] None   
 * @retval     E_INVALID_ID	
 * @retval     E_OK		 
 *
 * @par Description
 * @details    This function is called to post a mail in ISR. 
 * @note 
 *******************************************************************************
 */
#if CFG_MAX_SERVICE_REQUEST > 0
StatusType isr_PostMail(OS_EventID id,void* pmail)
{
    if(OSSchedLock > 0)         /* If scheduler is locked,(the caller is ISR) */
    {
        /* Insert the request into service request queue                      */
        if(InsertInSRQ(MBOX_REQ,id,pmail) == Co_FALSE)
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
        return(CoPostMail(id,pmail));     /* Sends the message to the mailbox */ 
    }
}
#endif

#endif


