/**
 *******************************************************************************
 * @file       coocox.h
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Gather for all header file of CooCox CoOS.	
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


#ifndef _COOCOX_H
#define _COOCOX_H

#define  OS_VERSION       (U16)0x0114   /*!< OS version.(format: Vx.xx), 	
                                            e.g. value 0x0114 is version V1.14*/
/*---------------------------- Include ---------------------------------------*/
#include "CoOS.h"
#include "OsArch.h"
#include "OsCore.h"
#include "OsTask.h"
#include "OsServiceReq.h"
#include "OsError.h"
#include "OsTime.h"


#if CFG_TMR_EN > 0
	#include "OsTimer.h"
#endif

#if CFG_KHEAP_EN > 0
	#include "OsKernelHeap.h"
#endif

#if CFG_MM_EN >0
	#include "OsMM.h"
#endif

#if CFG_EVENT_EN > 0
	#include "OsEvent.h"
#endif

#if CFG_MUTEX_EN > 0
	#include "OsMutex.h"
#endif

#if CFG_QUEUE_EN > 0
	#include "OsQueue.h"
#endif

#if CFG_FLAG_EN	 > 0
	#include "OsFlag.h"
#endif

#endif    /* _COOCOX_H    */  
