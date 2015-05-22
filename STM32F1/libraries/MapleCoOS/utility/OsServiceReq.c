/**
 *******************************************************************************
 * @file       serviceReq.c
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      servive request management implementation code of CooCox CoOS kernel.	
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

#if (CFG_TASK_WAITTING_EN > 0) || (CFG_TMR_EN >0)

#if CFG_MAX_SERVICE_REQUEST > 0
/*---------------------------- Variable Define -------------------------------*/
SRQ   ServiceReq = {0};                 /*!< ISR server request queue         */		     
#endif       
BOOL  IsrReq   = FALSE;
#if (CFG_TASK_WAITTING_EN > 0)
BOOL  TimeReq  = FALSE;                 /*!< Time delay dispose request       */
#endif

#if CFG_TMR_EN  > 0
BOOL  TimerReq = FALSE;                 /*!< Timer dispose request            */
#endif

/**
 *******************************************************************************
 * @brief      Insert into service requst queue	 
 * @param[in]  type     Service request type.	
 * @param[in]  id       Service request event id,event id/flag id.
 * @param[in]  arg      Service request argument. 
 * @param[out] None 
 * 	 
 * @retval     FALSE    Successfully insert into service request queue. 
 * @retval     TRUE     Failure to insert into service request queue.  
 *
 * @par Description		 
 * @details    This function be called to insert a requst into service request	
 *             queue.
 * @note 
 *******************************************************************************
 */
#if CFG_MAX_SERVICE_REQUEST > 0
BOOL InsertInSRQ(U8 type,U8 id,void* arg)
{
    U16 cnt;
    P_SQC pcell;
    cnt = Inc8(&ServiceReq.cnt);        /* Service request counter decrease   */
    if(cnt >= CFG_MAX_SERVICE_REQUEST)  /* If counter exceed max              */
    {
        return FALSE;                   /* Error return                       */
    }
    IsrReq = TRUE;
    pcell = &ServiceReq.cell[cnt]; /* Save service request type and parameter */
    pcell->type = type;
    pcell->id   = id;
    pcell->arg  = arg;
    return TRUE;                        /* Return OK                          */
}
#endif


/**
 *******************************************************************************
 * @brief      Respond the request in the service request queue.	 
 * @param[in]  None
 * @param[out] None 
 * @retval     None  
 *
 * @par Description		 
 * @details    This function be called to respond the request in the service  
 *             request queue.
 * @note 
 *******************************************************************************
 */
void RespondSRQ(void)
{
#if CFG_MAX_SERVICE_REQUEST > 0
    U16 i;
    P_SQC pcell;
#endif
 
#if (CFG_TASK_WAITTING_EN > 0)
    if(TimeReq == TRUE)                 /* Time delay request?                */
    {
        TimeDispose();                  /* Yes,call handler                   */
        TimeReq = FALSE;                /* Reset time delay request false     */
    }
#endif
#if CFG_TMR_EN  > 0
    if(TimerReq == TRUE)                /* Timer request?                     */
    {
        TmrDispose();                   /* Yes,call handler                   */
        TimerReq = FALSE;               /* Reset timer request false          */
    }
#endif

#if CFG_MAX_SERVICE_REQUEST > 0
    pcell = &ServiceReq.cell[0];  /* Get the head item of service request list*/
    for(i=0;i<ServiceReq.cnt;i++,pcell++) 
    {
        switch(pcell->type)             /* Judge service request type         */
        {
#if CFG_SEM_EN > 0
            case SEM_REQ:               /* Semaphore post request,call handler*/
                  CoPostSem(pcell->id);
                  break;
#endif
#if CFG_MAILBOX_EN > 0
            case MBOX_REQ:              /* Mailbox post request,call handler  */
                  CoPostMail(pcell->id,pcell->arg);
                  break;
#endif
#if CFG_FLAG_EN > 0
            case FLAG_REQ:              /* Flag set request,call handler      */
                  CoSetFlag(pcell->id);
                  break;
#endif	 
#if CFG_QUEUE_EN > 0
            case QUEUE_REQ:             /* Queue post request,call handler    */
				  CoPostQueueMail(pcell->id,pcell->arg);
                  break;
#endif
            default:                    /* Others,break                       */
                  break;
		}
    pcell->type = 0;                    /* Initialize the service request cell*/
    pcell->id   = 0;
    pcell->arg  = 0;	
	}
    ServiceReq.cnt = 0;               /* Initialize the service request queue */
#endif
    IsrReq = FALSE;
}

#endif

