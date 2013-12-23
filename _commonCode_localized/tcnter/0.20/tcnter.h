//tcnter 0.20-7
// Similar functionality to dmsTimer
//   Doesn't require interrupts
//   MUCH higher resolution-potential (e.g. for bit-rate generation)
//      limited only by the tcnter_update() call rate(?)
// TODO: Make some, if not all, functions inline.
//       CONSIDER: if timing is highly critical, it may make sense
//                 to call update() with every get()
//                 THOUGH: this would add a lot of calculations...
//       CONSIDER2: could myTcnter_t be smaller, and use wraparound math?
//                 e.g.  if(get() >= next) NOGO ? next=3, get=254
//                 e.g.2 if(get() - next >= 0) ... same
//                  wasn't there a way to do this?
//                       if(get() - last >= next - last) ?
//                             next=3, get=254, last=253
//                          get()-last = 254-253 = 1
//                          next-last = 3-253 = 6
//                  Promotion issues may occur, watch casts.
//                  See cTools/unsignedSubtraction.c
//                             next=5, get=7, last=6
//					Create "wrappableTimeCompare()" macro?
//             Also usable in dms, etc...
//0.20-7 adding isItTime (non 8-bit)
//0.20-6 adding to COM_HEADERS when INLINED
//0.20-5 Renaming Globals to tcnter_*... nastiness
//       see puar notes...
//0.20-4 tcnter8_t -> tcnter_isItTime8(startTime, delta)
//       HANDY: When inlined, if(tcnter_isItTime()) results in
//       an if statement from inside tcnter_isItTime, which is used
//       *in place of* the if surrounding tcnter_isItTime()
//       Inlining optimization is great!
//0.20-3 myTcnter_t is stupid -> tcnter_t
//0.20-2 adding _SPECIALHEADER_FOR_TCNTER_
//       e.g. TCNTER OVERFLOW VAL = _DMS_OCR_VAL_ requires dmsTimer.h
//       so CFLAGS+=-D'_SPECIALHEADER_FOR_TCNTER_=_DMSTIMER_HEADER_'
//0.20-1 inlining...
//				SEE NOTES in .c before using inlining
//          and experiment and stuff
//0.20 TCNTER_SOURCE_MAX renamed to TCNTER_SOURCE_OVERFLOW_VAL
//0.10-3 fixing names in test (to match 0.10-1 changes)
//       experimenting with test
//0.10-2 Need <avr/io.h> when TCNT, etc. is used...
//0.10-1 (forgot to backup first version?)
//       renaming all tcnt and whatnot to tcnter
//0.10 First version, stolen and modified from polled_uar 0.10-6
//
//---------
//polled_uar 0.10-6:
//   Doesn't use interrupts (no lag-times, etc.)
//0.10-4 
//       Looking into running-tcnt
//				myTcnter and nextTcnter now implemented
//          Fixes potential issues with multi-TCNTs per update
//            Aiding cumulative-error fixing
//            (Next time was dependent on the time of the current update)
//0.10-3 test app using makefile...
//			cleanup
//0.10-1 More mods, test app
//0.10 First Version stolen and modified heavily from usi_uart 0.22-3

#ifndef __TCNTER_H__
#define __TCNTER_H__

//This shouldn't be necessary... (or its necessity removed soon)
//#include <avr/io.h>
#include <stdint.h>

#ifdef __AVR_ARCH__
 //This is only necessary if TCNT0 (etc.) is used
 // according to the project makefile
 #include <avr/io.h>
#endif

#ifdef _SPECIALHEADER_FOR_TCNTER_
 #include _SPECIALHEADER_FOR_TCNTER_
#endif

/*
// This'd be nice to change...
#if (!defined(PU_PC_DEBUG) || !PU_PC_DEBUG)
// #include "../../bithandling/0.94/bithandling.h"
#else
 #include <stdio.h>
#endif
*/

//DON'T CHANGE THIS WITHOUT CHANGING both!
#define tcnter_source_t   uint8_t
#define tcnter_compare_t  int16_t


//This is a stupid name...
#define myTcnter_t  uint32_t
//This makes more sense...
// But I think I did it that way because I might later want to make this
// an internal structure, in case I want multiple tcnters...
// (e.g. like xyt, or hfm, etc...)
// seems less likely in this case...
#define tcnter_t uint32_t
#define tcnter8_t uint8_t


//********** TCNTER Prototypes **********//

//These are the functions that are used in main code...
// General initialization...
void tcnter_init(void);
// To be called in the main loop...
// (Handles reading the TCNT deltas and adding to myTcnter, etc)
void tcnter_update(void);
// Get the current value of myTcnter
tcnter_t tcnter_get(void);

//Of course, this is only safe if it's called often enough...
// and deltaTime is small enough...
// and tcnterUpdate is called often enough, as well (?)
uint8_t tcnter_isItTime8(tcnter8_t *startTime, tcnter8_t deltaTime);
uint8_t tcnter_isItTime(tcnter_t *startTime, tcnter_t deltaTime);

#if (defined(_TCNTER_INLINE_) && _TCNTER_INLINE_)
   #define TCNTER_INLINEABLE extern __inline__
   #include "tcnter.c"
#else
   #define TCNTER_INLINEABLE //Nothing Here
#endif


//Basic functionality:
// init();
// while(1)
// {
//		update();
//    ...
//		if(tcnter_get() >= nextTime)
//			nextTime += delay;
//			...
// }
//
//  This should probably be changed to be #defined application-specifically

#endif

