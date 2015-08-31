/**
 *******************************************************************************
 * @file       OsError.h	
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      rror dispose header file	
 * @details    This file use to dispose error which from error configure for OS.
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


#ifndef _ERROR_H
#define _ERROR_H

#if (CFG_SYSTICK_FREQ > 1000) ||(CFG_SYSTICK_FREQ < 1) 
    #error " OsConfig.h System Tick time must between 1ms and 1s!"
#endif

#if CFG_MAX_USER_TASKS > 253
    #error " OsConfig.h, CFG_MAX_USER_TASKS must be <= 253! "
#endif

#if CFG_LOWEST_PRIO > 254
    #error " OsConfig.h, CFG_LOWEST_PRIO must be <= 254! "
#endif

#if CFG_IDLE_STACK_SIZE <25
	#error " OsConfig.h, CFG_IDLE_STACK_SIZE must be >= 25! "
#endif


#if CFG_ROBIN_EN > 0
    #if CFG_TIME_SLICE > 4095
    #error " OsConfig.h, CFG_TIME_SLICE must be <= 4095! "
    #endif
#endif

#if CFG_TMR_EN > 0
    #if CFG_MAX_TMR > 32
    #error " OsConfig.h, CFG_MAX_TMR must be <= 32! "
    #endif
#endif


#if CFG_MM_EN > 0
    #if CFG_MAX_MM > 32
    #error " config.h, CFG_MAX_MM must be <= 32! "
    #endif
#endif


#if CFG_KHEAP_EN > 0
    #if KHEAP_SIZE < 0x20
    #error " config.h, CFG_MAX_MM must be >= 0x20! "
    #endif
#endif

#if CFG_MUTEX_EN > 0
    #if CFG_MAX_MUTEX > 254
    #error " config.h, CFG_MAX_MUTEX must be <= 254! "
    #endif
#endif


#if CFG_EVENT_EN > 0
    #if (CFG_MAX_EVENT > 254 || CFG_MAX_EVENT <= 0)
    #error " config.h, CFG_MAX_EVENT must be <= 254 && > 0! "
    #endif


    #if CFG_QUEUE_EN > 0 
        #if CFG_MAX_QUEUE > CFG_MAX_EVENT
        #error " config.h, CFG_MAX_QUEUE must be <= CFG_MAX_EVENT! "	
        #endif
    #endif	
#endif      /* CFG_EVENT_EN  */

#endif      /* _ERROR_H      */
