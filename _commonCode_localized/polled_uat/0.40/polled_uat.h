//polled_uat 0.40-13
//
// NOTE: BIT_TCNT <= 255 (not sure how it works near 255...)
//
// Stolen from polled_uar 0.10-5:
//   Screw the USI interface, let's bit-bang it dangit!
//   Benefits include: Should be usable on multiple pins (eventually)
//   Doesn't use interrupts (no lag-times, etc.)
//   Needn't be half-duplex, in fact could be quadraplex+ (intended)
// TODO:
//   Use structs to define "devices"? Similar to xytracker?
//   Use overall-distance rather than inter-bit distances for timing
//    (fractional bit-distances aren't feasible without something like xyt)
//    XYT only makes sense if the error is large...
//     e.g. if TCNT0 increments at 1MHz and the BAUD is 19200
//          that's 52.08 TCNTs per bit
//          the error is *tiny* (using 52)
//     but, if TCNT0 incremented slower... it could be handy
//          however, using xyt for fast TCNTs would a bit of a slow-down
//          (as it would have to do multiple increments on each update
//           to keep up with TCNT, likely)
//     (More notes on this in .c)

//0.40-13 Adding multiple stop-bits (especially for sdramThing at 10MHz)
//        Most data seems OK, but some bytes are mangled
//        Adding stop-bits might allow for resyncing...
//        8MHz -> 13.02 TCNTs, but 10MHz is 16.4 or something.
//        Doesn't seem to help much, and in some cases makes it worse 
//        (different stop-bit values)
//0.40-12 trying out tcnterIsItTime...
//        whoa, 8190 -> 8026 bytes! Nice.
//        Also updating this file to indicate the purpose of 0.40r
//       
//0.40r (was 0.40-11) FOR TESTING ONLY
//
//0.40-11 seeing the effect of using "register" on globalized variables
//0.40-10 adding notes re: codesize increase since globalling...
//0.40-9 adding to COM_HEADERS when INLINED
//0.40-8 Changing Globals' names to puat_*
//       See Notes In puar... Nasty.
//0.40-7 Fixing These (per puar)
// polled_uat.c:111: warning: ‘shiftingByte’ is static but declared in 
//                      inline function ‘puat_update’ which is not static
// polled_uat.c:115: warning: ‘nextTcnter’ is static but declared in 
//                      inline function ‘puat_update’ which is not static
//0.40-6	NYI: TODO: using tcnter_isItTime8() if BIT_TCNT is defined...
//0.40-5 If BIT_TCNT is defined, use it instead of bitTcnt
//       Should optimize a bit... (see similar notes in puar_0.40-4
//       TODO: bitTcnt could be smaller than myTcnter_t (uint32_t)???
//0.40-4 inlining again...
//0.40-3 numbering's off... test apps fixed, puartStuffs fixed
//0.40-2 moved puatStuff.c -> puatStuff.h
//			todo: revisit test application
//             See 0.40b for inlining stuff...
//0.40-1 puat_writeOutput inlining?
//0.40   changing BIT_TCNT -> bitTcnt[puatNum]
//0.35-2 modified note about init in test code
//0.35-1 testTxRx on two puar's
//0.35 add puat_writeOutput() to main... instead of makefile CFLAG
//0.30 Don't like these function-names... using "puat_" for now on.
//0.20-2(?) revised testuar as well
//0.20-1 revising test for 2 transmitters
//0.20 Adding multiple transmitter ability
//0.10-2 removing manual tcnt stuff from here, and using tcnter instead
//       thus it won't run redundantly when running both uar and uat
//0.10-1 adding uat->uar test
//0.10 First version, stolen and modified from polled_uar 0.10-5
//		TODO: MOST notes here are old, from uar
//-----------
//polled_uar:
//0.10-5 addressing 0.10-4's todone
//0.10-4 Buncha Notes regarding xyt at bottom of .c file
//       Looking into running-tcnt
//				myTcnter and nextTcnter now implemented
//          Fixes potential issues with multi-TCNTs per update
//            Aiding cumulative-error fixing
//            (Next time was dependent on the time of the current update)
//       See todo: in .c
//0.10-3 test app using makefile...
//			cleanup
//0.10-2 Notes re: inter-bit distances...
//0.10-1 More mods, test app
//0.10 First Version stolen and modified heavily from usi_uart 0.22-3
//----------
//usi_uart 0.22-3:
//   Using the TinyAVRs' Universal Serial Interface as a UART
//   It's hokey, and seems way more complex than bit-banging
//   using lots of interrupts and various other things
//   CRAZY, but it works. Originally from Atmel... see 0.10's notes.
//0.22-3 TIMER_COMPARE can be overridden by USI_TIMER_COMPARE in makefile
//0.22-2 inlining everywhere!
//0.22-1 (22 not backed up)
//       Looking into speedup with USI_UART_Data_In_Receive_Buffer()
//       as a macro...
//0.22 Looking into using uart_in(1.10) for its indicator/buffer...
//     (Apparently no changes were necessary... forgot to update makefile
//      to use 0.22 anyhow)
//0.20-1 Looking into reception errors... seems OK-ish now.
//       (a/o threePinIDer9i3)
//0.20 Looking into Compare-Match (as opposed to Overflow) on Timer0
//     as the baud-rate clock source...
//     (odd that it worked as-is... maybe I don't understand it yet)
//     or it was that OCR0A was 0x00, so compare-match occurred immediately
//     after an overflow.
//0.10-1 
//0.10 - First common-filed version... 
//       from USI_UART_Proj (my converting the original Atmel AppNote AVR307
//         code to avr-gcc on the ATTiny861)
// AppNote       : AVR307 - Half duplex UART using the USI Interface
// Description   : Header file for USI_UART driver

#ifndef __POLLED_UAT_H__
#define __POLLED_UAT_H__

//This shouldn't be necessary... (or its necessity removed soon)
//#include <avr/io.h>
#include <stdint.h>
#include _TCNTER_HEADER_

// This'd be nice to change...
#if (!defined(PU_PC_DEBUG) || !PU_PC_DEBUG)
 #include "../../bithandling/0.94/bithandling.h"
#else
 #include <stdio.h>
#endif


#if (!defined(NUMPUATS))
	#define NUMPUATS	1
#endif



//********** POLLED_UAT Prototypes **********//

#ifndef BIT_TCNT
void puat_setBitTcnt(uint8_t puatNum, myTcnter_t bitTcnt);
#endif

//These are the functions that are used in main code...
// General initialization...
// This doesn't really do anything
// if update() is called, it will be updated, regardless of init...
void puat_init(uint8_t puatNum);

// To be called in the main loop...
// (Handles writing the pins and shifting bits, etc)
void puat_update(uint8_t puatNum);

// Check if there's data waiting
uint8_t puat_dataWaiting(uint8_t puatNum);

// Load a byte to be sent
//  if data's waiting, this will overwrite it!
//  so best to test dataWaiting first...
void puat_sendByte(uint8_t puatNum, uint8_t byte);

#if (defined(_PUAT_INLINE_) && _PUAT_INLINE_)
	#define PUAT_INLINEABLE extern __inline__
	#include "polled_uat.c"
#else
	#define PUAT_INLINEABLE	//Nothing Here
#endif
// This must be defined in puatStuff.h somewhere in the include paths...
// See example below.
//extern __inline__ void puat_writeOutput(uint8_t puatNum, uint8_t value)
//	__attribute__((__always_inline__));

//Basic functionality:
// int main(void)
// {
//		init();
// 	while(1)
// 	{
//			update();
//			if(!dataWaiting())
//				sendByte(data);
// 	}
//	}
//
// //MUST BE DEFINED SOMEWHERE (e.g. main.c):
//	void puat_writeOutput(uint8_t puatNum, uint8_t value)
// {
//		if(puatNum==0)
//			writepinPORT(Tx0Pin, PORTB, value)
//		else
//			writepinPORT(Tx1Pin, PORTA, value)
//	}
//
//  This should probably be changed to be #defined application-specifically
//  Peripherals Used:
//     Timer0
//        --Required for timing of the serial bit-clock

#endif

