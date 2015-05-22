
/**
 *******************************************************************************
 * @file       OsServiceReq.h
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Header file	related to service request	
 * @details    This file including some defines and function declare related to
 *             service request.
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
    U8    head;    
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
