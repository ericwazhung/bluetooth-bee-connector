//polled_uar 0.30
//#ifdef _POLLED_UAR_HEADER_
#include _POLLED_UAR_HEADER_
//#include _TCNTER_HEADER_
//#else
// #include "polled_uar.h"
//#endif

//This file needs to be located in the project's include path...
// e.g. in the main folder
// It's project-specific for writeOutput()
#include "puarStuff.h"



#if (defined(PU_PC_DEBUG) && PU_PC_DEBUG)
	#define DPRINT	printf
	//This is a hack... not sure what to do about it yet
//	extern int	TCNT0;
//	extern int	getbit(a);
#else
	#define DPRINT(...) {}
#endif

//********** POLLED UART Defines **********//

#define DATA_BITS                 8
#define START_BIT                 1
#define STOP_BIT                  1

//#define TIMER0_SEED (256 - ( (SYSTEM_CLOCK / BAUDRATE) / TIMER_PRESCALER ))


//#define BIT_TCNT	 //THIS NEEDS TO BE CALCULATED!!!
//#define HALF_TCNT (BIT_TCNT/2)

uint8_t rxData[NUMPUARS];
uint8_t rxDataWaiting[NUMPUARS];//=FALSE;

//Inlining saved 12B (one call?)
PUAR_INLINEABLE
uint8_t puar_dataWaiting(uint8_t puarNum)
{
	return rxDataWaiting[puarNum];
}

//Must be called AFTER testing if data's waiting
// otherwise it will return garbage
//Inlining saved 20B (one call?)
PUAR_INLINEABLE
uint8_t puar_getByte(uint8_t puarNum)
{
	rxDataWaiting[puarNum] = FALSE;
	return rxData[puarNum];
}

uint8_t rxState[NUMPUARS];
#define RXSTATE_AWAITING_START	0	//Waiting for the edge
#define RXSTATE_EXPECTING_START  1	//Waiting to sample the start bit
#define RXSTATE_FIRST_BIT			2  //Waiting to sample the first bit
// Bits are received here
#define RXSTATE_EXPECTING_STOP	(RXSTATE_FIRST_BIT + DATA_BITS)



//#warning "Changing TCNT for this purpose makes the timer unusable for more than one UAR/T or other device at a time!"
// What about a tcnt DIFFERENCE, instead?
// How difficult would this be with wraparound and/or resets at arbitary
// values?
// Can we use the other devices' (e.g. DMS) OCR values?
// What about an xyTracker for more precision and less cumulative error?

// Wait for a 'U' to be sent, to determine BIT_TCNT
myTcnter_t puar_bitTcnt[NUMPUARS];

#ifndef BIT_TCNT


#if 0 //auto calibrating doesn't work...
//    ?G?G?V
//    vvvvvv
// ___             ___             ___
//    \/\/\___/\/\/   \/\/\___/\/\/   \/\/\___
//    ^    ^  ^
//    \____|__/<-- Bit Duration
//    .    |  .
//    .    Verified
//    .    ^  .
// ..______/\______/\______/\____
//   0.      1.      2       3		detectFilteredEdge call #
//    ^       ^
//   ST0     ST1

#define BIT_TCNT_MIN	2//5//12
//Internal for calibrateBitTcnt()
PUAR_INLINEABLE
myTcnter_t detectFilteredEdge(uint8_t puarNum)
{
	uint8_t checkingEdge = FALSE;
	uint8_t startLevel = puar_readInput(puarNum);
	myTcnter_t thisEdgeStartTime = 0;
	myTcnter_t firstEdgeStartTime = 0;

	//tcnter_update();
	//startTime = tcnter_get();


	while(1)	//continue until break
	{
		tcnter_update();

		if(puar_readInput(puarNum) != startLevel)
		{
			if(checkingEdge)
			{
				//the level has been stable for enough time...
				if(tcnter_get() - thisEdgeStartTime > BIT_TCNT_MIN)
					break;
			}
			else	//Not Yet Checking Edge, start doing-so
			{
				checkingEdge = TRUE;
				thisEdgeStartTime = tcnter_get();

				if(firstEdgeStartTime == 0)
					firstEdgeStartTime = tcnter_get();
			}

		}
		else
		{
			//[Back] at the start-level... reset the edge-timer
			checkingEdge = FALSE;
			thisEdgeStartTime = 0;
		}

	}

	return firstEdgeStartTime;

}

//On the plus side, by inlining both this and the above,
// if this isn't called, it doesn't take *any* code-space (?!)
// finally this inlining thing seems to work as expected...
PUAR_INLINEABLE
myTcnter_t puar_calibrateBitTcnt(uint8_t puarNum)
{
	myTcnter_t bitStartTcnt[9];
	uint8_t bitNum = 0;


	for(bitNum = 0; bitNum < 4; bitNum++)
	{
		bitStartTcnt[bitNum] = detectFilteredEdge(puarNum);
	}


	//myTcnter_t totalTcnt = bitStartTcnt[8] - bitStartTcnt[0];

	//puar_bitTcnt[puarNum] = totalTcnt / 9;

	puar_bitTcnt[puarNum] = bitStartTcnt[2] - bitStartTcnt[1];
	//So the same bit-rate can be used for Tx, etc.
	return puar_bitTcnt[puarNum];
}
#endif //FALSE (calibrated stuff doesn't work)

//Inlining saved 26 bytes (one call?!)
PUAR_INLINEABLE
void puar_setBitTcnt(uint8_t puarNum, myTcnter_t tcnt)
{
	puar_bitTcnt[puarNum] = tcnt;
}
#endif

//Inlining saved 2 Bytes !!!
PUAR_INLINEABLE
void puar_init(uint8_t puarNum)
{
#if(!defined(TCNTER_STUFF_IN_MAIN) || !TCNTER_STUFF_IN_MAIN)
	tcnter_init();
#endif

#ifdef BIT_TCNT
	puar_bitTcnt[puarNum] = BIT_TCNT;
#endif

	DPRINT("puar_init(%d)\n", puarNum);
}

//Inlining saved 172 Bytes !
PUAR_INLINEABLE
void puar_update(uint8_t puarNum)
{
//	static uint8_t lastTcnt = 0;		//TCNT as of the last update() call
	static uint8_t thisByte[NUMPUARS]; // = 0x00;
//	uint8_t thisTcnt = TCNT0;
	
//	static uint16_t myTcnter = 0;		//Running tcnts since last Start edge
	static myTcnter_t nextTcnter[NUMPUARS]; // = 0;	//When the next action should occur
												// wrt: myTcnter

#if(!defined(TCNTER_STUFF_IN_MAIN) || !TCNTER_STUFF_IN_MAIN)
	tcnter_update();
#endif

//	int16_t deltaTcnt = (int16_t)thisTcnt - (int16_t)lastTcnt;

	// Handle wrap-around...
//	if (thisTcnt < lastTcnt)
//		deltaTcnt += (int16_t)(OCR0A);

//	lastTcnt = thisTcnt;

//	myTcnter += deltaTcnt;

//            start                                stop
// bits:        v   0   1   2   3   4   5   6   7   v 
//    .....___     ___ ___ ___ ___ ___ ___ ___ ___ ___ .......
//            \___/___X___X___X___X___X___X___X___/
//            ^ ^   ^   ^   ^   ^   ^   ^   ^   ^   ^
//            | |   |   |   |   |   |   |   |   |   |
//            | |   \---+-Data Bits Sampled-+---/   |
//            | |                                   |
//            | \--Start Tested             Stop Bit Sampled
//            |
//            \--Start Detected
//            
//    .....___     ___ ___ ___ ___ ___ ___ ___ ___ ___ .......
//            \___/___X___X___X___X___X___X___X___/
//	           ^ ^   ^   ^   ^   ^   ^   ^   ^   ^   ^
//    ...____/ V \_/ \___ 7 States Unnamed ____/ \_/ \___AWAITING_START (2)
//      |      |  |                               |
//      |      |  \--FIRST_BIT                    \--EXPECTING_STOP
//      |      |
//      |      \--EXPECTING_START
//      |
//      \--AWAITING_START
//
// Bit order might be reversed, (looks right, pseudo-verified)
// I am, however, pretty sure that start-bits are 0 and stop-bits are 1
//    so then the data-bits are directly-read from the pin (not inverted)

	if(rxState[puarNum] == RXSTATE_AWAITING_START)
	{
	   if(!puar_readInput(puarNum)) //getbit(rxPin))
		{
			rxState[puarNum] = RXSTATE_EXPECTING_START;
			//Set it up to sample bits halfway...
			//TCNT0 = HALF_TCNT;
			//lastTcnt = thisTcnt;
			//myTcnter = 0;
			nextTcnter[puarNum] = tcnter_get() + puar_bitTcnt[puarNum]/2; //HALF_TCNT + tcnter_get();

			//Depending on the timer resolution, it might be worthwhile
			// to restart the timer...
			// hopefully the res is high enough not to have to

			thisByte[puarNum] = 0x00;
			DPRINT("Start Detected [%d]\n", (int)puarNum);
		}
		else
		{
			//Still waiting for that start indication
		}
	}
	// Assuming that this function is called often enough that
	// TCNT can never be >= 2* BIT_TCNT
	// This test should be safe, since it only gets here when 
	//   not awaiting start
//	else if(myTcnter >= nextTcnter)
	else if(tcnter_get() >= nextTcnter[puarNum])
	{
		//This should be safe here for all cases below
		//TCNT0 = thisTcnt - BIT_TCNT;
		//lastTcnt = thisTcnt;
		nextTcnter[puarNum] += puar_bitTcnt[puarNum]; //BIT_TCNT;

		if(rxState[puarNum] == RXSTATE_EXPECTING_START)
		{
   		//Make sure it wasn't just a glitch...
   		if(!puar_readInput(puarNum)) //getbit(rxPin))
   		{
         	//TCNT0 = thisTcnt - BIT_TCNT;
         	rxState[puarNum] = RXSTATE_FIRST_BIT;
         	//lastTcnt = thisTcnt;
         	//nextTcnter += BIT_TCNT;
         	DPRINT("Sampled[%d]: Start\n", (int)puarNum);
   		}
   		else  //GLITCH...
   		{
         	rxState[puarNum] = RXSTATE_AWAITING_START;
         	DPRINT("Error[%d]: Start Glitch\n", (int)puarNum);
   		}
		}
		// Receiving Data Bits...
		// We *might* be able to get away without these tests
		//  just making it an else case...
		// (Though, intuitively, it makes more visual-sense this way)
		else if( (rxState[puarNum] >= RXSTATE_FIRST_BIT) 
			 && (rxState[puarNum] < RXSTATE_EXPECTING_STOP) )
		{
			uint8_t pinVal = puar_readInput(puarNum); //getbit(rxPin);

			(thisByte[puarNum]) >>= 1;
			//This is specific to 8-bit data-streams!
			// (There may be a way to optimize this a bit)
			// (Especially since getbit uses shifts already)
			(thisByte[puarNum]) |= (pinVal<<7);
			(rxState[puarNum])++;
			DPRINT("Sampled[%d]: Bit %d = %d\n", (int)puarNum,
										(int)(rxState[puarNum]-RXSTATE_FIRST_BIT-1),
										(int)pinVal);
		}
		else if(rxState[puarNum] == RXSTATE_EXPECTING_STOP)
		{
			//Make sure we got our stop-bit
			if(puar_readInput(puarNum)) //getbit(rxPin))
			{
				//!!!LOAD DATA TO RX BUFFER
				rxData[puarNum] = thisByte[puarNum];
				rxDataWaiting[puarNum] = TRUE;
				DPRINT("Sampled[%d]: Stop. Received '%c'=0x%x\n",
									(int)puarNum, (char)(rxData[puarNum]),
								  	(int)(rxData[puarNum]));
			}
			else	//framing error...
			{
				//Not sure what to do... could have an error
				// and load data anyhow...
				// or just discard it
				DPRINT("Error[%d]: Stop Not Received\n", (int)puarNum);
			}

			rxState[puarNum] = RXSTATE_AWAITING_START;
		}
		else
		{
			//!!!We shouldn't get here!
		}
	}
	else
	{
/*
#if (defined(PU_PC_DEBUG) && PU_PC_DEBUG)
//		static uint8_t lastPrintedTcnt = 0;
		//Between bits... nothing to do
//		if(thisTcnt != lastPrintedTcnt)
//		{
//			lastPrintedTcnt = thisTcnt;
		if(deltaTcnt)
		{
			DPRINT(" myTcnter: %d ", myTcnter);
		}	
#endif
*/
	}
}


/* Timing Considerations:
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



//For compatibility with Tiny24?'s code on 861...
//#define TCNT0 TCNT0L
//#define TCCR0 TCCR0B


//1MHz/9600 = 104.1667

#if (!defined(PU_PC_DEBUG) || (PU_PC_DEBUG == FALSE))
#if (defined(_OSCCAL_SET_))
 #if (!_OSCCAL_SET_)
   #warning "You should calibrate OSCCAL. (OSCCAL_SET is not TRUE in your makefile)"
 #endif
#else
 #error "Define OSCCAL_SET either TRUE or FALSE in your makefile"
 #error "I'm not going to let you get away without it"
 #error "and FALSE is USE-AT-YOUR-OWN-RISK"
 #error "You SHOULD calibrate your oscillator! Otherwise, at the very least,"
 #error "UART reception is gonna be shitty"
#endif
#endif

//THIS IS HIGHLY dependent on the processor frequency...
// which can be calibrated via OSCCAL
// 108 was used BEFORE setting OSCCAL
// 104 seems to work perfectly after
// As I see it (experimentally)
// Leaving this value at 103 (0->OCR0A->0...)
// Causes reception of 0xff after every byte received with 0 in its MSB
// (since the MSB is transmitted last, before the stop-bit)
// Which is basically every ASCII character...
// (somehow this zero is both shifted-in AND picked up as a new start-bit)
// INCREASING THIS VALUE decreases the likelyhood by sampling later in each
//  bit
// BUT it also makes transmission (and reception?) more likely to fail
// as the data rate increases


