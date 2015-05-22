/**
 *******************************************************************************
 * @file       task.h
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      Header file	related to task.
 * @details    This file including some defines and function declare related to task.  	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 

#ifndef _TASK_H
#define _TASK_H

#define  SYS_TASK_NUM   (1)             /*!< System task number.              */

/*---------------------------- Task Status -----------------------------------*/
#define  TASK_READY     0               /*!< Ready status of task.            */	 				
#define  TASK_RUNNING   1               /*!< Running status of task.          */
#define  TASK_WAITING   2               /*!< Waitting status of task.         */
#define  TASK_DORMANT   3               /*!< Dormant status of task.          */ 


#define  INVALID_ID     (U8)0xff
#define  INVALID_VALUE  (U32)0xffffffff
#define  MAGIC_WORD     (U32)0x5a5aa5a5


/**
 * @struct  TCB  task.h  	
 * @brief   Task control blcok.
 * @details This struct use to manage task.	 	
 */
typedef  struct TCB
{
    OS_STK      *stkPtr;                /*!< The current point of task.       */
    U8          prio;                   /*!< Task priority.                   */
    U8          state;                  /*!< TaSk status.                     */
    OS_TID      taskID;                 /*!< Task ID.                         */

#if CFG_MUTEX_EN > 0
    OS_MutexID  mutexID;                /*!< Mutex ID.                        */
#endif
   
#if CFG_EVENT_EN > 0
    OS_EventID  eventID;                /*!< Event ID.                        */
#endif
    
#if CFG_ROBIN_EN >0
    U16         timeSlice;              /*!< Task time slice                  */
#endif
    
#if CFG_STK_CHECKOUT_EN >0
    OS_STK      *stack;                 /*!< The top point of task.           */
#endif
    
#if CFG_EVENT_EN > 0
    void*       pmail;                  /*!< Mail to task.                    */
    struct TCB  *waitNext;  /*!< Point to next TCB in the Event waitting list.*/
    struct TCB  *waitPrev;  /*!< Point to prev TCB in the Event waitting list.*/
#endif

#if CFG_TASK_SCHEDULE_EN == 0
	FUNCPtr     taskFuc;
	OS_STK      *taskStk;
#endif  

    
#if CFG_FLAG_EN > 0
    void*       pnode;                  /*!< Pointer to node of event flag.   */
#endif   

#if CFG_TASK_WAITTING_EN >0
    U32         delayTick;              /*!< The number of ticks which delay. */
#endif    
    struct TCB  *TCBnext;               /*!< The pointer to next TCB.         */
    struct TCB  *TCBprev;               /*!< The pointer to prev TCB.         */

}OSTCB,*P_OSTCB;


/*---------------------------- Variable declare ------------------------------*/
// save tcb ptr that created
extern P_OSTCB  FreeTCB;      /*!< A pointer to free TCB.                     */
extern OSTCB    TCBTbl[CFG_MAX_USER_TASKS+SYS_TASK_NUM];
extern P_OSTCB  TCBRdy;       /*!< A pointer to TCB that is ready status      */
extern P_OSTCB  TCBNext;      /*!< A pointer to TCB next be scheduled.        */	
extern P_OSTCB  TCBRunning;   /*!< A pointer to TCB that is running.          */

extern U64      OSCheckTime;
extern U8       OSIntNesting; /*!< Use to indicate interrupt nesting level.   */   				
extern U8       OSSchedLock;  /*!< Schedule is lock(LOCK) or unlock(UN_LOCK). */  	
extern BOOL     TaskSchedReq;
extern OS_STK   idle_stk[CFG_IDLE_STACK_SIZE];


void  Schedule(void);         /*!< Schedule function                          */
void  IdleTask(void* pdata);  /*!< IDLE task code                             */
void  InsertToTCBRdyList  (P_OSTCB tcbInser);	
void  RemoveFromTCBRdyList(P_OSTCB ptcb);
void  CreateTCBList(void);
#if CFG_ORDER_LIST_SCHEDULE_EN ==0
void  ActiveTaskPri(U8 pri);
void  DeleteTaskPri(U8 pri);
#endif

#endif
