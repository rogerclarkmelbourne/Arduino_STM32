/**
 *******************************************************************************
 * @file       event.c
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      event management implementation code of CooCox CoOS kernel.	
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


/*---------------------------- Variable Define -------------------------------*/
#if CFG_EVENT_EN > 0

ECB    EventTbl[CFG_MAX_EVENT]= {{0}};/*!< Table which save event control block.*/
P_ECB  FreeEventList = NULL;        /*!< Pointer to free event control block. */


/**
 *******************************************************************************
 * @brief      Create a empty list of event control block 	   
 * @param[in]  None 	 
 * @param[out] None  	 
 * @retval     None	 
 *
 * @par Description
 * @details    This function is called by OSInit() API to create a ECB list,supply
 *             a  pointer to next event control block that not used.	 				
 *******************************************************************************
 */
void CreateEventList(void)
{	
    U8  i;
    P_ECB pecb1;
#if CFG_MAX_EVENT > 1
    P_ECB pecb2;
#endif
    i=0;
    pecb1 = &EventTbl[0];               /* Get first item                     */
#if CFG_MAX_EVENT == 1                  /* Build event list for only one item */									   
    pecb1->eventPtr  = NULL;
    pecb1->id        = i;               /* Assign ID.                         */
    pecb1->eventType = EVENT_TYPE_INVALID;  /* Sign that not to use.          */
#endif
    
#if CFG_MAX_EVENT > 1             /* Build event list for more than one item  */								   
    pecb2 = &EventTbl[1];
    for(;i< (CFG_MAX_EVENT-1);i++ )
    {
        pecb1->eventPtr  = (void*)pecb2;      /* Set link for list            */
        pecb1->id        = i;                 /* Assign ID.                   */
        pecb1->eventType = EVENT_TYPE_INVALID;/* Sign that not to use.        */
        pecb1++;                              /* Get next item                */
        pecb2++;	
    }
	pecb1->eventType = EVENT_TYPE_INVALID;    /* Sign that not to use.        */
    pecb1->eventPtr  = NULL;                  /* Set link for last item       */
    pecb1->id        = i;	
#endif
    
    FreeEventList    = &EventTbl[0];          /* Set free event item          */	
}



/**
 *******************************************************************************
 * @brief      Release a ECB	 
 * @param[in]  pecb     A pointer to event control block which be released.	 
 * @param[out] None 
 * @retval     None	 
 *
 * @par Description
 * @details    This function is called to release a event control block when a 
 *             event be deleted.
 *******************************************************************************
 */
static void ReleaseECB(P_ECB pecb)
{
    pecb->eventType = EVENT_TYPE_INVALID;     /* Sign that not to use.        */ 
    OsSchedLock();                            /* Lock schedule                */
    pecb->eventPtr  = FreeEventList;          /* Release ECB that event hold  */
    FreeEventList   = pecb;                   /* Reset free event item        */
    OsSchedUnlock();                          /* Unlock schedule              */
}



/**
 *******************************************************************************
 * @brief      Create a event	  
 * @param[in]  eventType       The type of event which	being created.
 * @param[in]  eventSortType   Event sort type.
 * @param[in]  eventCounter    Event counter,ONLY for EVENT_TYPE_SEM.
 * @param[in]  eventPtr        Event struct pointer,ONLY for Queue.NULL for other 
 *                             event type.		
 * @param[out] None  
 * @retval     NULL     Invalid pointer,create event fail.					 
 * @retval     others   Pointer to event control block which had assigned right now.
 *
 * @par Description
 * @details    This function is called by CreateSem(),...
 *             to get a event control block and initial the event content. 
 *
 * @note       This is a internal function of CooCox CoOS,User can't call.
 *******************************************************************************
 */
P_ECB CreatEvent(U8 eventType,U8 eventSortType,void* eventPtr)
{
    P_ECB pecb;
    
    OsSchedLock();                      /* Lock schedule                      */
    if(FreeEventList == NULL)           /* Is there no free evnet item        */
    {
        OsSchedUnlock();                /* Yes,unlock schedule                */
        return NULL;                    /* Return error                       */
    }
    pecb          = FreeEventList;/* Assign the free event item to this event */
    FreeEventList = FreeEventList->eventPtr;  /* Reset free event item        */
    OsSchedUnlock();                    /* Unlock schedul                     */
    
    pecb->eventType     = eventType;    /* Initialize event item as user set  */
    pecb->eventSortType = eventSortType;
    pecb->eventPtr      = eventPtr;
    pecb->eventTCBList  = NULL;
    return pecb;                        /* Return event item pointer          */
}


/**
 *******************************************************************************
 * @brief      Delete a event	  
 * @param[in]  pecb     Pointer to event control block which will be deleted. 
 * @param[in]  opt      Delete option.
 * @arg        == OPT_DEL_ANYWAY     Delete event always   
 * @arg        == OPT_DEL_NO_PEND	 Delete event only when no task pending on.
 * @param[out] None  	 
 * @retval     E_INVALID_PARAMETER   Parameter passed is invalid,deleted fail.
 * @retval     E_TASK_WAITTING       These are one more tasks waitting event.  
 * @retval     E_OK                  Delete event control block successful.
 *		  	
 * @par Description
 * @details    This function is called to delete a event from the event wait list
 *             use specify option.
 *
 * @note       This is a internal function of Coocox CoOS,user can't call.		
 *******************************************************************************
 */
StatusType DeleteEvent(P_ECB pecb,U8 opt)
{
    P_OSTCB ptcb;
    if(opt == OPT_DEL_NO_PEND)          /* Do delete event when no task pend? */
    {
        if(pecb->eventTCBList != NULL)  /* Yes,is there task pend this event? */
        {
            return E_TASK_WAITING;      /* Yes,error return                   */
        }
        else
        {
            ReleaseECB(pecb);           /* No,release resource that event hold*/
        }
    }
    else if(opt == OPT_DEL_ANYWAY)      /* Do delete event anyway?            */
    {
        OsSchedLock();                      /* Lock schedule                  */
        while(pecb->eventTCBList != NULL)   /* Is there task pend this event? */
        {                                   /* Yes,remove it                  */
            ptcb = pecb->eventTCBList;/* Get first task in event waiting list */
            if(ptcb->delayTick != INVALID_VALUE) /* Is task in delay list?    */
            {
                RemoveDelayList(ptcb);    /* Yes,remove task from delay list  */
            }

            /* Set next item as event waiting list head */
            pecb->eventTCBList = ptcb->waitNext; 
            ptcb->waitNext     = NULL;  /* Clear link for event waiting list  */
            ptcb->eventID      = INVALID_ID;  /* Sign that not to use.        */

			InsertToTCBRdyList(ptcb);         /* Insert task into ready list  */
        }
        OsSchedUnlock();                  /* Unlock schedule                  */
        ReleaseECB(pecb);                 /* Release resource that event hold */
    }
    return E_OK;                          /* Return OK                        */
}


/**
 *******************************************************************************
 * @brief      Insert a task to event wait list 	  						  
 * @param[in]  pecb    Pointer to event control block corresponding to the event. 	
 * @param[in]  ptcb    Pointer to task that will be insert to event wait list.	 
 * @param[out] None   
 * @retval     None	 
 *
 * @par Description
 * @details   This function is called to insert a task by fllowing manner:
 *            opt == EVENT_SORT_TYPE_FIFO   By FIFO.
 *            opt == EVENT_SORT_TYPE_PRIO   By priority order,hghest priority 
 *                                          as head,lowest priority as end.
 *                                          (Highest-->...-->Lowest-->NULL)	
 *******************************************************************************
 */
void EventTaskToWait(P_ECB pecb,P_OSTCB ptcb)
{
    P_OSTCB ptcb1;
#if (CFG_EVENT_SORT == 2) || (CFG_EVENT_SORT == 3)
    P_OSTCB ptcb2;
#endif
    
    OsSchedLock();                  /* Lock schedule                          */
    ptcb1 = pecb->eventTCBList;     /* Get first task in event waiting list   */
    ptcb->eventID = pecb->id;       /* Set event ID for task                  */
    
#if CFG_EVENT_SORT == 3             /* Does event waiting list sort as FIFO?  */
                              
    if(pecb->eventSortType == EVENT_SORT_TYPE_FIFO)	
#endif
    
#if (CFG_EVENT_SORT == 1) || (CFG_EVENT_SORT == 3)
    {
        if(ptcb1 == NULL)                 /* Is no item in event waiting list?*/
        {
            pecb->eventTCBList = ptcb;    /* Yes,set task as first item       */
        }
        else
        {								
            while(ptcb1->waitNext != NULL)/* No,insert task in last           */
            {
                ptcb1 = ptcb1->waitNext;	
            }	
            ptcb1->waitNext = ptcb;       /* Set link for list                */
            ptcb->waitPrev  = ptcb1;	
        }
    }
#endif
    
#if CFG_EVENT_SORT ==3 /* Does event waiting list sort as preemptive priority?*/                           
    else if(pecb->eventSortType == EVENT_SORT_TYPE_PRIO)
#endif  
#if (CFG_EVENT_SORT == 2) || (CFG_EVENT_SORT == 3)
    {
        if(ptcb1 == NULL)               /* Is no item in event waiting list?  */
        {
            pecb->eventTCBList = ptcb;  /* Yes,set task as first item         */
        }
        /* Is PRI of task higher than list first item?                        */
        else if(ptcb1->prio > ptcb->prio) 
        {
            pecb->eventTCBList = ptcb;  /* Reset task as first item           */
            ptcb->waitNext     = ptcb1; /* Set link for list                  */
            ptcb1->waitPrev    = ptcb;	
        }
        else                            /* No,find correct place to insert    */
        {								
            ptcb2 = ptcb1->waitNext;
            while(ptcb2 != NULL)        /* Is last item?                      */
            {	                          
                if(ptcb2->prio > ptcb->prio)  /* No,is correct place?         */
                { 
                    break;                    /* Yes,break Circulation        */
                }
                ptcb1 = ptcb2;                /* Save current item            */
                ptcb2 = ptcb2->waitNext;      /* Get next item                */
            }
            ptcb1->waitNext = ptcb;           /* Set link for list            */
            ptcb->waitPrev  = ptcb1;
            ptcb->waitNext  = ptcb2;
            if(ptcb2 != NULL)
            {
                ptcb2->waitPrev = ptcb;	
            }
        }		
    }
#endif
    ptcb->state = TASK_WAITING;     /* Set task status to TASK_WAITING state  */
    TaskSchedReq = TRUE;
    OsSchedUnlock();                /* Unlock schedule,and call task schedule */
}


/**
 *******************************************************************************
 * @brief      Move a task from event WAITING list to the DELAY list	  
 * @param[in]  pecb    Pointer to event control block corresponding to the event. 		 	  
 * @param[out] None  
 * @retval     None	 
 *
 * @par Description
 * @details    This function is called to remove a task from event wait list,and	 
 *             then insert it into the READY list.
 *******************************************************************************
 */
void EventTaskToRdy(P_ECB pecb)
{
    P_OSTCB ptcb;
#if CFG_QUEUE_EN >0
    P_QCB   pqcb;
#endif
    ptcb = pecb->eventTCBList;
    if(ptcb == NULL)
        return;
    
    pecb->eventTCBList = ptcb->waitNext;/* Get first task in event waiting list*/
    if(pecb->eventTCBList != NULL)      /* Is no item in event waiting list?  */
    {
        pecb->eventTCBList->waitPrev = NULL; /* No,clear link for first item  */
    }
    
    ptcb->waitNext = NULL;                /* Clear event waiting link for task*/
    ptcb->eventID  = INVALID_ID;          /* Sign that not to use.            */
    
    if(ptcb->delayTick != INVALID_VALUE)  /* Is task in delay list?           */		         
    {
        RemoveDelayList(ptcb);            /* Yes,remove task from DELAY list  */
    }
    if(pecb->eventType == EVENT_TYPE_MBOX)/* Is it a mailbox event?           */
    {
        ptcb->pmail    = pecb->eventPtr;  /* Yes,send mail to task            */
        pecb->eventPtr = NULL;            /* Clear event sign                 */
        pecb->eventCounter--;
    }
#if CFG_QUEUE_EN >0
    else if(pecb->eventType == EVENT_TYPE_QUEUE)  /* Is it a queue event?     */
    {										   
        pqcb        = (P_QCB)pecb->eventPtr;      /* Yes,get queue pointer    */
        ptcb->pmail = *(pqcb->qStart + pqcb->head);   /* Send mail to task    */
        pqcb->head++;                             /* Clear event sign         */
        pqcb->qSize--;
        if(pqcb->head == pqcb->qMaxSize)
        {
            pqcb->head = 0;	
        }
    }
#endif

#if CFG_MAILBOX_EN >0
    else if(pecb->eventType == EVENT_TYPE_SEM)/* Is it a semaphore event?     */
    {
        pecb->eventCounter--;                 /* Yes,clear event sign         */
        ptcb->pmail = (void*)0xffffffff;      /* Indicate task woke by event  */
    }
#endif
	if(ptcb == TCBRunning)
	{
		ptcb->state = TASK_RUNNING;
	} 
	else
	{
		InsertToTCBRdyList(ptcb);            /* Insert task into ready list  */
	}
}



/**
 *******************************************************************************
 * @brief      Move a task from event wait list to the ready list	  
 * @param[in]  pecb    Pointer to event control block corresponding to the event. 		 	  
 * @param[out] None  
 * @retval     None	 
 *
 * @par Description
 * @details    This function is called to remove a task from event wait list,and	 
 *             then insert it to the ready list.
 *******************************************************************************
 */
void RemoveEventWaittingList(P_OSTCB ptcb)
{
    P_ECB pecb;
    pecb = &EventTbl[ptcb->eventID];    /* Get event control block            */
    
    /* Is there only one item in event waiting list?                          */
    if((ptcb->waitNext == NULL) && (ptcb->waitPrev == NULL))
    {
        pecb->eventTCBList = NULL;      /* Yes,set event waiting list as NULL */
    }
    else if(ptcb->waitPrev == NULL)/* Is the first item in event waiting list?*/
    {
        /* Yes,remove task from list,and reset event waiting list             */
        ptcb->waitNext->waitPrev = NULL;
		pecb->eventTCBList = ptcb->waitNext;	
        ptcb->waitNext = NULL;		
    }
    else if(ptcb->waitNext == NULL)/* Is the last item in event waiting list? */
    {
        ptcb->waitPrev->waitNext = NULL;  /* Yes,remove task form list        */
        ptcb->waitPrev = NULL;
    }
    else                                  /* No, remove task from list        */
    {										
        ptcb->waitPrev->waitNext = ptcb->waitNext;
        ptcb->waitNext->waitPrev = ptcb->waitPrev;
        ptcb->waitPrev = NULL;
        ptcb->waitNext = NULL;		
    }
    ptcb->eventID  = INVALID_ID;          /* Sign that not to use.            */
}

#endif	 //CFG_EVENT_EN

