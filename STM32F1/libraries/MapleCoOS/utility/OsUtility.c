/**
 *******************************************************************************
 * @file       utility.c
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      Utility management implementation code of CooCox CoOS kernel.	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 


/*---------------------------- Include ---------------------------------------*/
#include <coocox.h>

#if CFG_UTILITY_EN > 0


/**
 *******************************************************************************
 * @brief      Convert tick number to time 	  
 * @param[in]  ticks    Specifies the systerm tick numbers that will be converted.	 
 * @param[out] hour     Hours which converted. 
 * @param[out] minute   minutes which converted.
 * @param[out] sec      seconds which converted.
 * @param[out] millsec  millseconds which converted.
 * @retval     None		 
 *
 * @par Description
 * @details    This function is called to convert specify ticks to time format.	  	 	
 *******************************************************************************				
 */
#if CFG_TICK_TO_TIME_EN > 0
void CoTickToTime(U32 ticks,U8* hour,U8* minute,U8* sec,U16* millsec)
{
    U32 totalTime;
    
    /* Convert ticks to time*/
    totalTime = ticks * (1000/CFG_SYSTICK_FREQ);
    *millsec  = totalTime%1000;
    totalTime = totalTime/1000;
    *sec      = totalTime%60;
    totalTime = totalTime/60;
    *minute   = totalTime%60;
    totalTime = totalTime/60;
    *hour     = totalTime;		
}
#endif    /* CFG_TICK_TO_TIME_EN    */


/**
 *******************************************************************************
 * @brief      Convert time to tick	  
 * @param[in]  hour     Specifies the number of hours.
 * @param[in]  minute   Specifies the number of minutes.
 * @param[in]  sec      Specifies the number of seconds.
 * @param[in]  millsec  Specifies the number of millseconds.	 
 * @param[out] ticks    Tick numbers that converted.  
 * @retval     E_INVALID_PARAMETER   Invalid parameter be passed and convert fail.
 * @retval     E_OK                  Convert successful.
 *
 * @par Description
 * @details    This function is called to convert specify time to tick number. 		 
 *******************************************************************************
 */
#if CFG_TIME_TO_TICK_EN > 0
StatusType  CoTimeToTick(U8 hour,U8 minute,U8 sec,U16 millsec,U32* ticks)
{
#if CFG_PAR_CHECKOUT_EN >0
    /* Validate arguments to be within range */
    if((minute > 59)||(sec > 59)||(millsec > 999))
        return E_INVALID_PARAMETER;
#endif

    /* Convert time to ticks */
    *ticks = ((hour*3600) + (minute*60) + (sec)) * (CFG_SYSTICK_FREQ)\
              + (millsec*CFG_SYSTICK_FREQ + 500)/1000;
    return E_OK;
}
#endif    /* CFG_TIME_TO_TICK_EN  */

#endif    /* CFG_UTILITY_EN       */
