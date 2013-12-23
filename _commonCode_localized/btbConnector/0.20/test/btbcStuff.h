// This needn't (shouldn't?) be included anywhere in the project code...
//  It's included in btbConnector.c
// Not sure how to tie it all together with main.h or whatever, for e.g.
//   the #defines for Tx0pin, or if global variables were used, etc.

//void puat_writeOutput(uint8_t puatNum, uint8_t value)
//    __attribute__((__always_inline__));

#ifndef __BTBCSTUFF_H__
#define __BTBCSTUFF_H__

//#include "../../../../_commonCode/__std_wrappers/serOut_nonBlock/0.10/serOut_nonBlock.h"

#include <stdio.h>

extern void serOutNB_sendByte(char txChar);

extern __inline__ void btbc_sendByteToBTB(uint8_t byte)
{
	serOutNB_sendByte(byte);
}

extern __inline__ void btbc_sendByteToDevice(uint8_t byte)
{
	printf("Retransmitting byte to device:  0x%x='%c'\n", 
			(int)(byte), (char)(byte));

}

extern __inline__ uint8_t btbc_deviceReadyForData(void)
{
	return TRUE;
}

extern __inline__ uint8_t btbc_btbReadyForData(void)
{
	return TRUE;
}

#endif

