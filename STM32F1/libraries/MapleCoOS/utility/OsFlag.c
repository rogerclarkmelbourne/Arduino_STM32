/**
 *******************************************************************************
 * @file       flag.c
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      Flag management implementation code of coocox CoOS kernel.	
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

#if CFG_FLAG_EN > 0
/*---------------------------- Variable Define -------------------------------*/
#define FLAG_MAX_NUM  32                /*!< Define max flag number.          */
FCB     FlagCrl = {0};                  /*!< Flags list struct                */


/*---------------------------- Function Declare ------------------------------*/
static  void FlagBlock(P_FLAG_NODE pnode,U32 flags,U8 waitType);
static  P_FLAG_NODE RemoveFromLink(P_FLAG_NODE pnode);

/**
 *******************************************************************************
 * @brief      Create a flag	 
 * @param[in]  bAutoReset      Reset mode,TRUE(Auto Reset)  FLASE(Manual Reset).
 * @param[in]  bInitialState   Initial state.	 
 * @param[out] None  
 * @retval     E_CREATE_FAIL   Create flag fail.
 * @retval     others          Create flag successful.			 
 *
 * @par Description
 * @details    This function use to create a event flag.	 
 * @note 
 *******************************************************************************
 */
OS_FlagID CoCreateFlag(BOOL bAutoReset,BOOL bInitialState)
{
    U8  i;
    OsSchedLock();
    
    for(i=0;i<FLAG_MAX_NUM;i++)
    {
        /* Assign a free flag control block                                   */
        if((FlagCrl.flagActive&(1<<i)) == 0 )
        {
            FlagCrl.flagActive |= (1<<i);         /* Initialize active flag   */
            FlagCrl.flagRdy    |= (bInitialState<<i);/* Initialize ready flag */
            FlagCrl.resetOpt   |= (bAutoReset<<i);/* Initialize reset option  */
            OsSchedUnlock();
            return i ;                  /* Return Flag ID                     */
        }	
    }
    OsSchedUnlock();
    
    return E_CREATE_FAIL;               /* There is no free flag control block*/	
}


/**
 *******************************************************************************
 * @brief      Delete a flag
 * @param[in]  id      Flag ID. 	
 * @param[in]  opt     Delete option. 
 * @param[out] None          
 * @retval     E_CALL            Error call in ISR.
 * @retval     E_INVALID_ID      Invalid event ID.
 * @retval     E_TASK_WAITTING   Tasks waitting for the event,delete fail.
 * @retval     E_OK              Event deleted successful.   
 *
 * @par Description
 * @details    This function is called to delete a event flag.
 * @note 
 *******************************************************************************
 */
StatusType CoDelFlag(OS_FlagID id,U8 opt)
{
    P_FLAG_NODE pnode;
    P_FCB pfcb;
    pfcb  = &FlagCrl;
    if(OSIntNesting > 0)                /* If be called from ISR              */
    {
        return E_CALL;
    }
#if CFG_PAR_CHECKOUT_EN >0
    if((pfcb->flagActive&(1<<id)) == 0) /* Flag is valid or not               */
    {
        return E_INVALID_ID;	
    }
#endif
    OsSchedLock();
    pnode = pfcb->headNode;
    
    while(pnode != NULL)                /* Ready all tasks waiting for flags  */
    {
        if((pnode->waitFlags&(1<<id)) != 0) /* If no task is waiting on flags */
    	  {
            if(opt == OPT_DEL_NO_PEND)      /* Delete flag if no task waiting */
            {
              	OsSchedUnlock();
               	return E_TASK_WAITING;
            }
            else if (opt == OPT_DEL_ANYWAY) /* Always delete the flag         */
            {
                if(pnode->waitType == OPT_WAIT_ALL)
                {
                    /* If the flag is only required by NODE                   */
                    if( pnode->waitFlags == (1<<id) )	
                    {
                        /* Remove the NODE from waiting list                  */
                        pnode = RemoveFromLink(pnode); 	
                        continue;	
                    }	
                    else
                    {
                        pnode->waitFlags &= ~(1<<id);   /* Update waitflags   */
                    }		
                }
                else   							
                {
                    pnode = RemoveFromLink(pnode);
                    continue;	
                }	
            }
        }	
        pnode = pnode->nextNode;		
    }
    
    /* Remove the flag from the flags list */
    pfcb->flagActive &= ~(1<<id);			
    pfcb->flagRdy    &= ~(1<<id);
    pfcb->resetOpt   &= ~(1<<id);
    OsSchedUnlock();
    return E_OK;
}


/**
 *******************************************************************************
 * @brief      AcceptSingleFlag  
 * @param[in]  id     Flag ID.
 * @param[out] None
 * @retval     E_INVALID_ID      Invalid event ID.
 * @retval     E_FLAG_NOT_READY  Flag is not in ready state.
 * @retval     E_OK              The call was successful and your task owns the Flag.
 *
 * @par Description
 * @details    This fucntion is called to accept single flag
 * @note 
 *******************************************************************************
 */
StatusType CoAcceptSingleFlag(OS_FlagID id)
{
    P_FCB pfcb;
    pfcb  = &FlagCrl;
#if CFG_PAR_CHECKOUT_EN >0	
    if(id >= FLAG_MAX_NUM)              
    {
        return E_INVALID_ID;            /* Invalid 'id',return error          */
    }
    if((pfcb->flagActive&(1<<id)) == 0) 
    {
        return E_INVALID_ID;            /* Flag is deactive,return error      */
    }	
#endif
    if((pfcb->flagRdy&(1<<id)) != 0)    /* If the required flag is set        */
    {
        OsSchedLock()
        pfcb->flagRdy &= ~((FlagCrl.resetOpt)&(1<<id)); /* Clear the flag     */
        OsSchedUnlock();
        return E_OK;
    }
    else                                /* If the required flag is not set    */
    {
        return E_FLAG_NOT_READY;
    }
}


/**
 *******************************************************************************
 * @brief      AcceptMultipleFlags 
 * @param[in]  flags      Flags that waiting to active task.
 * @param[in]  waitType   Flags wait type.
 * @param[out] perr       A pointer to error code.
 * @retval     0
 * @retval     springFlag
 *
 * @par Description
 * @details    This fucntion is called to accept multiple flags. 
 * @note 
 *******************************************************************************
 */
U32 CoAcceptMultipleFlags(U32 flags,U8 waitType,StatusType *perr)
{
    U32  springFlag;
    P_FCB pfcb;
    pfcb  = &FlagCrl;
    
#if CFG_PAR_CHECKOUT_EN >0	
    if((flags&pfcb->flagActive) != flags )  /* Judge flag is active or not?   */    
    {
        *perr = E_INVALID_PARAMETER;        /* Invalid flags                  */
        return 0;
    }
#endif
    
    springFlag = flags & pfcb->flagRdy;
    
    OsSchedLock();
    /* If any required flags are set */
    if( (springFlag != 0) && (waitType == OPT_WAIT_ANY) )	
    {
        
        pfcb->flagRdy &= ~(springFlag & pfcb->resetOpt);  /* Clear the flags  */
        OsSchedUnlock();
        *perr = E_OK;
        return springFlag;
    }
    
    /* If all required flags are set */
    if((springFlag == flags) && (waitType == OPT_WAIT_ALL))
    {
        pfcb->flagRdy &= ~(springFlag&pfcb->resetOpt);    /* Clear the flags  */
        OsSchedUnlock();	
        *perr = E_OK;					
        return springFlag;		 	
    }
    OsSchedUnlock();
    *perr = E_FLAG_NOT_READY;		
    return 0;
}




/**
 *******************************************************************************
 * @brief      WaitForSingleFlag 
 * @param[in]  id        Flag ID.
 * @param[in]  timeout   The longest time for writting flag.
 * @param[out] None   
 * @retval     E_CALL         Error call in ISR.   
 * @retval     E_INVALID_ID   Invalid event ID.	
 * @retval     E_TIMEOUT      Flag wasn't received within 'timeout' time.
 * @retval     E_OK           The call was successful and your task owns the Flag,
 *                            or the event you are waiting for occurred.	 
 *
 * @par Description
 * @details    This function is called to wait for only one flag,
 *             (1) if parameter "timeout" == 0,waiting until flag be set;
 *             (2) when "timeout" != 0,if flag was set or wasn't set but timeout 
 *                 occured,the task will exit the waiting list,convert to READY 
 *                 or RUNNING state.  
 * @note 
 *******************************************************************************
 */
StatusType CoWaitForSingleFlag(OS_FlagID id,U32 timeout)
{
    FLAG_NODE flagNode;
    P_FCB     pfcb;
    P_OSTCB   curTCB;
    
    if(OSIntNesting > 0)                /* See if the caller is ISR           */
    {
        return E_CALL;
    }
    if(OSSchedLock != 0)                /* Schedule is lock?                  */
    {								 
        return E_OS_IN_LOCK;            /* Yes,error return                   */
    }	
    
#if CFG_PAR_CHECKOUT_EN >0	
    if(id >= FLAG_MAX_NUM)              /* Judge id is valid or not?          */  
    {
        return E_INVALID_ID;            /* Invalid 'id'                       */      	
    }
    if((FlagCrl.flagActive&(1<<id)) == 0 )/* Judge flag is active or not?       */
    {
        return E_INVALID_ID;            /* Flag is deactive ,return error     */
    }	
#endif

   	OsSchedLock();
	pfcb = &FlagCrl;
    /* See if the required flag is set */
    if((pfcb->flagRdy&(1<<id)) != 0)    /* If the required flag is set        */
    {
        pfcb->flagRdy &= ~((pfcb->resetOpt&(1<<id))); /* Clear the flag       */
        OsSchedUnlock();
    }
    else                                /* If the required flag is not set    */
    {
        curTCB = TCBRunning;
        if(timeout == 0)                /* If time-out is not configured      */
        {
            /* Block task until the required flag is set                      */
            FlagBlock (&flagNode,(1<<id),OPT_WAIT_ONE);  
            curTCB->state  = TASK_WAITING;	
			TaskSchedReq   = TRUE;
            OsSchedUnlock();
            
            /* The required flag is set and the task is in running state      */
            curTCB->pnode  = NULL;					   		
            OsSchedLock();
            
            /* Clear the required flag or not                                 */	
            pfcb->flagRdy &= ~((1<<id)&(pfcb->resetOpt)); 
            OsSchedUnlock();
        }
        else                            /* If time-out is configured          */
        {
            /* Block task until the required flag is set or time-out occurs   */
            FlagBlock(&flagNode,(1<<id),OPT_WAIT_ONE);
            InsertDelayList(curTCB,timeout);
            
            OsSchedUnlock();
            if(curTCB->pnode == NULL)     /* If time-out occurred             */
            {
                return E_TIMEOUT;		
            }
            else                          /* If flag is set                   */
            {
                curTCB->pnode = NULL;
                OsSchedLock();
                
                /* Clear the required flag or not                             */
                pfcb->flagRdy &= ~((1<<id)&(pfcb->resetOpt));	 
                OsSchedUnlock();
            }	
        }
    }
    return E_OK;	
}


/**
 *******************************************************************************
 * @brief      WaitForMultipleFlags 
 * @param[in]  flags      Flags that waiting to active task.
 * @param[in]  waitType   Flags wait type.
 * @param[in]  timeout    The longest time for writting flag.
 * @param[out] perr       A pointer to error code.
 * @retval     0
 * @retval     springFlag	 
 *
 * @par Description
 * @details    This function is called to pend a task for waitting multiple flag. 
 * @note 
 *******************************************************************************
 */
U32 CoWaitForMultipleFlags(U32 flags,U8 waitType,U32 timeout,StatusType *perr)
{
    U32       springFlag;  	
    P_FCB     pfcb;
    FLAG_NODE flagNode;
    P_OSTCB   curTCB;
    
   
    if(OSIntNesting > 0)                /* If the caller is ISR               */
    {
        *perr = E_CALL;
        return 0;
    }
    if(OSSchedLock != 0)                /* Schedule is lock?                  */
    {	
        *perr = E_OS_IN_LOCK;							 
        return 0;                       /* Yes,error return                   */
    }
#if CFG_PAR_CHECKOUT_EN >0  
    if( (flags&FlagCrl.flagActive) != flags )
    {
        *perr = E_INVALID_PARAMETER;    /* Invalid 'flags'                    */
        return 0;
    }
#endif
    OsSchedLock();
	pfcb = &FlagCrl;
    springFlag = flags & pfcb->flagRdy;
    
    /* If any required flags are set  */
    if((springFlag != 0) && (waitType == OPT_WAIT_ANY))
    {
        pfcb->flagRdy &= ~(springFlag & pfcb->resetOpt);  /* Clear the flag   */
        OsSchedUnlock();
        *perr = E_OK;
        return springFlag;
    }
    
    /* If all required flags are set */
    if( (springFlag == flags) && (waitType == OPT_WAIT_ALL) )  
    {
        pfcb->flagRdy &= ~(springFlag & pfcb->resetOpt);  /* Clear the flags  */
        OsSchedUnlock();	
        *perr = E_OK;
        return springFlag;		 	
    }
    
    curTCB = TCBRunning;
    if(timeout == 0)                    /* If time-out is not configured      */
    {
        /* Block task until the required flag are set                         */
        FlagBlock(&flagNode,flags,waitType);
        curTCB->state  = TASK_WAITING;	
		TaskSchedReq   = TRUE;
		OsSchedUnlock();
        
        curTCB->pnode  = NULL;
        OsSchedLock();			 	
        springFlag     = flags & pfcb->flagRdy;		
        pfcb->flagRdy &= ~(springFlag & pfcb->resetOpt);/* Clear the flags    */	
        OsSchedUnlock();
        *perr = E_OK;
        return springFlag;
    }
    else                                /* If time-out is configured          */
    {
        /* Block task until the required flag are set or time-out occurred    */
        FlagBlock(&flagNode,flags,waitType);
        InsertDelayList(curTCB,timeout);
        
        OsSchedUnlock();
        if(curTCB->pnode == NULL)       /* If time-out occurred               */
        {
            *perr = E_TIMEOUT;
            return 0;	
        }
        else                            /* If the required flags are set      */
        {
            curTCB->pnode = NULL;
            OsSchedLock();
            springFlag    = flags & FlagCrl.flagRdy;
            
            /* Clear the required ready flags or not */
            pfcb->flagRdy &= ~(springFlag & pfcb->resetOpt);	
            OsSchedUnlock();
            *perr = E_OK;
            return springFlag;	
        }	
    }	
}


/**
 *******************************************************************************
 * @brief       Clear a Flag	 
 * @param[in]   id     Flag ID.
 * @param[out]  None
 * @retval      E_OK           Event deleted successful. 	 
 * @retval      E_INVALID_ID   Invalid event ID. 	 
 *
 * @par Description
 * @details     This function is called to clear a flag. 
 *
 * @note 
 *******************************************************************************
 */
StatusType CoClearFlag(OS_FlagID id)
{
    P_FCB pfcb;
    pfcb = &FlagCrl;
#if CFG_PAR_CHECKOUT_EN >0
    if(id >= FLAG_MAX_NUM)                  
    {
        return E_INVALID_ID;                /* Invalid id                     */	
    }
    if((pfcb->flagActive&(1<<id)) == 0)     
    {
        return E_INVALID_ID;                /* Invalid flag                   */
    }
#endif

    pfcb->flagRdy &= ~(1<<id);              /* Clear the flag                 */
    return E_OK;
}


/**
 *******************************************************************************
 * @brief      Set a flag	   
 * @param[in]  id     Flag ID.
 * @param[out] None
 * @retval     E_INVALID_ID   Invalid event ID.
 * @retval     E_OK           Event deleted successful. 	 
 *
 * @par Description
 * @details    This function is called to set a flag. 
 * @note 
 *******************************************************************************
 */
StatusType CoSetFlag(OS_FlagID id)
{
    P_FLAG_NODE pnode;
    P_FCB pfcb;
    pfcb  = &FlagCrl;
    
#if CFG_PAR_CHECKOUT_EN >0
    if(id >= FLAG_MAX_NUM)              /* Flag is valid or not               */							
    {
        return E_INVALID_ID;            /* Invalid flag id                    */      	
    }
    if((pfcb->flagActive&(1<<id)) == 0)  
    {
        return E_INVALID_ID;            /* Flag is not exist                  */
    }
#endif
    
    if((pfcb->flagRdy&(1<<id)) != 0)    /* Flag had already been set          */
    {
    	return E_OK;
    }
    
    pfcb->flagRdy |= (1<<id);           /* Update the flags ready list        */
    
    OsSchedLock();
    pnode = pfcb->headNode;	  		
    while(pnode != NULL)
    {
        if(pnode->waitType == OPT_WAIT_ALL)   /* Extract all the bits we want */
      	{			
            if((pnode->waitFlags&pfcb->flagRdy) == pnode->waitFlags)
            {
               /* Remove the flag node from the wait list                    */
                pnode = RemoveFromLink(pnode);		
                if((pfcb->resetOpt&(1<<id)) != 0)/* If the flags is auto-reset*/	
                {
                    break;							
                }
                continue;	
            }	
      	}
        else                           /* Extract only the bits we want       */	
      	{
            if( (pnode->waitFlags & pfcb->flagRdy) != 0)
            {
                /* Remove the flag node from the wait list                    */
                pnode = RemoveFromLink(pnode);	 	
                if((pfcb->resetOpt&(1<<id)) != 0)
                {
                    break;              /* The flags is auto-reset            */	
                }
                continue;
            }	
      	}
      	pnode = pnode->nextNode;					
    }
    OsSchedUnlock();
    return E_OK;
}



/**
 *******************************************************************************
 * @brief      Set a flag	in ISR 
 * @param[in]  id     Flag ID.
 * @param[out] None 
 * @retval     E_INVALID_ID   Invalid event ID.
 * @retval     E_OK           Event deleted successful. 	 
 *
 * @par Description
 * @details    This function is called in ISR to set a flag. 
 * @note 
 *******************************************************************************
 */
#if CFG_MAX_SERVICE_REQUEST > 0
StatusType isr_SetFlag(OS_FlagID id)
{
    if(OSSchedLock > 0)         /* If scheduler is locked,(the caller is ISR) */
    {
        /* Insert the request into service request queue                      */
        if(InsertInSRQ(FLAG_REQ,id,NULL) == FALSE)	
        {
            return E_SEV_REQ_FULL;      /* The service requst queue is full   */
        }			
        else
        {
            return E_OK;   							
        }
    }
    else
    {
        return(CoSetFlag(id));          /* The caller is not ISR, set the flag*/
    }
}
#endif

/**
 *******************************************************************************
 * @brief      Block a task to wait a flag event  
 * @param[in]  pnode       A node that will link into flag waiting list.
 * @param[in]  flags       Flag(s) that the node waiting for.
 * @param[in]  waitType    Waiting type of the node.
 * @param[out] None	 
 * @retval     None
 *
 * @par Description
 * @details    This function is called to block a task to wait a flag event.	 
 * @note 
 *******************************************************************************
 */
static void FlagBlock(P_FLAG_NODE pnode,U32 flags,U8 waitType)
{
    P_FCB     pfcb;
    pfcb  = &FlagCrl;
    
    TCBRunning->pnode = pnode;	
    pnode->waitTask   = TCBRunning;
    pnode->waitFlags  = flags;      /* Save the flags that we need to wait for*/
    pnode->waitType   = waitType;   /* Save the type of wait                  */
        
    if(pfcb->tailNode == NULL)      /* If this is the first NODE to insert?   */
    {
        pnode->nextNode = NULL;
        pnode->prevNode = NULL;
        pfcb->headNode  = pnode;    /* Insert the NODE to the head            */	
    }
    else                            /* If it is not the first NODE to insert? */
    {
        pfcb->tailNode->nextNode = pnode;   /* Insert the NODE to the tail    */
        pnode->prevNode          = pfcb->tailNode;
        pnode->nextNode          = NULL;
    }
    pfcb->tailNode = pnode;
}


/**
 *******************************************************************************
 * @brief      Remove a flag node from list
 * @param[in]  pnode    A node that will remove from flag waiting list.
 * @param[out] None   
 * @retval     pnode    Next node of the node that have removed out.
 *
 * @par Description
 * @details   This function is called to remove a flag node from the wait list.			 
 * @note 
 *******************************************************************************
 */
static P_FLAG_NODE RemoveFromLink(P_FLAG_NODE pnode)
{
    P_OSTCB ptcb;
    
    RemoveLinkNode(pnode);            /* Remove the flag node from wait list. */			 
    ptcb = pnode->waitTask;
    
    /* The task in the delay list */
    if(ptcb->delayTick != INVALID_VALUE)/* If the task is in tick delay list  */			         
    {
        RemoveDelayList(ptcb);        /* Remove the task from tick delay list */	
    }
	
	ptcb->pnode = (void*)0xffffffff;

	if(ptcb == TCBRunning)
	{
		ptcb->state = TASK_RUNNING;
	} 
	else
	{
		InsertToTCBRdyList(ptcb);         /* Insert the task to ready list        */	
	}   
    return (pnode->nextNode);	
}

/**
 *******************************************************************************
 * @brief      Remove a flag node from list  
 * @param[in]  pnode 	A node that will remove from flag waiting list.
 * @param[out] None   
 * @retval     None		
 *
 * @par Description
 * @details    This function is called to remove a flag node from the wait list.			 
 * @note 
 *******************************************************************************
 */
void RemoveLinkNode(P_FLAG_NODE pnode)
{
    /* If only one NODE in the list*/
    if((pnode->nextNode == NULL) && (pnode->prevNode == NULL)) 
    {
        FlagCrl.headNode = NULL;
        FlagCrl.tailNode = NULL;
    }
    else if(pnode->nextNode == NULL)      /* If the NODE is tail              */
    {
        FlagCrl.tailNode          = pnode->prevNode;
        pnode->prevNode->nextNode = NULL;
    }
    else if(pnode->prevNode == NULL)      /* If the NODE is head              */
    {
        FlagCrl.headNode          = pnode->nextNode;
        pnode->nextNode->prevNode = NULL;	
    }
    else                                  /* The NODE is in the middle        */
    {
        pnode->nextNode->prevNode = pnode->prevNode;
        pnode->prevNode->nextNode = pnode->nextNode;
    }
    pnode->waitTask->pnode = NULL;
}

#endif
