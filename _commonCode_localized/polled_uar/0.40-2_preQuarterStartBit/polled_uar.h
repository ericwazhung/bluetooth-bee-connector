//polled_uar 0.40-2_pre
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

//0.40-2_pre...
//     Discovered what looks to be a pretty big bug in the latest version
//     (0.40-14)
//     a/o tabletBluetoother-8, which has been revisited *long* after last
//     used. This version, 0.40-2, appears to have been developed alongside
//     tabletBluetoother-7 (judging by the date/time)
//     The latest version has been disabled until I have a chance to figure
//     out what's wrong (and right).
//     ADDING (from a later version):
//       # Extract the directory we're in and add this to COM_HEADERS...
//       COM_HEADERS += $(dir $(POLLED_UAR_LIB))
//     So "make localize" will work...
//0.40-2 inlining...
//		 bitTcnt -> puar_bitTcnt (so it won't conflict with puat's bitTcnt
//      when both are inlined)
//     forgot to mention in a prior version BIT_TCNT now puar_bitTcnt[]
//0.40-1	puar_readInput() in puarStuff.h
//0.40 
//			TODO: NOGO: Trying to figure out how to auto-calibrate...
//     Ideas auto-calibrate either:
//        OSSCAL
//				Doesn't affect code, BIT_TCNT remains constant 
//					(rather than a variable) so slightly faster updates
//        BIT_TCNT
//				Small BIT_TCNT values could lead to tremendous error...
//          Easier to implement... 
//					send a character (so bit7 = 0, then there's guaranteed
//						0->1 transition between bit7 and stop
//					check TCNT at start edge
//					check TCNT at bit7->stop edge
//             divide by... appropriate time (9 bits)
//              ___     ___ ___ ___ ___ ___ ___ ___ ___ ___
//                 \___/___X___X___X___X___X___X___X___/
//             TODO: Higher precision if this isn't in the main loop...
//               Ideally, use timer with interrupts...
//                  Difficult without knowing exactly what byte is sent
//               SAME PROBLEM with any...
//          BAUD-Rate independent... (auto-baud rather than auto-OSCCAL)
//             U = 0x55 =
//              ___     ___     ___     ___     ___     ___ ___
//                 \___/   \___/   \___/   \___/   \___/
//            Start--^   0   1   2   3   4   5   6   7   ^--Stop
//
//0.35 TONS of changes, catching up with polled_uat:
//				Multiple Receivers
//				renaming POLLED_UAR_ functions to puar_
//       	add puar_readInput() to main instead of makefile CFLAG
//0.10-8 renaming tcnt_ and TCNT_ stuff to tcnter_ and TCNTER_
//0.10-7 removing manual tcnt stuff from here, and using tcnter instead
//       thus it won't run redundantly when running both uar and uat
//0.10-6 slight modification for better debugging
//       polled_uat now has a combined test of uar and uat
//0.10-5 addressing 0.10-4's todone
//        Minor fix in .mk re: UART -> UAR
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

#ifndef __POLLED_UAR_H__
#define __POLLED_UAR_H__

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


#if(!defined(NUMPUARS))
	#define NUMPUARS	1
#endif

//********** POLLED_UAR Prototypes **********//

//When called, it waits in its own loop to receive 'U' to calibrate
// the BIT_TCNT
#ifndef BIT_TCNT
myTcnter_t puar_calibrateBitTcnt(uint8_t puarNum);

//This only needs to be called when setting a specific bitTcnt
// (e.g. calibrated on puar0, and mimicking the rate on puar1)
void puar_setBitTcnt(uint8_t puarNum, myTcnter_t tcnt);
#endif


//These are the functions that are used in main code...
// General initialization...
void puar_init(uint8_t puarNum);

// To be called in the main loop...
// (Handles reading the pins and shifting bits, etc)
void puar_update(uint8_t puarNum);

// Check if there's data waiting
uint8_t puar_dataWaiting(uint8_t puarNum);

// Read the data that's waiting
//Must be called AFTER testing if data's waiting
// otherwise it will return garbage
uint8_t puar_getByte(uint8_t puarNum);

#if (defined(_PUAR_INLINE_) && _PUAR_INLINE_)
   #define PUAR_INLINEABLE extern __inline__
   #include "polled_uar.c"
#else
   #define PUAR_INLINEABLE //Nothing Here
#endif




// This must be defined in main.c or somewhere...
// See example below.
// how can we inline this? Is it possible?
//extern uint8_t puar_readInput(uint8_t puarNum);

//Basic functionality:
// int main(void)
// {
//		init();
// 	while(1)
// 	{
//			update();
//			if(dataWaiting())
//				data = getByte();
// 	}
// }
//
// This must be defined in puarStuff.h somewhere in the include paths...
// See example below.
//extern __inline__ uint8_t puar_readInput(uint8_t puarNum)
// __attribute__((__always_inline__));
//
// uint8_t puar_readInput(uint8_t puarNum)
//	{
//		if(puarNum==0)
//			return getpinPORT(Rx0Pin, PORTB);
//		else
//			return getpinPORT(Rx1Pin, PORTA);
//	}
//
//  This should probably be changed to be #defined application-specifically
//  Peripherals Used:
//     Timer0
//        --Required for timing of the serial bit-clock

#endif

