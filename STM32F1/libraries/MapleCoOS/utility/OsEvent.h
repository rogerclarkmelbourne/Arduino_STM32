/**
 *******************************************************************************
 * @file       event.h	
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      Event management header file
 * @details    This file including some defines and declares related to event 
 *             (semaphore,mailbox,queque) management.
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
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
