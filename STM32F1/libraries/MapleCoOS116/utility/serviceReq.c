/**
 *******************************************************************************
 * @file       serviceReq.c
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      servive request management implementation code of CooCox CoOS kernel.	
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

#if (CFG_TASK_WAITTING_EN > 0) || (CFG_TMR_EN >0)

#if CFG_MAX_SERVICE_REQUEST > 0
/*---------------------------- Variable Define -------------------------------*/
SRQ   ServiceReq = {0,0};             /*!< ISR server request queue         */		     
#endif       
BOOL  IsrReq   = Co_FALSE;
#if (CFG_TASK_WAITTING_EN > 0)
BOOL  TimeReq  = Co_FALSE;                 /*!< Time delay dispose request       */
#endif

#if CFG_TMR_EN  > 0
BOOL  TimerReq = Co_FALSE;                 /*!< Timer dispose request            */
#endif

/**
 *******************************************************************************
 * @brief      Insert into service requst queue	 
 * @param[in]  type     Service request type.	
 * @param[in]  id       Service request event id,event id/flag id.
 * @param[in]  arg      Service request argument. 
 * @param[out] None 
 * 	 
 * @retval     Co_FALSE    Successfully insert into service request queue.
 * @retval     Co_TRUE     Failure to insert into service request queue.
 *
 * @par Description		 
 * @details    This function be called to insert a requst into service request	
 *             queue.
 * @note 
 *******************************************************************************
 */
#if (CFG_MAX_SERVICE_REQUEST > 0)
BOOL InsertInSRQ(U8 type,U8 id,void* arg)
{
    P_SQC   pcell;
	U8 cnt;
	U8 heed;
    IRQ_DISABLE_SAVE();
    if (ServiceReq.cnt >= CFG_MAX_SERVICE_REQUEST)
    {
        IRQ_ENABLE_RESTORE ();

        return Co_FALSE;                   /* Error return                       */
    }
	cnt = Inc8(&ServiceReq.cnt);
	heed = ServiceReq.head;
    IsrReq = Co_TRUE;
    pcell = &ServiceReq.cell[((cnt+heed)%CFG_MAX_SERVICE_REQUEST)];/*the tail */
    pcell->type = type;                 /* Save service request type,         */
    pcell->id   = id;                   /* event id                           */
    pcell->arg  = arg;                  /* and parameter                      */
    IRQ_ENABLE_RESTORE ();

    return Co_TRUE;                        /* Return OK                          */
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
    SQC cell;

#endif

#if (CFG_TASK_WAITTING_EN > 0)
    if(TimeReq == Co_TRUE)                 /* Time delay request?                */
    {
        TimeDispose();                  /* Yes,call handler                   */
        TimeReq = Co_FALSE;                /* Reset time delay request Co_FALSE     */
    }
#endif
#if CFG_TMR_EN  > 0
    if(TimerReq == Co_TRUE)                /* Timer request?                     */
    {
        TmrDispose();                   /* Yes,call handler                   */
        TimerReq = Co_FALSE;               /* Reset timer request Co_FALSE          */
    }
#endif

#if CFG_MAX_SERVICE_REQUEST > 0

    while (ServiceReq.cnt != 0)
    {
        IRQ_DISABLE_SAVE ();            /* need to protect the following      */
        cell = ServiceReq.cell[ServiceReq.head];  /* extract one cell         */
        ServiceReq.head = (ServiceReq.head + 1) % /* move head (pop)          */
                     CFG_MAX_SERVICE_REQUEST;
        ServiceReq.cnt--;
        IRQ_ENABLE_RESTORE ();          /* now use the cell copy              */

        switch(cell.type)               /* Judge service request type         */
        {
#if CFG_SEM_EN > 0
        case SEM_REQ:                   /* Semaphore post request,call handler*/
            CoPostSem(cell.id);
            break;
#endif
#if CFG_MAILBOX_EN > 0
        case MBOX_REQ:                  /* Mailbox post request,call handler  */
            CoPostMail(cell.id, cell.arg);
            break;
#endif
#if CFG_FLAG_EN > 0
        case FLAG_REQ:                  /* Flag set request,call handler      */
            CoSetFlag(cell.id);
            break;
#endif
#if CFG_QUEUE_EN > 0
        case QUEUE_REQ:                 /* Queue post request,call handler    */
            CoPostQueueMail(cell.id, cell.arg);
            break;
#endif
        default:                        /* Others,break                       */
            break;
        }
    }
#endif
    IRQ_DISABLE_SAVE ();                /* need to protect the following      */

    if (ServiceReq.cnt == 0)            /* another item in the queue already? */
    {
        IsrReq = Co_FALSE;                 /* queue still empty here             */
    }
    IRQ_ENABLE_RESTORE ();              /* now it is done and return          */
}

#endif

