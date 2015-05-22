/**
 *******************************************************************************
 * @file       task.c
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      task management implementation code of CooCox CoOS kernel.	
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

/*!< Table use to save TCB pointer.              */
OSTCB    TCBTbl[CFG_MAX_USER_TASKS+SYS_TASK_NUM] = {{0}};

/*!< The stack of IDLE task.                     */
OS_STK   idle_stk[CFG_IDLE_STACK_SIZE] = {0};

P_OSTCB  FreeTCB     = Co_NULL;  /*!< pointer to free TCB                        */
P_OSTCB  TCBRdy      = Co_NULL;  /*!< Pointer to the READY list.                 */
P_OSTCB  TCBNext     = Co_NULL;  /*!< Poniter to task that next scheduled by OS  */
P_OSTCB  TCBRunning  = Co_NULL;  /*!< Pointer to TCB that current running task.  */
U64      OSCheckTime = 0;     /*!< The counter of system tick.                */

#if CFG_ORDER_LIST_SCHEDULE_EN ==0
OS_TID   PriNum;
U8       ActivePri[CFG_MAX_USER_TASKS+SYS_TASK_NUM];
U8       TaskNumPerPri[CFG_MAX_USER_TASKS+SYS_TASK_NUM];
OS_TID   RdyTaskPri[CFG_MAX_USER_TASKS+SYS_TASK_NUM] = {0};	
U32      RdyTaskPriInfo[(CFG_MAX_USER_TASKS+SYS_TASK_NUM+31)/32];
#endif


/**
 *******************************************************************************
 * @brief      Create a TCB list.	  
 * @param[in]  None 	 
 * @param[out] None    
 * @retval     None		 
 *
 * @par Description
 * @details    This function is called by CoOSInit() to initial the empty list	 
 *             of OS_TCBS,supply a pointer to free TCB.
 *******************************************************************************
 */
void CreateTCBList(void)
{	
    U8  i;
    P_OSTCB ptcb1,ptcb2;
    
#if CFG_ORDER_LIST_SCHEDULE_EN ==0
	PriNum = 0;
#endif

	ptcb1 = &TCBTbl[0];	                /* Build the free TCB list            */
    ptcb2 = &TCBTbl[1];  
    for(i=0;i< (CFG_MAX_USER_TASKS+SYS_TASK_NUM-1);i++ )
    {
		ptcb1->taskID    = i;
		ptcb1->state     = TASK_DORMANT;
        ptcb1->TCBnext   = ptcb2;
#if CFG_ORDER_LIST_SCHEDULE_EN ==0
		RdyTaskPri[i]    = INVALID_ID;
		ActivePri[i]	 = INVALID_ID;
#endif
        ptcb1++;
        ptcb2++;	
    }
#if CFG_ORDER_LIST_SCHEDULE_EN ==0
		ActivePri[i]	 = INVALID_ID;
#endif

	ptcb1->taskID    = i;	
    ptcb1->TCBnext   = Co_NULL;
    FreeTCB = &TCBTbl[0];         /* Initialize FreeTCB as head item of list  */			
}



#if CFG_ORDER_LIST_SCHEDULE_EN ==0

/**
 *******************************************************************************
 * @brief      Get sequence number for Assign priority	  
 * @param[in]  pri            Assign priority	 
 * @param[out] SequenceNum    priority number 
 * @retval     Co_TRUE           Assign priority in priority queue.
 *             Co_FALSE          Assign priority not in priority queue.
 *					
 * @par Description
 * @details    This function is called in Binary-Scheduling Algorithm 
 *             to get sequence number for Assign priority.     
 *******************************************************************************
 */
static BOOL  GetPriSeqNum(U8 pri,OS_TID* SequenceNum)
{
	OS_TID  seqNum;
	OS_TID  num,tmpNum;
	num = 0;
	seqNum = PriNum;
	while(num != seqNum)
	{
		tmpNum = num;
		num = (num+seqNum)/2;
		if(pri == ActivePri[num])
		{
			*SequenceNum = num;
			return Co_TRUE;
		}
		else if (pri < ActivePri[num])
		{
			seqNum = num;
			num = tmpNum;
		}
		else
		{
			num++;
		}
	}
	*SequenceNum = num;
	return Co_FALSE;
}


/**
 *******************************************************************************
 * @brief      Get the nearest ready priority sequence number for Assign number	  
 * @param[in]  seqNum         Assign sequence number	 
 * @param[out] None
 * @retval     INVALID_ID     Cannot find higher ready priority.   
 *             Others         Nearest ready priority sequence number 		 
 *					
 * @par Description
 * @details    This function is called in Binary-Scheduling Algorithm 
 *             to get the nearest ready priority sequence number.    
 *******************************************************************************
 */
static U8 GetRdyPriSeqNum(U8 seqNum)
{
	U32 tmp;
	U8  i,j,num;
	S8  cnt;
	i = seqNum/32;
	j = seqNum%32;

	do
	{
	  	tmp = RdyTaskPriInfo[i];
		if(tmp != 0)
		{
			num = j/8;
			do
			{
				if((tmp&(0xff<<(num*8))) !=0 )
				{
					if((tmp&(0xf0<<(num*8))) !=0)
					{
						for(cnt=j; cnt >=(num*8+4); cnt--)	
						{
							if( (tmp&(1<<cnt)) !=0)
							{
								return (32*i+cnt);
							}	
						}			
					}

					if((j&0x4)==4)
						j = (j|0x3) -4;
					
					for(cnt=j; cnt >=num*8; cnt--)	
					{
						if( (tmp&(1<<cnt)) !=0)
						{
							return (32*i+cnt);
						}	
					}
				}
				j = num*8 -1;
			}while((num--)!=0);
		}
		j=31;
	}while((i--)!=0);
	return INVALID_ID;											
}


/**
 *******************************************************************************
 * @brief      Remap the ready status of priority queue from Assign sequence number 
 * @param[in]  seqNum         Assign sequence number	 
 * @param[out] None
 * @retval     None    		 
 *					
 * @par Description
 * @details    This function is called in Binary-Scheduling Algorithm 
 *             to Remap the ready status for priority queue.    
 *******************************************************************************
 */
static void PrioRemap(OS_TID  seqNum)
{
	U8 i,j;
	U32 tmp;
	tmp = j = 0;
	j = seqNum/32;
	for(i=0;i<seqNum%32;i++)
	{
		tmp |= 1<<i;
	}
	tmp &= RdyTaskPriInfo[j];
	
	for(i=seqNum; i<PriNum; i++)
	{
		if((i%32==0)&&(i!=seqNum))
		{
			RdyTaskPriInfo[j++] = tmp;
			tmp = 0;
		}
		if(RdyTaskPri[i] != INVALID_ID)
		{
			tmp = tmp | (1<<(i%32));
		}
	}
	RdyTaskPriInfo[j++] = tmp;
}


/**
 *******************************************************************************
 * @brief      Get the ready status for assign sequence number 
 * @param[in]  seqNum      Assign sequence number	 
 * @param[out] None
 * @retval     Co_TRUE        This priority has ready task
 *             Co_FALSE       This priority doesn't have ready task
 *					
 * @par Description
 * @details    This function is called in Binary-Scheduling Algorithm 
 *             to get the ready status for assign sequence number.    
 *******************************************************************************
 */
static BOOL GetPrioSeqNumStatus(U8 seqNum)
{
	if( (RdyTaskPriInfo[seqNum/32] & (1<<(seqNum%32))) == 0)
	{
		return Co_FALSE;
	}
	return Co_TRUE;
}


/**
 *******************************************************************************
 * @brief      Set the ready status for assign sequence number 
 * @param[in]  seqNum      Assign sequence number
 * @param[in]  isRdy       Ready statues for assign sequence number 	 
 * @param[out] None
 * @retval     None 		 
 *					
 * @par Description
 * @details    This function is called in Binary-Scheduling Algorithm 
 *             to set the ready status for assign sequence number.    
 *******************************************************************************
 */
static void SetPrioSeqNumStatus(U8 seqNum, BOOL isRdy)
{
	U32 tmp;
	tmp = RdyTaskPriInfo[seqNum/32];
	tmp	&= ~(1<<(seqNum%32));
	tmp |= isRdy<<(seqNum%32);
	RdyTaskPriInfo[seqNum/32] = tmp;
}


/**
 *******************************************************************************
 * @brief      Active priority in queue 
 * @param[in]  pri       Task priority
 * @param[in]  None     
 * @param[out] None
 * @retval     None 		 
 *					
 * @par Description
 * @details    This function is called in Binary-Scheduling Algorithm 
 *             to active priority in queue, if this priority had been in activation,
 *             increate the task num for this priority.    
 *******************************************************************************
 */
void ActiveTaskPri(U8 pri)
{
	OS_TID  seqNum,num;
	if(GetPriSeqNum(pri,&seqNum) == Co_FALSE)
	{
		for(num=PriNum;num>seqNum;num--)
		{
			ActivePri[num]     = ActivePri[num-1];
			TaskNumPerPri[num] = TaskNumPerPri[num-1];
			RdyTaskPri[num]    = RdyTaskPri[num-1];
		}
		ActivePri[seqNum]     = pri;
		TaskNumPerPri[seqNum] = 1;
		RdyTaskPri[seqNum]    = INVALID_ID;
		PriNum++;
		PrioRemap(seqNum);
	}
	else
	{
		 TaskNumPerPri[seqNum]++;
	}
}



/**
 *******************************************************************************
 * @brief      Delete priority in queue 
 * @param[in]  pri       Task priority
 * @param[in]  None     
 * @param[out] None
 * @retval     None 		 
 *					
 * @par Description
 * @details    This function is called in Binary-Scheduling Algorithm 
 *             to decrease the task num for this priority, if the num goto 0,
 *             remove the priority for queue.
 *******************************************************************************
 */
void DeleteTaskPri(U8 pri)
{
	OS_TID  seqNum,num;

	GetPriSeqNum(pri,&seqNum);
	TaskNumPerPri[seqNum]--;
	if(TaskNumPerPri[seqNum]==0)
	{
		for(num=seqNum; num<(PriNum-1); num++)
		{
			ActivePri[num]     = ActivePri[num+1];
			TaskNumPerPri[num] = TaskNumPerPri[num+1];
			RdyTaskPri[num]    = RdyTaskPri[num+1];
		}
		PriNum--;
		PrioRemap(seqNum);
	}
}

#endif


/**
 *******************************************************************************
 * @brief      Insert a task to the ready list	   
 * @param[in]  tcbInsert    A pointer to task will be inserted.
 * @param[out] None  
 * @retval     None	 
 *
 * @par Description
 * @details   This function is called to insert a task to the READY list. 
 *******************************************************************************
 */
void InsertToTCBRdyList(P_OSTCB tcbInsert)
{
    P_OSTCB ptcbNext,ptcb;
    U8  prio;
#if CFG_ORDER_LIST_SCHEDULE_EN ==0
	U8  seqNum;
	U8  RdyTaskSeqNum;
#endif
    
    prio = tcbInsert->prio;             /* Get PRI of inserted task           */
    tcbInsert->state     = TASK_READY;  /* Set task as TASK_READY             */

#if CFG_ROBIN_EN >0
	ptcb = TCBRunning;
    /* Set schedule time for the same PRI task as TCBRunning.                 */
    if(prio == ptcb->prio)  /* Is PRI of inserted task equal to running task? */
    {
        if(ptcb != tcbInsert) /* Yes,is inserted task equal to running task?  */
        {
            if(ptcb != Co_NULL)            /* No,TCBRunning == Co_NULL?             */
            {                           /* N0,OSCheckTime < OSTickCnt?        */
                if(OSCheckTime < OSTickCnt)	 
                {                       /* Yes,set OSCheckTime for task robin */
                    OSCheckTime = OSTickCnt + ptcb->timeSlice;	
                }			
            }			
        }
    }
#endif


#if CFG_ORDER_LIST_SCHEDULE_EN ==0
	GetPriSeqNum(prio,&seqNum);
	if(GetPrioSeqNumStatus(seqNum) == Co_TRUE)
	{
		ptcb = &TCBTbl[RdyTaskPri[seqNum]];
		RdyTaskPri[seqNum] = tcbInsert->taskID;
	}
	else
	{
		RdyTaskPri[seqNum] = tcbInsert->taskID;
		RdyTaskSeqNum = GetRdyPriSeqNum(seqNum);
		SetPrioSeqNumStatus(seqNum, 1);
		if(RdyTaskSeqNum == INVALID_ID)
		{
		    ptcb = TCBRdy;
		    TaskSchedReq = Co_TRUE;
			if(ptcb == Co_NULL)
			{
				TCBRdy   = tcbInsert;	
			}
			else
			{
				tcbInsert->TCBnext = ptcb;  /* Yes,set tcbInsert as head item of list */
				ptcb->TCBprev = tcbInsert;
				TCBRdy         = tcbInsert;
			}
			return;
		}
		else
		{
			ptcb = &TCBTbl[RdyTaskPri[RdyTaskSeqNum]];	
		}
	}

	ptcbNext = ptcb->TCBnext;
	tcbInsert->TCBnext = ptcbNext;    /* Set link for list                  */
	ptcb->TCBnext      = tcbInsert;
	tcbInsert->TCBprev = ptcb;
	if(ptcbNext != Co_NULL)
	{
	    ptcbNext->TCBprev  = tcbInsert;
	}


#else
    ptcb = TCBRdy;
    if (ptcb == Co_NULL)                   /* Is ready list Co_NULL?                */
    {
        TaskSchedReq = Co_TRUE;
        TCBRdy = tcbInsert;         /* Yse,set tcbInsert as head item of list */
    }
    else if (prio < ptcb->prio)/* Is PRI of inserted task higher than TCBRdy? */
    {
        TaskSchedReq = Co_TRUE;
        tcbInsert->TCBnext = ptcb;  /* Yes,set tcbInsert as head item of list */
        ptcb->TCBprev  = tcbInsert;
        TCBRdy         = tcbInsert;
    }
    else                                /* No,find correct place              */
    {								    
        ptcbNext = ptcb->TCBnext;       /* Get next item                      */
        while(ptcbNext != Co_NULL)         /* Is last item in ready list?        */
        {                               /* No,find correct place              */
            if(prio < ptcbNext->prio)   /* Is correct place?                  */
                break;                  /* Yes,break circulation              */
            ptcb     = ptcbNext;        /* Save current item                  */
            ptcbNext = ptcbNext->TCBnext; /* Get next item                    */
        }
        tcbInsert->TCBnext = ptcbNext;  /* Set link for list                  */
        ptcb->TCBnext      = tcbInsert;
        tcbInsert->TCBprev = ptcb;
        if(ptcbNext != Co_NULL)
        {
            ptcbNext->TCBprev  = tcbInsert;
        }		
    }
#endif
}



/**
 *******************************************************************************
 * @brief      Remove a task from the READY list	   
 * @param[in]  ptcb     A pointer to task which be removed.	 
 * @param[out] None 				 
 * @retval     None		 
 *
 * @par Description
 * @details    This function is called to remove a task from the READY list.
 *******************************************************************************
 */
void RemoveFromTCBRdyList(P_OSTCB ptcb)
{

#if CFG_ORDER_LIST_SCHEDULE_EN ==0
	U8 prio;
	U8 seqNum;
	BOOL isChange;
	isChange = Co_FALSE;
	prio = ptcb->prio;
	GetPriSeqNum(prio,&seqNum);
#endif

    /* Is there only one item in READY list?                                  */
    if((ptcb->TCBnext == Co_NULL) && (ptcb->TCBprev == Co_NULL) )
    {
        TCBRdy = Co_NULL;                  /* Yes,set READY list as Co_NULL         */
#if CFG_ORDER_LIST_SCHEDULE_EN ==0
		isChange = Co_TRUE;
#endif
    }
    else if(ptcb->TCBprev == Co_NULL)      /* Is the first item in READY list?   */
    {   
	    /* Yes,remove task from the list,and reset the head of READY list     */
        TCBRdy = ptcb->TCBnext;		    
        ptcb->TCBnext   = Co_NULL;
        TCBRdy->TCBprev = Co_NULL;
#if CFG_ORDER_LIST_SCHEDULE_EN ==0
		if(TCBRdy->prio != prio)
			isChange = Co_TRUE;
		
#endif
    }
    else if( ptcb->TCBnext == Co_NULL)     /* Is the last item in READY list?    */
    {                                   /* Yes,remove task from list          */
#if CFG_ORDER_LIST_SCHEDULE_EN ==0
		if(ptcb->TCBprev->prio != prio)
			isChange = Co_TRUE;
		else 
			RdyTaskPri[seqNum] = ptcb->TCBprev->taskID;
#endif
        ptcb->TCBprev->TCBnext = Co_NULL;
        ptcb->TCBprev          = Co_NULL;
    }
    else                                /* No, remove task from list          */
    {	
#if CFG_ORDER_LIST_SCHEDULE_EN ==0
		if((ptcb->TCBprev->prio != prio) && (ptcb->TCBnext->prio != prio))
			isChange = Co_TRUE;
		else if((ptcb->TCBprev->prio == prio) && (ptcb->TCBnext->prio != prio))
			RdyTaskPri[seqNum] = ptcb->TCBprev->taskID;
#endif								
        ptcb->TCBprev->TCBnext = ptcb->TCBnext;
        ptcb->TCBnext->TCBprev = ptcb->TCBprev;
        ptcb->TCBnext = Co_NULL;
        ptcb->TCBprev = Co_NULL;
    }
#if CFG_ORDER_LIST_SCHEDULE_EN ==0
		if(isChange == Co_TRUE)
		{
			RdyTaskPri[seqNum] = INVALID_ID;
			SetPrioSeqNumStatus(seqNum, 0);
		}
#endif
}


#if CFG_MUTEX_EN > 0
#define CFG_PRIORITY_SET_EN       (1)
#endif
#if CFG_PRIORITY_SET_EN >0
/**
 *******************************************************************************
 * @brief      Change task priority	   
 * @param[in]  taskID     Specify task id.
 * @param[in]  priority   New priority.	 
 * @param[out] None		   
 * @retval     E_OK              Change priority successful.
 * @retval     E_INVALID_ID      Invalid id,change priority fail.
 * @retval     E_PROTECTED_TASK  Can't change idle task priority.		 
 *
 * @par Description
 * @details    This function is called to change priority for a specify task. 	
 *******************************************************************************
 */
StatusType CoSetPriority(OS_TID taskID,U8 priority)
{			
    P_OSTCB ptcb;
#if CFG_MUTEX_EN >0
    U8 prio;
    P_MUTEX	pMutex;
#endif
#if CFG_EVENT_EN >0
    P_ECB pecb;
#endif

    if(taskID == 0)                     /* Is idle task?                      */
    {											 
        return E_PROTECTED_TASK;        /* Yes,error return                   */
    }   
	
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(taskID >= CFG_MAX_USER_TASKS + SYS_TASK_NUM)
    {
        return E_INVALID_ID;
    }
#endif
	ptcb = &TCBTbl[taskID];             /* Get TCB of task ID                 */
#if CFG_PAR_CHECKOUT_EN >0    
    if(ptcb->state == TASK_DORMANT)
    {
        return E_INVALID_ID;
    }
    if(priority > CFG_LOWEST_PRIO)
    {
        return E_INVALID_ID;
    }
#endif

    if(ptcb->prio != priority)          /* Is PRI equal to original PRI?      */
    {                                   /* No                                 */
#if CFG_MUTEX_EN >0
        if(ptcb->mutexID != INVALID_ID)
        {
            pMutex = &MutexTbl[ptcb->mutexID];
            if(pMutex->taskID == ptcb->taskID)  /* Task hold mutex?               */
            {
                 pMutex->originalPrio= priority;/* Yes,change original PRI in mutex*/
                 if(ptcb->prio < priority)     /* Is task priority higher than set?*/
                 {
                     return E_OK;                /* Yes,do nothing,return OK       */
                 }
            }		
         }

#endif	

#if CFG_ORDER_LIST_SCHEDULE_EN ==0
		DeleteTaskPri(ptcb->prio);
		ActiveTaskPri(priority);	
#endif	

        ptcb->prio = priority;              /* Change task PRI                */
        if(ptcb->state == TASK_READY)       /* Is task in READY list?         */
        {
            OsSchedLock();                  /* Yes,reorder task in READY list */
            RemoveFromTCBRdyList(ptcb);
            InsertToTCBRdyList(ptcb);	
            OsSchedUnlock();
        }
        else if(ptcb->state == TASK_RUNNING)/* Is task running?               */
        {
            if(ptcb->prio > TCBRdy->prio)   /* Yes,Is PRI higher than TCBRdy? */
            {
				OsSchedLock();              /* Yes,reorder task in READY list */
				TaskSchedReq = Co_TRUE;
                OsSchedUnlock();
            }
        }
        else
        {                                   /* No,task in WAITING list        */
#if CFG_MUTEX_EN >0
            if(ptcb->mutexID != INVALID_ID) /* Is task in mutex WAITING list? */
            {
                /* Yes,reset the highest PRI in the list */
				OsSchedLock(); 
				pMutex = &MutexTbl[ptcb->mutexID];
                ptcb = pMutex->waittingList;  
                prio = pMutex->originalPrio; 
                pMutex->hipriTaskID = pMutex->taskID;
                while(ptcb != Co_NULL)
                {
                    if(ptcb->prio < prio)
                    {
                        prio = ptcb->prio;
                        pMutex->hipriTaskID = ptcb->taskID;
                    }
                    ptcb = ptcb->TCBnext;			
                }
				OsSchedUnlock();
                if(pMutex->originalPrio != prio)
                {
                    CoSetPriority(pMutex->taskID,prio);	
                }	
            }
#endif

#if CFG_EVENT_EN >0
			ptcb = &TCBTbl[taskID];
            if(ptcb->eventID != INVALID_ID) /* Is task in event WAITING list? */
            {								    
                pecb = &EventTbl[ptcb->eventID];
                
                /* Yes,is event sort type as preemptive PRI?                  */
                if(pecb->eventSortType == EVENT_SORT_TYPE_PRIO)
                {	  
                    /* Yes,reorder task in the list                           */
                    RemoveEventWaittingList(ptcb);
                    EventTaskToWait(pecb,ptcb);
                }	
            }
#endif
        }
    }
    return E_OK;
}
#endif

/**
 *******************************************************************************
 * @brief      Schedule function	  
 * @param[in]  None 	 
 * @param[out] None  	 
 * @retval     None	 
 *
 * @par Description
 * @details    This function is called by every where need to switch context,
 *             It is schedule function of OS kernel.
 * Modified as per Marlo to add yield.
 *******************************************************************************
 */
static inline void _schedule(BOOL yield) 
{
    U8  RunPrio,RdyPrio;
    P_OSTCB pRdyTcb,pCurTcb;
   
	
    pCurTcb = TCBRunning;    
    pRdyTcb = TCBRdy;

	if((pRdyTcb==Co_NULL) || (pCurTcb != TCBNext) || (OSSchedLock >1) || (OSIntNesting >0))
	{
		return;
	}
    
	TaskSchedReq = Co_FALSE;
    RunPrio = pCurTcb->prio;
    RdyPrio = pRdyTcb->prio;

	/* Is Running task status was changed? */
    if(pCurTcb->state != TASK_RUNNING)	
    {
        TCBNext        = pRdyTcb;   /* Yes,set TCBNext and reorder READY list */
        pRdyTcb->state = TASK_RUNNING;
        RemoveFromTCBRdyList(pRdyTcb);
    }

    else if(RdyPrio < RunPrio )     /* Is higher PRI task coming in?          */
    {
        TCBNext        = pRdyTcb;   /* Yes,set TCBNext and reorder READY list */
        InsertToTCBRdyList(pCurTcb);
		RemoveFromTCBRdyList(pRdyTcb);
        pRdyTcb->state = TASK_RUNNING;
    }
    
#if CFG_ROBIN_EN >0                 /* Is time for robinning                  */                            
	else if ((RunPrio == RdyPrio) && (yield || (OSCheckTime == OSTickCnt)))  
#else /* simple yield */  
    else if ((RunPrio == RdyPrio) && yield)  
#endif  
    {
        TCBNext        = pRdyTcb;   /* Yes,set TCBNext and reorder READY list */
        InsertToTCBRdyList(pCurTcb);
		RemoveFromTCBRdyList(pRdyTcb);
        pRdyTcb->state = TASK_RUNNING;
    }
    else
    {								    
        return;	
    }
    
#if CFG_ROBIN_EN >0
    if(TCBNext->prio == TCBRdy->prio)  /* Reset OSCheckTime for task robinnig */
        OSCheckTime = OSTickCnt + TCBNext->timeSlice;
#endif
    
  
#if CFG_STK_CHECKOUT_EN > 0                       /* Is stack overflow?       */
    if((pCurTcb->stkPtr < pCurTcb->stack)||(*(U32*)(pCurTcb->stack) != MAGIC_WORD))       
    {									
        CoStkOverflowHook(pCurTcb->taskID);       /* Yes,call handler         */		
    }   
#endif
 	
    SwitchContext();                              /* Call task context switch */
}

void Schedule(void)  
{  
    _schedule(Co_FALSE);  
}  


/**
 *******************************************************************************
 * @brief      Assign a TCB to task being created	 					
 * @param[in]  None     
 * @param[out] None     
 * 	 
 * @retval     XXXX							 
 *
 * @par Description
 * @details    This function is called to assign a task control block for task 
 *              being created.
 *******************************************************************************
 */
static P_OSTCB AssignTCB(void)
{
    P_OSTCB	ptcb;
    
    OsSchedLock();                      /* Lock schedule                      */
    if(FreeTCB == Co_NULL)                 /* Is there no free TCB               */
    {
        OsSchedUnlock();                /* Yes,unlock schedule                */
        return Co_NULL;                    /* Error return                       */
    }	
	ptcb    = FreeTCB;          /* Yes,assgin free TCB for this task  */    
	/* Set next item as the head of free TCB list                     */
    FreeTCB = FreeTCB->TCBnext; 
	OsSchedUnlock();
	return ptcb;
}


/**
 *******************************************************************************
 * @brief      Create a task	   
 * @param[in]  task       Task code entry.
 * @param[in]  argv       The parameter passed to task.
 * @param[in]  parameter  Task priority + stack size + time slice + isWaitting.
 * @param[in]  stk        Pointer to stack top of task.
 * @param[out] None   
 * @retval     E_CREATE_FAIL    Fail to create a task .
 * @retval     others           Valid task id.				 
 *
 * @par Description
 * @details    This function is called by application to create a task,return a id 
 *             to mark this task.
 *******************************************************************************
 */
OS_TID CreateTask(FUNCPtr task,void *argv,U32 parameter,OS_STK *stk)
{
    OS_STK* stkTopPtr;
    P_OSTCB ptcb;
    U8      prio;
#if CFG_ROBIN_EN >0	
    U16     timeSlice;
#endif
   
#if CFG_STK_CHECKOUT_EN >0              /* Check validity of parameter        */
    U16 sktSz;
    sktSz = (parameter&0xfff00)>>8;    
#endif
    prio = parameter&0xff;

#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(task == Co_NULL)
    {
        return E_CREATE_FAIL;
    }
    if(stk == Co_NULL)
    {
        return E_CREATE_FAIL;
    }
    if(prio > CFG_LOWEST_PRIO)
    {
        return E_CREATE_FAIL;		
    }
#if CFG_STK_CHECKOUT_EN >0
    if(sktSz < 20)
    {
        return E_CREATE_FAIL;		
    }
#endif	  // CFG_STK_CHECKOUT_EN
#endif	  // CFG_PAR_CHECKOUT_EN

#if CFG_TASK_SCHEDULE_EN == 0
	if(TCBRunning != Co_NULL)
		 return E_CREATE_FAIL;	
#endif   

    stkTopPtr = InitTaskContext(task,argv,stk);   /* Initialize task context. */
    
    ptcb = AssignTCB();                 /* Get free TCB to use                */
    
    if(ptcb == Co_NULL)                    /* Is free TCB equal to Co_NULL?         */
    {
        return E_CREATE_FAIL;           /* Yes,error return                   */
    }
    
    ptcb->stkPtr = stkTopPtr;           /* Initialize TCB as user set         */
    ptcb->prio   = prio;
#if CFG_STK_CHECKOUT_EN >0
    ptcb->stack = stk+1 - sktSz; /* Set bottom stack for stack overflow check */
    *(U32*)(ptcb->stack) = MAGIC_WORD;
#endif	

#if CFG_TASK_WAITTING_EN >0
    ptcb->delayTick	= INVALID_VALUE;	
#endif		 

#if CFG_TASK_SCHEDULE_EN == 0
	ptcb->taskFuc = task;
	ptcb->taskStk = stk;
#endif     
    ptcb->TCBnext = Co_NULL;               /* Initialize TCB link in READY list  */
    ptcb->TCBprev = Co_NULL;

#if CFG_ROBIN_EN >0						/* Set task time slice for task robin */
    timeSlice = (parameter&0x7fff0000)>>20; 
    if(timeSlice == 0)
    {
        timeSlice = CFG_TIME_SLICE;
    }
    ptcb->timeSlice = timeSlice;
#endif

#if CFG_FLAG_EN > 0
    ptcb->pnode = Co_NULL;                 /* Initialize task as no flag waiting */
#endif

#if CFG_EVENT_EN > 0
    ptcb->eventID  = INVALID_ID;      	/* Initialize task as no event waiting*/
    ptcb->pmail    = Co_NULL;
    ptcb->waitNext = Co_NULL;
    ptcb->waitPrev = Co_NULL;
#endif

#if CFG_MUTEX_EN > 0
    /* Initialize task as no mutex holding or waiting                         */
    ptcb->mutexID = INVALID_ID; 
#endif 

#if CFG_ORDER_LIST_SCHEDULE_EN ==0
	ActiveTaskPri(prio);	
#endif	

	if((parameter>>31) == 0)			/* Is task in waitting state?         */
	{									/* No,set it into ready list          */
		OsSchedLock();                  /* Lock schedule                      */
		InsertToTCBRdyList(ptcb);       /* Insert into the READY list         */
	    OsSchedUnlock();                /* Unlock schedule                    */
	}
	else
	{									/* Yes,Set task status as TASK_WAITING*/
		ptcb->state   = TASK_WAITING;	
	}
    return ptcb->taskID;                /* Return task ID                     */
}


/**
 *******************************************************************************
 * @brief      Delete Task	 
 * @param[in]  taskID      Task ID 
 * @param[out] None  
 * @retval     E_INVALID_ID      Invalid task ID.	 
 * @retval     E_PROTECTED_TASK  Protected task in OS.	 
 * @retval     E_OK              Delete successful.	
 *
 * @par Description
 * @details    This function is called to delete assign task.	 
 *******************************************************************************
 */
StatusType CoDelTask(OS_TID taskID)
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
    if(ptcb->state == TASK_DORMANT)
    {
        return E_INVALID_ID;
    }
#endif
    if(taskID == 0)                     /* Is idle task?                      */
    {											 
        return E_PROTECTED_TASK;        /* Yes,error return                   */
    }    
    
    if(ptcb->state == TASK_RUNNING)     /* Is task running?                   */
    {
        if(OSSchedLock != 0)            /* Yes,is OS lock?                    */
        {
            return E_OS_IN_LOCK;        /* Yes,error return                   */
        }	
    }
		
#if CFG_MUTEX_EN >0                     /* Do task hold mutex?                */
    if(ptcb->mutexID != INVALID_ID)
	{
        if(MutexTbl[ptcb->mutexID].taskID == ptcb->taskID)
        {                               /* Yes,leave the mutex                */
            CoLeaveMutexSection(ptcb->mutexID);
        }
    }
	
#endif	

    OsSchedLock();                      /* Lock schedule                      */
    
    if(ptcb->state == TASK_READY)       /* Is task in READY list?             */
    {
        RemoveFromTCBRdyList(ptcb);     /* Yes,remove task from the READY list*/
    }

#if CFG_TASK_WAITTING_EN > 0 
    else if(ptcb->state == TASK_WAITING)/* Is task in the WAITING list?       */
    {
        /* Yes,Is task in delay list? */
        if(ptcb->delayTick != INVALID_VALUE)			         
        {
            RemoveDelayList(ptcb);      /* Yes,remove task from READY list    */
        }

#if CFG_EVENT_EN > 0
        if(ptcb->eventID != INVALID_ID) /* Is task in event waiting list?     */
        {		
            /* Yes,remove task from event waiting list                        */
            RemoveEventWaittingList(ptcb);	
        }
#endif

#if CFG_FLAG_EN > 0
        if(ptcb->pnode != Co_NULL)         /* Is task in flag waiting list?      */
        {
            /* Yes,remove task from flag waiting list                         */
            RemoveLinkNode(ptcb->pnode);	
        }
#endif

#if CFG_MUTEX_EN >0
        if(ptcb->mutexID != INVALID_ID) /* Is task in mutex waiting list?     */
        {
            RemoveMutexList(ptcb);  /* Yes,remove task from mutex waiting list*/
        }
#endif
	  }
#endif
    ptcb->state   = TASK_DORMANT;       /* Release TCB                        */
	TaskSchedReq  = Co_TRUE;

#if CFG_ORDER_LIST_SCHEDULE_EN ==0
	DeleteTaskPri(ptcb->prio);	
#endif	

#if CFG_TASK_SCHEDULE_EN >0
    ptcb->TCBnext = FreeTCB;
    FreeTCB       = ptcb;
#endif
    OsSchedUnlock();                    /* Unlock schedule                    */
    return E_OK;                        /* return OK                          */
}


/**
 *******************************************************************************
 * @brief      Exit Task	   
 * @param[in]  None 
 * @param[out] None  
 * @retval     None			 
 *
 * @par Description
 * @details    This function is called to exit current task.	 
 *******************************************************************************
 */
void CoExitTask(void)
{
    CoDelTask(TCBRunning->taskID);      /* Call task delete function          */
}


#if CFG_TASK_SCHEDULE_EN ==0
/**
 *******************************************************************************
 * @brief      Activate Task	   
 * @param[in]  taskID      Task ID 
 * @param[in]  argv        Task argv 
 * @param[out] None  
 * @retval     E_INVALID_ID      Invalid task ID.	  
 * @retval     E_OK              Activate task successful.			 
 *
 * @par Description
 * @details    This function is called to activate current task.	 
 *******************************************************************************
 */
StatusType CoActivateTask(OS_TID taskID,void *argv)
{
	P_OSTCB ptcb;
	OS_STK* stkTopPtr;
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(taskID >= CFG_MAX_USER_TASKS + SYS_TASK_NUM)
    {
        return E_INVALID_ID;
    }
#endif
	ptcb = &TCBTbl[taskID];
#if CFG_PAR_CHECKOUT_EN >0
	if(ptcb->stkPtr == Co_NULL)
		return E_INVALID_ID;
#endif
	if(ptcb->state != TASK_DORMANT)	
		return E_OK;


									    /* Initialize task context. */
	stkTopPtr = InitTaskContext(ptcb->taskFuc,argv,ptcb->taskStk);   
        
    ptcb->stkPtr = stkTopPtr;           /* Initialize TCB as user set         */
	OsSchedLock();                      /* Lock schedule                      */
	InsertToTCBRdyList(ptcb);           /* Insert into the READY list         */
    OsSchedUnlock();                    /* Unlock schedule                    */
	return E_OK;
}
#endif


/**
 *******************************************************************************
 * @brief      Get current task id	  
 * @param[in]  None
 * @param[out] None
 * @retval     ID of the current task.			 
 *
 * @par Description
 * @details    This function is called to get current task id.	 
 *******************************************************************************
 */
OS_TID CoGetCurTaskID(void)
{
    return (TCBRunning->taskID);        /* Return running task ID             */
}

#if CFG_TASK_SUSPEND_EN >0
/**
 *******************************************************************************
 * @brief      Suspend Task	  
 * @param[in]  taskID    ID of task that want to suspend.
 * @param[out] None  
 * @retval     E_OK                  Task suspend successful. 
 * @retval     E_INVALID_ID          Invalid event ID. 
 * @retval     E_PROTECTED_TASK      Can't suspend idle task. 
 * @retval     E_ALREADY_IN_WAITING  Task now in waiting state.
 	 
 *
 * @par Description
 * @details    This function is called to exit current task.	 
 *******************************************************************************
 */
StatusType CoSuspendTask(OS_TID taskID)
{
    P_OSTCB ptcb;

	if(taskID == 0)                     /* Is idle task?                      */
    {											 
        return E_PROTECTED_TASK;        /* Yes,error return                   */
    }   
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(taskID >= CFG_MAX_USER_TASKS + SYS_TASK_NUM)
    {
        return E_INVALID_ID;
    }
#endif
	ptcb = &TCBTbl[taskID];
#if CFG_PAR_CHECKOUT_EN >0  
    if(ptcb->state == TASK_DORMANT)
    {
        return E_INVALID_ID;
    }
#endif
    if(OSSchedLock != 0)
    {
        return E_OS_IN_LOCK;
    }
    if(ptcb->state == TASK_WAITING)     /* Is task in WAITING list?           */
    {
        return E_ALREADY_IN_WAITING;    /* Yes,error return                   */
    }
    
    OsSchedLock();	
    if(ptcb != TCBRunning)              /* Is runing task?                    */
    {
        RemoveFromTCBRdyList(ptcb);     /* No,Remove task from READY list     */
    }
	else
	{
		TaskSchedReq = Co_TRUE;
	}

    ptcb->state = TASK_WAITING;	        /* Set task status as TASK_WAITING    */
    OsSchedUnlock();                    /* Call task schedule                 */
    return E_OK;                        /* Return OK                          */
}


/**
 *******************************************************************************
 * @brief      Awake Task	 
 * @param[in]  taskID      ID of task that will been awaked.
 * @param[out] None  
 * @retval     E_OK                 Task awake successful. 
 * @retval     E_INVALID_ID         Invalid task ID.
 * @retval     E_TASK_NOT_WAITING   Task now not in waiting state.
 * @retval     E_TASK_WAIT_OTHER    Task now waiting other awake event.
 * @retval     E_PROTECTED_TASK     Idle task mustn't be awaked. 	 
 *
 * @par Description
 * @details    This function is called to awake current task.	 
 *******************************************************************************
 */
StatusType CoAwakeTask(OS_TID taskID)
{
    P_OSTCB ptcb;
	
 	if(taskID == 0)                     /* Is idle task?                      */
    {											 
        return E_PROTECTED_TASK;        /* Yes,error return                   */
    } 
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(taskID >= CFG_MAX_USER_TASKS + SYS_TASK_NUM)
    {
        return E_INVALID_ID;
    }
#endif
	ptcb = &TCBTbl[taskID];
#if CFG_PAR_CHECKOUT_EN >0  
    if(ptcb->state == TASK_DORMANT)
    {
        return E_INVALID_ID;
    }
#endif
    
    if(ptcb->state != TASK_WAITING)     /* Is task in WAITING list            */
    {
        return E_TASK_NOT_WAITING;      /* No,error return                    */
    }	

#if CFG_TASK_WAITTING_EN > 0
    if(ptcb->delayTick != INVALID_VALUE)/* Is task in READY list              */
    {
        return E_TASK_WAIT_OTHER;       /* Yes,error return                   */
    }

#if CFG_FLAG_EN > 0
    if(ptcb->pnode != Co_NULL)             /* Is task in flag waiting list       */
    {
        return E_TASK_WAIT_OTHER;       /* Yes,error return                   */
    }
#endif

#if CFG_EVENT_EN>0
    if(ptcb->eventID != INVALID_ID)     /* Is task in event waiting list      */
    {
        return E_TASK_WAIT_OTHER;       /* Yes,error return                   */
    }
#endif	

#if CFG_MUTEX_EN > 0
    if(ptcb->mutexID != INVALID_ID)     /* Is task in mutex waiting list      */
    {
        return E_TASK_WAIT_OTHER;       /* Yes,error return                   */
    }
#endif

#endif	  //CFG_TASK_WAITTING_EN

    /* All no,so WAITING state was set by CoSuspendTask()                     */
    OsSchedLock();                      /* Lock schedule                      */
	InsertToTCBRdyList(ptcb);           /* Insert the task into the READY list*/
    OsSchedUnlock();                    /* Unlock schedule                    */
    return E_OK;                        /* return OK                          */
}
#endif
/*
*	Added to add yielf function
*/

void CoYield(void)  
{  
    _schedule(Co_TRUE);
}  
