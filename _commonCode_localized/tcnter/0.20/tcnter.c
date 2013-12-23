//tcnter 0.10
#ifdef _TCNTER_HEADER_
 #include _TCNTER_HEADER_
#else
 #include "tcnter.h"
#endif



#if (defined(PU_PC_DEBUG) && PU_PC_DEBUG)
	#include <stdio.h>
	#define DPRINT	printf
#else
	#define DPRINT(...) {}
#endif

#if (defined(TCNTER_SOURCE_EXTERNED) && TCNTER_SOURCE_EXTERNED)
	extern tcnter_source_t	TCNTER_SOURCE_VAR;
#endif



//DON'T CHANGE THIS WITHOUT CHANGING both!
//#define tcnter_source_t	uint8_t
//#define tcnter_compare_t	int16_t
//#define tcnter_t	uint32_t

//Running TCNTs since last init
tcnter_t tcnter_myTcnter = 0;
//Last TCNT value read in update()
// this is not used, except internally
// (global for reset on init)
tcnter_source_t tcnter_lastTcnt = 0;
// Before inlining functions:
//INLINING EXPERIMENTS: INLINE=FALSE 4098B INLINE=TRUE 4240B
//   after all inlining, it's still bigger than without inlining!
//Inlined: Saved 24B (4216)	
TCNTER_INLINEABLE
void tcnter_init(void)
{
	tcnter_lastTcnt = TCNTER_SOURCE_VAR;
	tcnter_myTcnter = 0;

	DPRINT("tcnter_init()\n");
}

//#warning "Changing TCNT for this purpose makes the timer unusable for more than one UAR/T or other device at a time!"
// What about a tcnt DIFFERENCE, instead?
// How difficult would this be with wraparound and/or resets at arbitary
// values?
// Can we use the other devices' (e.g. DMS) OCR values?
// What about an xyTracker for more precision and less cumulative error?

//Inlined: WTF?! 4240->4880B! (used twice... once on init)
//  Also seems to cause the main loop to not function
//  (how could it take *more* instructions?!)
//   trying to shuffle registers?
//TCNTER_INLINEABLE
void tcnter_update(void)
{
	tcnter_source_t thisTcnt = TCNTER_SOURCE_VAR;	//e.g. TCNT0
	
	tcnter_compare_t deltaTcnt = (tcnter_compare_t)thisTcnt 
										- (tcnter_compare_t)tcnter_lastTcnt;

	// Handle wrap-around...
	if (thisTcnt < tcnter_lastTcnt)
		deltaTcnt += (tcnter_compare_t)(TCNTER_SOURCE_OVERFLOW_VAL);

	tcnter_lastTcnt = thisTcnt;

	tcnter_myTcnter += (tcnter_t)deltaTcnt;
}

//Inlined: Saved 40B
TCNTER_INLINEABLE
tcnter_t tcnter_get(void)
{
	return tcnter_myTcnter;
}


//This is a recurring dilemma... dms6sec_t had issues as well
// Should create such a function there, as well..
// e.g. 
//thisTime Wraps at 0xff... (8bit)
// nextTime is in 16 units (timeDiff) (nextTime=0x00)
// thisTime is at 0xf0
// then nextTime is met when thisTime = 0x00
//Can't say if(thisTime > nextTime) (obviously)
// but could do (thisTime - timeDiff) > startTime) ???
// or maybe even (startTime + timeDiff > thisTime) ???
// or (thisTime - startTime > timeDiff) ???
//  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ---- GOOD
//  Wrapping Math handles this case...
//  Additionally, nice(?), no calculation necessary for nextTime... 
// (thisTime > nextTime)
// *** StartTime is updated so that no cumulative error occurs...
// THE WHOLE POINT OF THIS IS TO REDUCE INSTRUCTIONS....
//  The old method (thisTime > nextTime)
//   resulted in sometimes 12 instructions (4=readThisTime, 4=readNextTime
//    4=compare) Then 4=calcNextTime...
TCNTER_INLINEABLE
uint8_t tcnter_isItTime8(tcnter8_t *startTime, tcnter8_t deltaTime)
{
	tcnter8_t thisDelta = (uint8_t)tcnter_myTcnter - (uint8_t)(*startTime);
	
	if(thisDelta >= deltaTime)
	{
		//Say thisDelta = 5, deltaTime = 4
		// that means we're 1 after the desired time
		//  (thisDelta - deltaTime) = 1
		// So set startTime (for the next go-round) to thisTime - 1
		//  (to hopefully eliminate cumulative error)

		// Could this math be combined with >= above?
		*startTime = (uint8_t)tcnter_myTcnter - (thisDelta - deltaTime);
		return TRUE;
	}
	else
		return FALSE;
// return (((uint8_t)tcnter_myTcnter - (uint8_t)startTime) > (uint8_t)deltaTime);
}


TCNTER_INLINEABLE
uint8_t tcnter_isItTime(tcnter_t *startTime, tcnter_t deltaTime)
{
	tcnter_t thisDelta = tcnter_myTcnter - *startTime;

	if(thisDelta >= deltaTime)
	{
		*startTime = tcnter_myTcnter - (thisDelta - deltaTime);
		return TRUE;
	}
	else
		return FALSE;
}


/* poll_uar notes, could be useful here...?
	Timing Considerations:
		TCNT increments:
			near bit-rate (e.g. 1.5 TCNTS = 1 bit)
				high bit-rate
					Risky, updates might not be often enough regardless of xyt
				slow counter
					NEEDs xyt
			good (e.g. 52.9 TCNTS = 1 bit)
				since .9 is dropped from calculations we have 52 TCNTS/bit
				after 10 bits (one frame) this is only 9 TCNTS away from center
					or 9/52 = ~.2 bits away from center
			   	(and next Start-edge should realign again)
				Fast Counter
					multiple tcnts per update (possibly)
						HEREIN LIES THE PROBLEM:
							How to track sample times...?
								tcnt likely to overflow several times per byte
									running tcnt?
				
				xyt would probably be aceptable
				  though it's increasingly likely that each update will have
				  multiple tcnts (and xyt will need to update multiple times)
			far/risky (e.g. 250 TCNTS = 1 bit)
				math gets difficult due to constant wraparound
				Fast Counter
					xyt would be extremely difficult to keep-up
						then again, much less necessary; high-precision

*/

