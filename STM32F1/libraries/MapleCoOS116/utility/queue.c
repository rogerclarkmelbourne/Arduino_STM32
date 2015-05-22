/**
 *******************************************************************************
 * @file       queue.c
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Queue management implementation code of CooCox CoOS kernel.	
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


#if CFG_QUEUE_EN > 0										 
/*---------------------------- Variable Define -------------------------------*/
QCB   QueueTbl[CFG_MAX_QUEUE] = {{0}};    /*!< Queue control block table        */
U32   QueueIDVessel = 0;                /*!< Queue list mask                  */


 
/**
 *******************************************************************************
 * @brief      Create a queue	 
 * @param[in]  qStart    Pointer to mail pointer buffer.
 * @param[in]  size      The length of queue.
 * @param[in]  sortType  Mail queue waiting list sort type.
 * @param[out] None  
 * @retval     E_CREATE_FAIL  Create queue fail.
 * @retval     others         Create queue successful.
 *
 * @par Description
 * @details    This function is called to create a queue. 
 * @note 
 *******************************************************************************
 */			 		   
OS_EventID CoCreateQueue(void **qStart, U16 size ,U8 sortType)
{
    U8    i;  
    P_ECB pecb;

#if CFG_PAR_CHECKOUT_EN >0	
    if((qStart == Co_NULL) || (size == 0))
    {
        return E_CREATE_FAIL;
    }
#endif

    OsSchedLock();
    for(i = 0; i < CFG_MAX_QUEUE; i++)
    {
        /* Assign a free QUEUE control block                                  */
        if((QueueIDVessel & (1 << i)) == 0)	
        {
            QueueIDVessel |= (1<<i);		
            OsSchedUnlock();
            
            QueueTbl[i].qStart   = qStart;  /* Initialize the queue           */
            QueueTbl[i].id       = i;
            QueueTbl[i].head     = 0;
            QueueTbl[i].tail     = 0;
            QueueTbl[i].qMaxSize = size; 
            QueueTbl[i].qSize    = 0;
            
            /* Get a event control block and initial the event content        */
            pecb = CreatEvent(EVENT_TYPE_QUEUE,sortType,&QueueTbl[i]);
            
            if(pecb == Co_NULL )       /* If there is no free EVENT control block*/
            {
                return E_CREATE_FAIL;
            }
            return (pecb->id);		
        }
    }
    
    OsSchedUnlock();
    return E_CREATE_FAIL;             /* There is no free QUEUE control block */	
}


/**
 *******************************************************************************
 * @brief      Delete a queue	  
 * @param[in]  id     Event ID. 	
 * @param[in]  opt    Delete option. 	 
 * @param[out] None   
 * @retval     E_INVALID_ID         Invalid event ID.
 * @retval     E_INVALID_PARAMETER  Invalid parameter.
 * @retval     E_TASK_WAITTING      Tasks waitting for the event,delete fail.
 * @retval     E_OK                 Event deleted successful. 
 *
 * @par Description
 * @details    This function is called to delete a queue. 
 * @note 
 *******************************************************************************
 */
StatusType CoDelQueue(OS_EventID id,U8 opt)
{
    P_ECB   pecb;
    P_QCB   pqcb;
    StatusType err;
#if CFG_PAR_CHECKOUT_EN >0      
    if(id >= CFG_MAX_EVENT)	                     
    {
        return E_INVALID_ID;            /* Invalid id,return error            */
    }
#endif

    pecb = &EventTbl[id];
#if CFG_PAR_CHECKOUT_EN >0
    if( pecb->eventType != EVENT_TYPE_QUEUE)
    {
        return E_INVALID_ID;            /* The event is not queue,return error*/	
    }
#endif
    pqcb = (P_QCB)pecb->eventPtr;       /* Point at queue control block       */
    err  = DeleteEvent(pecb,opt);       /* Delete the event control block     */
    if(err == E_OK)                   /* If the event block have been deleted */
    {
        QueueIDVessel &= ~((U32)(1<<(pqcb->id)));   /* Update free queue list             */
        pqcb->qStart   = Co_NULL;
		    pqcb->id       = 0;
        pqcb->head     = 0;
        pqcb->tail     = 0;
        pqcb->qMaxSize = 0;
        pqcb->qSize    = 0;
    }
    return err;	
}


 
/**
 *******************************************************************************
 * @brief      Accept a mail from queue   
 * @param[in]  id     Event ID.	 	 
 * @param[out] perr   A pointer to error code.  
 * @retval     Co_NULL
 * @retval     A pointer to mail accepted.
 *
 * @par Description
 * @details    This function is called to accept a mail from queue.
 * @note 
 *******************************************************************************
 */
void* CoAcceptQueueMail(OS_EventID id,StatusType* perr)
{
  P_ECB pecb;
  P_QCB pqcb;
  void* pmail;
#if CFG_PAR_CHECKOUT_EN >0
    if(id >= CFG_MAX_EVENT)             
    {
        *perr = E_INVALID_ID;           /* Invalid id,return error            */
        return Co_NULL;
    }
#endif

    pecb = &EventTbl[id];
#if CFG_PAR_CHECKOUT_EN >0
    if(pecb->eventType != EVENT_TYPE_QUEUE)/* Invalid event control block type*/          		
    {
        *perr = E_INVALID_ID;
        return Co_NULL;
    }
#endif	
    pqcb = (P_QCB)pecb->eventPtr;       /* Point at queue control block       */
	OsSchedLock();
    if(pqcb->qSize != 0)            /* If there are any messages in the queue */
    {
        /* Extract oldest message from the queue */
        pmail = *(pqcb->qStart + pqcb->head);  
        pqcb->head++;                   /* Update the queue head              */ 
        pqcb->qSize--;          /* Update the number of messages in the queue */  
        if(pqcb->head == pqcb->qMaxSize)
        {
            pqcb->head = 0;	
        }
		OsSchedUnlock();
        *perr = E_OK;
        return pmail;                   /* Return message received            */
    }
    else                                /* If there is no message in the queue*/
    {
		OsSchedUnlock();
        *perr = E_QUEUE_EMPTY;                 
        return Co_NULL;                    /* Return Co_NULL                        */
    }	
}



/**
 *******************************************************************************
 * @brief      Pend for a mail	 
 * @param[in]  id       Event ID.	 
 * @param[in]  timeout  The longest time for writting mail.	
 * @param[out] perr     A pointer to error code.   
 * @retval     Co_NULL
 * @retval     A pointer to mail accept.	 
 *
 * @par Description
 * @details    This function is called to wait for a mail.		   	
 * @note 
 *******************************************************************************
 */
void* CoPendQueueMail(OS_EventID id,U32 timeout,StatusType* perr)
{
    P_ECB   pecb;
    P_QCB   pqcb;
    P_OSTCB curTCB;
    void*   pmail;
    if(OSIntNesting > 0)                /* If the caller is ISR               */
    {
        *perr = E_CALL;
        return Co_NULL;
    }
#if CFG_PAR_CHECKOUT_EN >0
    if(id >= CFG_MAX_EVENT)	         
    {
        *perr = E_INVALID_ID;           /* Invalid event id,return error      */
        return Co_NULL;
    }
#endif

    pecb = &EventTbl[id];
#if CFG_PAR_CHECKOUT_EN >0
    if(pecb->eventType != EVENT_TYPE_QUEUE) /* The event type is not queue    */
    {
        *perr = E_INVALID_ID;
        return Co_NULL;
    }
#endif	
    if(OSSchedLock != 0)                /* Judge schedule is locked or not?   */
    {	
        *perr = E_OS_IN_LOCK;           /* Schedule is locked,return error    */								 
        return Co_NULL;
    }	
    pqcb = (P_QCB)pecb->eventPtr;       /* Point at queue control block       */
    OsSchedLock();
    if(pqcb->qSize != 0)            /* If there are any messages in the queue */
    {
        /* Extract oldest message from the queue                              */
        pmail = *(pqcb->qStart + pqcb->head);   
        pqcb->head++;                   /* Update the queue head              */ 
        pqcb->qSize--;          /* Update the number of messages in the queue */  
        if(pqcb->head == pqcb->qMaxSize)/* Check queue head                   */
        {
            pqcb->head = 0;	
        }
        OsSchedUnlock();
        *perr = E_OK;
        return pmail;                   /* Return message received            */
    }
    else                                /* If there is no message in the queue*/
    {
    	OsSchedUnlock();
        curTCB = TCBRunning;
        if(timeout == 0)                /* If time-out is not configured      */
        {
            /* Block current task until the event occur                       */
            EventTaskToWait(pecb,curTCB); 
            
            /* Have recived message or the queue have been deleted            */
            OsSchedLock(); 
            pmail = curTCB->pmail;              
            curTCB->pmail = Co_NULL;
            pqcb->head++;                             /* Clear event sign         */
            pqcb->qSize--;
            if(pqcb->head == pqcb->qMaxSize)
            {
                pqcb->head = 0;	
            }
            OsSchedUnlock();
            *perr = E_OK;
            return pmail;               /* Return message received or Co_NULL    */
        }
        else                            /* If time-out is configured          */
        {
            OsSchedLock(); 
            
            /* Block current task until event or timeout occurs               */           
            EventTaskToWait(pecb,curTCB);       
            InsertDelayList(curTCB,timeout);
            OsSchedUnlock();
            if(curTCB->pmail == Co_NULL)   /* If time-out occurred               */
            {
                *perr = E_TIMEOUT;
                return Co_NULL;
            }
            else                        /* If event occured                   */
            {
                OsSchedLock();
                pmail = curTCB->pmail;              
                curTCB->pmail = Co_NULL;
                pqcb->head++;                             /* Clear event sign         */
                pqcb->qSize--;
                if(pqcb->head == pqcb->qMaxSize)
                {
                    pqcb->head = 0;	
                }
                OsSchedUnlock();
                *perr = E_OK;
                return pmail;           /* Return message received or Co_NULL    */
            }				
        }	
    }
}


 
/**
 *******************************************************************************
 * @brief      Post a mail to queue	   
 * @param[in]  id      Event ID.
 * @param[in]  pmail   Pointer to mail that want to send.	 	 
 * @param[out] None   
 * @retval     E_OK
 * @retval     E_INVALID_ID
 * @retval     E_QUEUE_FULL		 
 *
 * @par Description
 * @details    This function is called to post a mail to queue.
 * @note 
 *******************************************************************************
 */
StatusType CoPostQueueMail(OS_EventID id,void* pmail)
{	
    P_ECB pecb;
    P_QCB pqcb;
#if CFG_PAR_CHECKOUT_EN >0                     
    if(id >= CFG_MAX_EVENT)	
    {
        return E_INVALID_ID;          
    }
#endif

    pecb = &EventTbl[id];
#if CFG_PAR_CHECKOUT_EN >0
    if(pecb->eventType != EVENT_TYPE_QUEUE)   
    {
        return E_INVALID_ID;            /* The event type isn't queue,return  */	
    }	
#endif
    pqcb = (P_QCB)pecb->eventPtr;	
    if(pqcb->qSize == pqcb->qMaxSize)   /* If queue is full                   */
    {
        return E_QUEUE_FULL;
    }
    else                                /* If queue is not full               */
    {
        OsSchedLock();
        *(pqcb->qStart + pqcb->tail) = pmail;   /* Insert message into queue  */
        pqcb->tail++;                           /* Update queue tail          */
        pqcb->qSize++;          /* Update the number of messages in the queue */
        if(pqcb->tail == pqcb->qMaxSize)        /* Check queue tail           */   
        {
            pqcb->tail = 0;	
        }
        EventTaskToRdy(pecb);           /* Check the event waiting list       */
        OsSchedUnlock();
        return E_OK;
    }
}


/**
 *******************************************************************************
 * @brief      Post a mail to queue in ISR	 
 * @param[in]  id      Event ID.
 * @param[in]  pmail   Pointer to mail that want to send.	 	 
 * @param[out] None   
 * @retval     E_OK
 * @retval     E_INVALID_ID
 * @retval     E_QUEUE_FULL		 
 *
 * @par Description
 * @details    This function is called in ISR to post a mail to queue.
 * @note 				   
 *******************************************************************************
 */
#if CFG_MAX_SERVICE_REQUEST > 0
StatusType isr_PostQueueMail(OS_EventID id,void* pmail)
{
    if(OSSchedLock > 0)         /* If scheduler is locked,(the caller is ISR) */
    {
        /* Insert the request into service request queue                      */
        if(InsertInSRQ(QUEUE_REQ,id,pmail) == Co_FALSE)
        {
            return E_SEV_REQ_FULL;      /* If service request queue is full   */          
        }			
        else  /* If the request have been inserted into service request queue */
        {
            return E_OK;
        }
    }
    else                                /* The scheduler is unlocked          */
    {
        return(CoPostQueueMail(id,pmail));    /* Send the message to the queue*/ 
    }
}
#endif
							   	 
#endif