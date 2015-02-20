/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2015 Frank-Michael Krause
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/
#include <wirish/wirish.h>
#include "functiontimer.h"
#include "debug.h"


//#define TMR_DEBUG

typedef void (*tmrFuncPtr)(int arg);


typedef struct timerDaten {
	struct timerDaten *pNext;
	struct timerDaten *pPrev;
	int time;
	int state;
	int arg;
	int id;
	tmrFuncPtr handler;
}TimerData_t;


TimerData_t timerArray[TIMER_ANZAHL];
TimerData_t *pNextTimer = NULL;



#define TIMEOUT_RATE 5000    // in microseconds

HardwareTimer timer(2);


static void timeoutHandler() {
	if (pNextTimer) {
       pNextTimer->time -= 5;
       if (pNextTimer->time <= 0) {
#ifdef TMR_DEBUG
    	  PrintNumber("to", pNextTimer->id);
#endif
    	  TimerData_t *p = pNextTimer;
    	  pNextTimer = pNextTimer->pNext;
    	  if (pNextTimer)
    	     pNextTimer->pPrev = NULL;
    	  p->state = 0;
    	  //p->pNext = NULL;
    	  //p->pPrev = NULL;
    	  p->handler(p->arg);
       }
	}
}

static bool timerInitialized = false;

void timerInit() {
	if (timerInitialized == false) {
		for (int i=0; i<TIMER_ANZAHL; i++)
			timerArray[i].id = i;

		timer.pause();

		// Set up period
		timer.setPeriod(TIMEOUT_RATE); // in microseconds

		// Set up an interrupt on channel 1S
		timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
		timer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
		timer.attachCompare1Interrupt(timeoutHandler);

		// Refresh the timer's count, prescale, and overflow
		timer.refresh();

		// Start the timer counting
		timer.resume();

		timerInitialized = true;
	}
}

void PrintTimerList();

void timerStart(int id, int ms, tmrFuncPtr handler, int arg) {
    if (id < 0)
    	return;
    if (id>= TIMER_ANZAHL)
    	return;
    if (timerArray[id].state) // is already running?
    	return;

	TimerData_t *pNew = timerArray + id;
	pNew->time = ms+5;
    pNew->state = 1;
    pNew->handler = handler;
    pNew->pNext = NULL;
    pNew->pPrev = NULL;
    pNew->arg = arg;
    if (pNextTimer == NULL) {
    	pNextTimer = pNew;
#ifdef TMR_DEBUG
    	PrintNumber("st", id);
#endif
    }
    else {
#ifdef TMR_DEBUG
    	Print2Number("sta", id, ms);
#endif
    	noInterrupts();
		TimerData_t *p  = pNextTimer;
		TimerData_t *last = NULL;
		while (p) {
			if (pNew->time < p->time) // vorher einhaengen
				break;
			else
				pNew->time -= p->time;
			last = p;
			p = p->pNext;
		}

		if (p==NULL) {
		   // hinter Last mit Restzeit einhängen
		   last->pNext = pNew;
		   pNew->pPrev = last;
#ifdef TMR_DEBUG
		   debugPrint("at end");
#endif
		}
		else { // vor p einhängen und p->time um pNew->time verkleinern
		   p->time -= pNew->time;
           pNew->pNext = p;

		   if (p==pNextTimer) {
			 pNextTimer = pNew;
			 p->pPrev = pNew;
#ifdef TMR_DEBUG
			 PrintNumber("at begin", p->id);
#endif
		   }
		   else {
			   pNew->pPrev = p->pPrev;
			   p->pPrev = pNew;
			   if (pNew->pPrev)
				   pNew->pPrev->pNext = pNew;
#ifdef TMR_DEBUG
			   Print2Number("between", pNew->pPrev->id, pNew->pNext->id);
#endif
		   }
		}
		interrupts();
    }
#ifdef TMR_DEBUG
    PrintTimerList();
#endif
}


void timerStop(int id) {
    if (id < 0)
    	return;
    if (id>= TIMER_ANZAHL)
    	return;
    if (timerArray[id].state==0) // is timer running?
    	return;

	TimerData_t *pTimer = timerArray + id;
	noInterrupts();
	if (pTimer->pNext == NULL) {
		// ist letzter in der queue
		if (pTimer->pPrev == NULL)  {
			// ist einziger Timer
			pTimer->state = 0;
			pNextTimer = NULL;
		}
		else {
			pTimer->pPrev->pNext = NULL;
			pTimer->pPrev = NULL;
			pTimer->state = 0;
		}
	}
	else {
		// Restzeit des zu löschenden Timers auf den nächsten in der Queue addieren
		pTimer->pNext->time += pTimer->time;
        if (pTimer == pNextTimer) {// erster Timer in der queue
        	pNextTimer = pTimer->pNext;
        	pTimer->pNext->pPrev = NULL;
        	pTimer->pNext = NULL;
        }
        else {
        	TimerData_t *pPrev = pTimer->pPrev;
        	pPrev->pNext = pTimer->pNext;
        	pTimer->pNext->pPrev = pPrev;
            pTimer->pNext = NULL;
            pTimer->pPrev = NULL;
        }
	}
	interrupts();

}


void PrintTimerList() {
   TimerData_t *p = pNextTimer;

   while (p) {
	  Print2Number("tmr",p->id, p->time);
	  if (p->pNext)
		  PrintNumber("  nxt", p->pNext->id);
	  if (p->pPrev)
		  PrintNumber("  prv", p->pPrev->id);
	   p = p->pNext;
   }
}
