/**
 *******************************************************************************
 * @file       timer.c
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      timer management implementation code of CooCox CoOS kernel.	
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
#if CFG_TMR_EN > 0

TmrCtrl    TmrTbl[CFG_MAX_TMR]= {{0}};/*!< Table which save timer control block.*/
P_TmrCtrl  TmrList     = NULL;      /*!< The header of the TmrCtrl list.      */
U32        TmrIDVessel = 0;         /*!< Timer ID container.                  */


/**
 *******************************************************************************
 * @brief      Insert a timer into the timer list	   
 * @param[in]  tmrID    Specify timer ID which insertted.		 
 * @param[out] None  
 * @retval     E_INVALID_ID  Timer ID passed was invalid,insert fail.
 * @retval     E_OK          Insert successful.			 
 *
 * @par Description
 * @details    This function is called to insert a timer into the timer list.  
 *******************************************************************************
 */
static void InsertTmrList(OS_TCID tmrID)
{
    P_TmrCtrl pTmr;
    S32 deltaTicks;
    U32 tmrCnt;
    tmrCnt = TmrTbl[tmrID].tmrCnt;      /* Get timer time                     */
    
    if(tmrCnt == 0)                     /* Is timer time==0?                  */
    {
        return;                         /* Do nothing,return                  */
    }
    
    OsSchedLock();                      /* Lock schedule                      */
    if(TmrList == NULL)                 /* Is no item in timer list?          */
    {
        TmrList = &TmrTbl[tmrID];       /* Yes,set this as first item         */
    }
    else                  /* No,find correct place ,and insert inserted timer */
    {								    
      	pTmr       = TmrList; 
      	deltaTicks = tmrCnt;            /* Get timer tick                     */
      	
      	/* find correct place */
      	while(pTmr != NULL)
      	{				    
            deltaTicks -= pTmr->tmrCnt; /* Get ticks with previous item       */
            if(deltaTicks < 0)          /* Is delta ticks<0?                  */  
            {	
                /* Yes,get correct place */
                if(pTmr->tmrPrev!= NULL)/* Is head item of timer list?        */
                {	
                    /* No,insert into */
                    pTmr->tmrPrev->tmrNext = &TmrTbl[tmrID]; 
                    TmrTbl[tmrID].tmrPrev  = pTmr->tmrPrev;
                    TmrTbl[tmrID].tmrNext  = pTmr;
                    pTmr->tmrPrev          = &TmrTbl[tmrID];
                }
                else                    /* Yes,set task as first item         */ 	
                {
                    TmrTbl[tmrID].tmrNext = TmrList;
                    TmrList->tmrPrev      = &TmrTbl[tmrID];
                    TmrList               = &TmrTbl[tmrID];
                }
                TmrTbl[tmrID].tmrCnt            = TmrTbl[tmrID].tmrNext->tmrCnt+deltaTicks;
                TmrTbl[tmrID].tmrNext->tmrCnt  -= TmrTbl[tmrID].tmrCnt; 
                break;	
            }
            /* Is last item in list? */									
            else if((deltaTicks >= 0) && (pTmr->tmrNext == NULL))
            {	
                /* Yes,insert into */
                TmrTbl[tmrID].tmrPrev = pTmr;
                pTmr->tmrNext         = &TmrTbl[tmrID];	
                TmrTbl[tmrID].tmrCnt  = deltaTicks;
                break;	
            }
            pTmr = pTmr->tmrNext;       /* Get the next item in timer list    */	
      	}
    }
    OsSchedUnlock();                    /* Unlock schedule                    */
}


/**
 *******************************************************************************
 * @brief      Remove a timer from the timer list	  
 * @param[in]  tmrID    Specify ID for a timer which removed form timer list.	 
 * @param[out] None 
 * @retval     None
 *
 * @par Description
 * @details    This function is called to remove a timer from the timer list. 
 *******************************************************************************
 */
static void RemoveTmrList(OS_TCID tmrID)
{
    P_TmrCtrl pTmr;
    
    pTmr = &TmrTbl[tmrID];
    
    OsSchedLock();                      /* Lock schedule                      */
    
    /* Is there only one item in timer list?                                  */
    if((pTmr->tmrPrev == NULL) && (pTmr->tmrNext == NULL))
    {		
        TmrList = NULL;                 /* Yes,set timer list as NULL         */ 	
    }
    else if(pTmr->tmrPrev == NULL)      /* Is the first item in timer list?   */
    {   /* Yes,remove timer from list,and reset timer list                    */
        TmrList  = pTmr->tmrNext;
        TmrList->tmrPrev = NULL;
        pTmr->tmrNext->tmrCnt += pTmr->tmrCnt;
        pTmr->tmrNext    = NULL;  
    }
    else if(pTmr->tmrNext == NULL)      /* Is the last item in timer list?    */
    {
        /* Yes,remove timer form list */
        pTmr->tmrPrev->tmrNext = NULL;	
        pTmr->tmrPrev = NULL;
    }
    else                                /* No, remove timer from list         */
    {
        pTmr->tmrPrev->tmrNext  =  pTmr->tmrNext;
        pTmr->tmrNext->tmrPrev  =  pTmr->tmrPrev;
        pTmr->tmrNext->tmrCnt  += pTmr->tmrCnt;
        pTmr->tmrNext = NULL;
        pTmr->tmrPrev = NULL;
    }
    OsSchedUnlock();                    /* Unlock schedule                    */
}


/**
 *******************************************************************************
 * @brief      Create a timer	   
 * @param[in]  tmrType     Specify timer's type.		 
 * @param[in]  tmrCnt      Specify timer initial counter value.  
 * @param[in]  tmrReload   Specify timer reload value.
 * @param[in]  func        Specify timer callback function entry.
 * @param[out] None
 * @retval     E_CREATE_FAIL   Create timer fail.
 * @retval     others          Create timer successful.			 
 *
 * @par Description
 * @details    This function is called to create a timer.
 *******************************************************************************
 */
OS_TCID CoCreateTmr(U8 tmrType, U32 tmrCnt, U32 tmrReload, vFUNCPtr func)
{
    U8 i;
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if((tmrType != TMR_TYPE_ONE_SHOT) && (tmrType != TMR_TYPE_PERIODIC))
    {
        return E_CREATE_FAIL;	
    }
    if(func == NULL)
    {
        return E_CREATE_FAIL;
    }
#endif
    OsSchedLock();                        /* Lock schedule                    */
    for(i = 0; i < CFG_MAX_TMR; i++)
    {
        if((TmrIDVessel & (1 << i)) == 0) /* Is free timer ID?                */
        {
            TmrIDVessel |= (1<<i);        /* Yes,assign ID to this timer      */
            OsSchedUnlock();              /* Unlock schedule                  */
            TmrTbl[i].tmrID     = i;      /* Initialize timer as user set     */
            TmrTbl[i].tmrType   = tmrType;	
            TmrTbl[i].tmrState  = TMR_STATE_STOPPED;
            TmrTbl[i].tmrCnt    = tmrCnt;
            TmrTbl[i].tmrReload	= tmrReload;
            TmrTbl[i].tmrCallBack = func;
            TmrTbl[i].tmrPrev   = NULL;
            TmrTbl[i].tmrNext   = NULL;
            return i;                     /* Return timer ID                  */
        }
    }
    OsSchedUnlock();                      /* Unlock schedule                  */
    return E_CREATE_FAIL;                 /* Error return                     */
}


/**
 *******************************************************************************
 * @brief      Start counter	 
 * @param[in]  tmrID    Specify a timer which startted.		 
 * @param[out] None 
 * @retval     E_INVALID_ID  The timer id passed was invalid,can't start timer	
 * @retval     E_OK          Insert a timer to timer list and start it successful. 
 *
 * @par Description
 * @details    This function is called to make a timer start countering. 
 *******************************************************************************
 */
StatusType CoStartTmr(OS_TCID tmrID)
{
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(tmrID >= CFG_MAX_TMR)
    {
        return E_INVALID_ID;
    }
    if( (TmrIDVessel & (1<<tmrID)) == 0)
    {
        return E_INVALID_ID;
    }
#endif
    
    if(TmrTbl[tmrID].tmrState == TMR_STATE_RUNNING)   /* Is timer running?    */
    {
        return E_OK;                              /* Yes,do nothing,return OK */
    }
    
    /* No,set timer status as TMR_STATE_RUNNING */
    TmrTbl[tmrID].tmrState = TMR_STATE_RUNNING; 
    InsertTmrList(tmrID);               /* Insert this timer into timer list  */
    return E_OK;                        /* Return OK                          */
}



/**
 *******************************************************************************
 * @brief      Stop countering for a spcify timer	  
 * @param[in]  tmrID    Specify a timer which stopped.	 
 * @param[out] None  	 
 * @retval     E_INVALID_ID  The timer id passed was invalid, stop failure.
 * @retval     E_OK          Stop a timer countering successful.
 *
 * @par Description
 * @details    This function is called to stop a timer from counting. 
 *******************************************************************************
 */
StatusType CoStopTmr(OS_TCID tmrID)
{	
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(tmrID >= CFG_MAX_TMR)
    {
        return E_INVALID_ID;
    }
    if((TmrIDVessel & (1<<tmrID)) == 0)
    {
        return E_INVALID_ID;
    }
#endif
    
    
    if(TmrTbl[tmrID].tmrState == TMR_STATE_STOPPED)/* Does timer stop running?*/
    {
        return E_OK;                    /* Yes,do nothing,return OK           */
    }
    RemoveTmrList(tmrID);             /* No,remove this timer from timer list */
    
    /* Set timer status as TMR_STATE_STOPPED  */
    TmrTbl[tmrID].tmrState = TMR_STATE_STOPPED;	
    return E_OK;                        /* Return OK                          */
}


/**
 *******************************************************************************
 * @brief      Delete a timer	 
 * @param[in]  tmrID     Specify a timer which deleted.		 
 * @param[out] None   
 * @retval     E_INVALID_ID  The timer id passed was invalid,deleted failure.	
 * @retval     E_OK          Delete a timer successful.
 *
 * @par Description
 * @details    This function is called to delete a timer which created before.	
 *******************************************************************************
 */
StatusType CoDelTmr(OS_TCID tmrID)
{
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(tmrID >= CFG_MAX_TMR)
    {
        return E_INVALID_ID;
    }
    if( (TmrIDVessel & (1<<tmrID)) == 0)
    {
        return E_INVALID_ID;
    }
#endif
	
    if(TmrTbl[tmrID].tmrState == TMR_STATE_RUNNING) /* Is timer running?      */
    {
        RemoveTmrList(tmrID);         /* Yes,remove this timer from timer list*/
    }
    TmrIDVessel &=~(1<<tmrID);        /* Release resource that this timer hold*/
    return E_OK;                      /* Return OK                            */
}

 
/**
 *******************************************************************************
 * @brief      Get current counter of specify timer	 
 * @param[in]  tmrID          Specify timer by ID.		 
 * @param[out] E_INVALID_ID   Invalid ID was passed and get counter failure.	  
 * @param[out] E_OK           Get current counter successful.	 
 * @retval     Current counter of a timer which specify by id.			 
 *
 * @par Description
 * @details    This function is called to obtain current counter of specify timer.
 *******************************************************************************
 */
U32 CoGetCurTmrCnt(OS_TCID tmrID,StatusType* perr)
{
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(tmrID >= CFG_MAX_TMR)
    {
        *perr = E_INVALID_ID;
        return 0;
    }
    if((TmrIDVessel & (1<<tmrID)) == 0)
    {
        *perr = E_INVALID_ID;
        return 0;
    }
#endif
    *perr = E_OK;
    return TmrTbl[tmrID].tmrCnt;        /* Return timer counter               */
}


/**
 *******************************************************************************
 * @brief      Setting for a specify timer	  		   	
 * @param[in]  tmrID       Specify timer by ID.
 * @param[in]  tmrCnt      Specify timer counter which need to be set.
 * @param[in]  tmrReload   Specify timer reload value which need to be set.		 
 * @param[out] None  
 * @retval     E_INVALID_ID  The ID passed was invalid,set fail.
 * @retval     E_OK          Set timer counter successful.				 
 *
 * @par Description
 * @details    This function is called to set timer counter and reload value.
 *******************************************************************************
 */
StatusType CoSetTmrCnt(OS_TCID tmrID,U32 tmrCnt,U32 tmrReload)
{
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(tmrID >= CFG_MAX_TMR)
    {
        return E_INVALID_ID;
    }
    if( (TmrIDVessel & (1<<tmrID)) == 0)
    {
        return E_INVALID_ID;
    }
#endif
    TmrTbl[tmrID].tmrCnt    = tmrCnt; /* Reset timer counter and reload value */
    TmrTbl[tmrID].tmrReload = tmrReload;
    								
    if(TmrTbl[tmrID].tmrState == TMR_STATE_RUNNING)   /* Is timer running?    */
    {
        RemoveTmrList(tmrID);           /* Yes,reorder timer in timer list    */
        InsertTmrList(tmrID);	
    }
    return E_OK;                        /* Return OK                          */
}


/**
 *******************************************************************************
 * @brief      Timer counter dispose	   
 * @param[in]  None 	 
 * @param[out] None	 
 * @retval     None	 
 *
 * @par Description
 * @details    This function is called to dispose timer counter.
 *******************************************************************************
 */
void TmrDispose(void)
{
    P_TmrCtrl	pTmr;
    
    pTmr = TmrList;                     /* Get first item of timer list       */
    while((pTmr != NULL) && (pTmr->tmrCnt == 0) )
    {	
        if(pTmr->tmrType == TMR_TYPE_ONE_SHOT)    /* Is a One-shot timer?     */
        {
            /* Yes,remove this timer from timer list                          */
            RemoveTmrList(pTmr->tmrID);
            
            /* Set timer status as TMR_STATE_STOPPED                          */
            pTmr->tmrState = TMR_STATE_STOPPED;
            (pTmr->tmrCallBack)();          /* Call timer callback function   */
        }
        else if(pTmr->tmrType == TMR_TYPE_PERIODIC)   /* Is a periodic timer? */
        {
            /* Yes,remove this timer from timer list                          */
            RemoveTmrList(pTmr->tmrID); 
            pTmr->tmrCnt = pTmr->tmrReload;   /* Reset timer tick             */
            InsertTmrList(pTmr->tmrID);       /* Insert timer into timer list */
            (pTmr->tmrCallBack)();            /* Call timer callback function */
        }
        pTmr = TmrList;	                      /* Get first item of timer list */
    }
}


/**
 *******************************************************************************
 * @brief      Timer counter dispose in ISR	   
 * @param[in]  None 	 
 * @param[out] None	 
 * @retval     None	 
 *
 * @par Description
 * @details    This function is called to dispose timer counter.
 *******************************************************************************
 */
void isr_TmrDispose(void)
{
    if(OSSchedLock > 1)                 /* Is schedule lock?                  */
    {
        IsrReq = TRUE;
        TimerReq  = TRUE;               /* Yes,set timer request true         */
    }
    else
    {
        TmrDispose();                   /* No,call handler                    */
    }
}	 

#endif
