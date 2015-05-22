/**
 *******************************************************************************
 * @file       error.h	
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      rror dispose header file	
 * @details    This file use to dispose error which from error configure for OS.
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
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
