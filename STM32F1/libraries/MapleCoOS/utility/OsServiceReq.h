
/**
 *******************************************************************************
 * @file       serviceReq.h
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      Header file	related to service request	
 * @details    This file including some defines and function declare related to
 *             service request.
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 


#ifndef _SERVICEREQ_H
#define _SERVICEREQ_H

#if CFG_MAX_SERVICE_REQUEST > 0
#define   SEM_REQ       (U8)0x1
#define   MBOX_REQ      (U8)0x2
#define   FLAG_REQ      (U8)0x3
#define   QUEUE_REQ     (U8)0x4

typedef struct ServiceReqCell
{
    U8      type;
    U8      id;
    void*   arg;
}SQC,*P_SQC;

typedef struct ServiceReqQueue
{
    U8    cnt;
    SQC   cell[CFG_MAX_SERVICE_REQUEST];
}SRQ,*P_SRQ;


extern SRQ  ServiceReq;
extern BOOL InsertInSRQ(U8 type,U8 id,void* arg);
#endif

extern void RespondSRQ(void);
extern BOOL TimeReq;
extern BOOL TimerReq;
extern BOOL IsrReq;
#endif
