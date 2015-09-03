/**
 *******************************************************************************
 * @file       utility.c
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Utility management implementation code of CooCox CoOS kernel.	
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
