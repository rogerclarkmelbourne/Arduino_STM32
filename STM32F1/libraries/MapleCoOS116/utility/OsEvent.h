/**
 *******************************************************************************
 * @file       OsEvent.h	
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Event management header file
 * @details    This file including some defines and declares related to event 
 *             (semaphore,mailbox,queque) management.
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


#ifndef _EVENT_H
#define _EVENT_H

#define EVENT_TYPE_SEM        (U8)0x01      /*!< Event type:Semaphore.        */
#define EVENT_TYPE_MBOX       (U8)0x02      /*!< Event type:Mailbox.          */
#define EVENT_TYPE_QUEUE      (U8)0x03      /*!< Event type:Queue.            */
#define EVENT_TYPE_INVALID    (U8)0x04      /*!< Invalid event type.          */


/**
 * @struct  EventCtrBlk  event.h	  	
 * @brief   Event control block
 * @details This struct is use to manage event,
 *          e.g. semaphore,mailbox,queue.	
 */
typedef struct EventCtrBlk
{
    void*   eventPtr;                   /*!< Point to mailbox or queue struct */
    U8      id;                         /*!< ECB id                           */
    U8      eventType:4;                /*!< Type of event                    */
    U8      eventSortType:4;            /*!< 0:FIFO 1: Preemptive by prio     */
    U16     eventCounter;               /*!< Counter of semaphore.            */
    U16     initialEventCounter;        /*!< Initial counter of semaphore.    */
    P_OSTCB eventTCBList;               /*!< Task waitting list.              */
}ECB,*P_ECB;

/*---------------------------- Variable declare ------------------------------*/
extern ECB  EventTbl[CFG_MAX_EVENT];    /*!< Table use to save TCB.           */

/*---------------------------- Function declare ------------------------------*/
/*!< Create a event   */
extern P_ECB      CreatEvent(U8 eventType,U8 eventSortType,void* eventPtr);

/*!< Remove a task from wait list */	
extern void       EventTaskToWait(P_ECB pecb,P_OSTCB ptcb);
extern StatusType DeleteEvent(P_ECB pecb,U8 opt);   /*!< Delete a event.      */
extern void       EventTaskToRdy(P_ECB pecb); /*!< Insert a task to ready list*/
extern void       CreateEventList(void);    /*!< Create a event list.         */			
extern void       RemoveEventWaittingList(P_OSTCB ptcb);
#endif  
