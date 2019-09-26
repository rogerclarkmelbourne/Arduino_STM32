#include <wiring_pulse.h>
#include "boards.h"
#include "variant.h"
/* Measures the length (in microseconds) of a pulse on the pin; state is HIGH
 * or LOW, the type of pulse to measure.  Works on pulses from 2-3 microseconds
 * to 3 minutes in length, but must be called at least a few dozen microseconds
 * before the start of the pulse. */
 
 
 /*
  * Roger Clark
  *
  * Note. The API spec for this function published on http://www.arduino.cc/en/Reference/PulseIn 
  * doesn't reflect what either the AVR or SAM version of this function actualy do with regard to the timeout value
  *
  * "timeout (optional): the number of microseconds to wait for the pulse to start; default is one second (unsigned long) "
  *
  * Because the timeout, is actually coded as the total time to both wait while the input is in the state requested
  * then wait for the opposite state duration
  * then count the length of the pulse when it has the value of state (HIGH or LOW)
  * 
  * So I think the code for both the AVR and the Due is wrong in that it doesnt match the spec
  * 
  * I have done basically the same as the AVR and Due code, except to make the timeout a bit more accurate I have put in a dummy volatile varable 
  * dummyWidth so that both the waiting while loops take the same number of clock cycles to execute as the acount width counting loop
  *
  * to be slighly more accurate the maxLoops variable really needs to take into account the loop setup code, but its probably as good as necessary
  *
  */
uint32_t pulseIn( uint32_t pin, uint32_t state, uint32_t timeout )
{
   // cache the IDR address and bit of the pin in order to speed up the
   // pulse width measuring loop and achieve finer resolution.  calling
   // digitalRead() instead yields much coarser resolution.
 
   __IO uint32_t * const idr = portInputRegister(digitalPinToPort(pin));
   const uint32_t bit = digitalPinToBitMask(pin);
   const uint32_t stateMask = (state ? bit:0);

   uint32_t width = 0; // keep initialization out of time critical area
   
   // convert the timeout from microseconds to a number of times through
   // the initial loop; it takes 16 clock cycles per iteration.
   uint32_t numloops = 0;
   uint32_t maxloops =  timeout * ( F_CPU / 16000000);
   volatile uint32_t dummyWidth=0;
   
   // wait for any previous pulse to end
   while ((*idr & bit) == stateMask)   {
      if (numloops++ == maxloops)  {
         return 0;
      }
      dummyWidth++;
   }
   
   // wait for the pulse to start
   while ((*idr & bit) != stateMask)   {
      if (numloops++ == maxloops) {
         return 0;
      }
      dummyWidth++;
   }
   
   // wait for the pulse to stop
   while ((*idr & bit) == stateMask) {
      if (numloops++ == maxloops)  {
         return 0;
      }
      width++;
   }

   // Excluding time taking up by the interrupts, it needs 16 clock cycles to look through the last while loop  
   // 5 is added as a fiddle factor to correct for interrupts etc. But ultimately this would only be accurate if it was done ona hardware timer
   
   return (uint32_t)( ( (unsigned long long)(width+5) *  (unsigned long long) 16000000.0) /(unsigned long long)F_CPU ) ;
}
