/**
 *******************************************************************************
 * @file       CoOS.h
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      API header file of CooCox CoOS.
 * @details    This file including all API functions's declare of CooCox CoOS.	
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

#ifndef _CCRTOS_H
#define _CCRTOS_H
#include "OsConfig.h"
/*---------------------------- Type Define  ----------------------------------*/
typedef signed   char      S8;              
typedef unsigned char      U8;	
typedef short              S16;
typedef unsigned short     U16;
typedef int                S32;
typedef unsigned int       U32;
typedef long long          S64;
typedef unsigned long long U64;
typedef unsigned char      BIT;
typedef unsigned char      BOOL;
typedef unsigned int       OS_STK;
typedef U8                 OS_TID;
typedef U8                 OS_TCID;
typedef U8                 OS_MutexID;
typedef U8                 OS_EventID;
typedef U8                 OS_FlagID;
typedef U8                 OS_MMID;
typedef U8                 StatusType;
typedef U16                OS_VER;
typedef void               (*FUNCPtr)(void*);
typedef void               (*vFUNCPtr)(void);


/*---------------------------- Constant Define -------------------------------*/
#ifndef Co_NULL
#define Co_NULL          ((void *)0)
#endif

#ifndef Co_FALSE
#define Co_FALSE         (0)
#endif

#ifndef Co_TRUE
#define Co_TRUE          (1)
#endif


/*---------------------------- Error Codes   ---------------------------------*/
#define E_CREATE_FAIL         (StatusType)-1
#define E_OK                  (StatusType)0
#define E_INVALID_ID          (StatusType)1
#define E_INVALID_PARAMETER   (StatusType)2
#define E_CALL                (StatusType)3
#define E_TASK_WAITING        (StatusType)4
#define E_TIMEOUT             (StatusType)5
#define E_SEM_FULL            (StatusType)6
#define E_MBOX_FULL           (StatusType)7
#define E_QUEUE_FULL          (StatusType)8
#define E_SEM_EMPTY           (StatusType)9
#define E_MBOX_EMPTY          (StatusType)10
#define E_QUEUE_EMPTY         (StatusType)11
#define E_FLAG_NOT_READY      (StatusType)12
#define E_ALREADY_IN_WAITING  (StatusType)13
#define E_TASK_NOT_WAITING    (StatusType)14
#define E_TASK_WAIT_OTHER     (StatusType)15
#define E_EXCEED_MAX_NUM      (StatusType)16
#define E_NOT_IN_DELAY_LIST   (StatusType)17
#define E_SEV_REQ_FULL        (StatusType)18	
#define E_NOT_FREE_ALL        (StatusType)19	
#define E_PROTECTED_TASK      (StatusType)20 
#define E_OS_IN_LOCK          (StatusType)21												


/*---------------------------- Wait Opreation type  --------------------------*/
#define OPT_WAIT_ALL          0         /*!< Wait for all flags.              */
#define OPT_WAIT_ANY          1         /*!< Wait for any one of flags.       */
#define OPT_WAIT_ONE          2         /*!< Waot for one flag.               */	


/*---------------------------- Delete Opreation type  ------------------------*/
#define OPT_DEL_NO_PEND       0         /*!< Delete when no task waitting for */
#define OPT_DEL_ANYWAY        1         /*!< Delete always.                   */


/*---------------------------- Timer Types  ----------------------------------*/
#if CFG_TMR_EN >0
#define TMR_TYPE_ONE_SHOT     0         /*!< Timer counter type: One-shot     */
#define TMR_TYPE_PERIODIC     1         /*!< Timer counter type: Periodic     */
#endif


/*---------------------------- Event Control ---------------------------------*/
#if CFG_EVENT_EN >0
#define EVENT_SORT_TYPE_FIFO  (U8)0x01  /*!< Insert a event by FIFO           */
#define EVENT_SORT_TYPE_PRIO  (U8)0x02  /*!< Insert a event by prio           */
#endif


/*---------------------------- Function declare-------------------------------*/

/* Implement in file "core.c"      */
extern void    CoInitOS(void);          /*!< Initialize OS                    */
extern void    CoStartOS(void);         /*!< Start multitask                  */
extern void    CoEnterISR(void);        /*!< Enter a ISR                      */
extern void    CoExitISR(void);         /*!< Exit a ISR                       */
extern void    CoSchedLock(void);
extern void    CoSchedUnlock(void);
extern OS_VER  CoGetOSVersion(void);    /*!< Get OS version value             */


/* Implement in file "task.c"      */
#define CoCreateTask(task,argv,prio,stk,stkSz)              \
            CreateTask(task,argv,(prio)|(((stkSz)<<8) &0x000FFF00 ),stk)


#define CoCreateTaskEx(task,argv,prio,stk,stkSz,timeSlice,isWaitting)  \
           CreateTask(task,argv,(prio)|(((stkSz)<<8)&0x000FFF00)|(((timeSlice)<<20)&0x7FF00000)|((isWaitting<<31)&0x80000000),stk)

extern void        CoExitTask(void);
extern OS_TID      CoGetCurTaskID(void);
extern StatusType  CoDelTask(OS_TID taskID);
extern StatusType  CoActivateTask(OS_TID taskID,void *argv);
extern StatusType  CoAwakeTask(OS_TID taskID);
extern StatusType  CoSuspendTask(OS_TID taskID);
extern StatusType  CoSetPriority(OS_TID taskID,U8 priority);
extern OS_TID      CreateTask(FUNCPtr task,void *argv,U32 parameter,OS_STK *stk);
extern void        CoYield(void);

/* Implement in file "time.c"      */
extern U64         CoGetOSTime(void);
extern StatusType  CoTickDelay(U32 ticks);
extern StatusType  CoResetTaskDelayTick(OS_TID taskID,U32 ticks);
extern StatusType  CoTimeDelay(U8 hour,U8 minute,U8 sec,U16 millsec);


/* Implement in file "timer.c"     */ 
extern StatusType  CoDelTmr(OS_TCID tmrID);
extern StatusType  CoStopTmr(OS_TCID tmrID);
extern StatusType  CoStartTmr(OS_TCID tmrID);
extern U32         CoGetCurTmrCnt(OS_TCID tmrID,StatusType* perr);
extern StatusType  CoSetTmrCnt(OS_TCID tmrID,U32 tmrCnt,U32 tmrReload);
extern OS_TCID     CoCreateTmr(U8 tmrType, U32 tmrCnt, U32 tmrReload, vFUNCPtr func);


/* Implement in file "kernelHeap.c"*/
extern void*       CoKmalloc(U32 size);
extern void        CoKfree(void* memBuf);


/* Implement in file "mm.c"        */
extern void*       CoGetMemoryBuffer(OS_MMID mmID);
extern StatusType  CoDelMemoryPartition(OS_MMID mmID);
extern StatusType  CoFreeMemoryBuffer(OS_MMID mmID,void* buf);
extern U32         CoGetFreeBlockNum(OS_MMID mmID,StatusType* perr);
extern OS_MMID     CoCreateMemPartition(U8* memBuf,U32 blockSize,U32 blockNum);

/* Implement in file "mutex.c"     */
extern OS_MutexID  CoCreateMutex(void);
extern StatusType  CoEnterMutexSection(OS_MutexID mutexID);
extern StatusType  CoLeaveMutexSection(OS_MutexID mutexID);


/* Implement in file "sem.c"       */
extern StatusType  CoPostSem(OS_EventID id);
extern StatusType  CoAcceptSem(OS_EventID id);
extern StatusType  isr_PostSem(OS_EventID id);
extern StatusType  CoDelSem(OS_EventID id,U8 opt);
extern StatusType  CoPendSem(OS_EventID id,U32 timeout);
extern OS_EventID  CoCreateSem(U16 initCnt,U16 maxCnt,U8 sortType);


/* Implement in file "mbox.c"      */
extern OS_EventID  CoCreateMbox(U8 sortType);
extern StatusType  CoDelMbox(OS_EventID id,U8 opt);
extern StatusType  CoPostMail(OS_EventID id,void* pmail);
extern StatusType  isr_PostMail(OS_EventID id,void* pmail);
extern void*       CoAcceptMail(OS_EventID id,StatusType* perr);
extern void*       CoPendMail(OS_EventID id,U32 timeout,StatusType* perr);


/* Implement in file "queue.c"     */
extern StatusType  CoDelQueue(OS_EventID id,U8 opt);
extern StatusType  CoPostQueueMail(OS_EventID id,void* pmail);
extern StatusType  isr_PostQueueMail(OS_EventID id,void* pmail);
extern void*       CoAcceptQueueMail(OS_EventID id,StatusType* perr);
extern OS_EventID  CoCreateQueue(void **qStart, U16 size ,U8 sortType);
extern void*       CoPendQueueMail(OS_EventID id,U32 timeout,StatusType* perr);



/* Implement in file "flag.c"      */
extern StatusType  CoSetFlag (OS_FlagID id);
extern StatusType  CoClearFlag (OS_FlagID id);
extern StatusType  isr_SetFlag (OS_FlagID id);
extern StatusType  CoDelFlag (OS_FlagID id,U8 opt);
extern StatusType  CoAcceptSingleFlag (OS_FlagID id);
extern StatusType  CoWaitForSingleFlag (OS_FlagID id,U32 timeout);
extern OS_FlagID   CoCreateFlag (BOOL bAutoReset,BOOL bInitialState);
extern U32         CoAcceptMultipleFlags (U32 flags,U8 waitType,StatusType *perr);
extern U32         CoWaitForMultipleFlags (U32 flags,U8 waitType,U32 timeout,StatusType *perr);


/* Implement in file "utility.c"   */
extern StatusType  CoTimeToTick(U8 hour,U8 minute,U8 sec,U16 millsec,U32* ticks);
extern void        CoTickToTime(U32 ticks,U8* hour,U8* minute,U8* sec,U16* millsec);


/* Implement in file "hook.c"      */
extern void        CoIdleTask(void* pdata);
extern void        CoStkOverflowHook(OS_TID taskID);


#endif
