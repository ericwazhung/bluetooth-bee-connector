//cirBuff 0.99-1

//0.99-1 replacing __CIRBUF_H__ with __CIRBUFF_H__
//       adding cirBuff_empty()
//0.99 attempting to remove the necessity for calloc...
//     (threePinIDer35i, uart_in1.10)
//     when CIRBUFF_NO_CALLOC = TRUE
//0.97-1 added to uart_in... some warnings were converted to errors
//       control reaches end of non-void function
//0.97 First version, stolen from midi_out 2.00-7
//      NOT used as a "common library" yet:
//      Developed for T200CS TabletRepeater (currently the only implemetation of cirBuf "libraries")
//      midi_out 2.00-7:
//         2.00-3ish adding buffer-test for sending only if it won't halt
//         1.00-1 buffer length experiments, also maybe buffer-full sei...
//          .94   midi_out Buffered (allows background sending)

#ifndef __CIRBUFF_H__
#define __CIRBUFF_H__

#if (!defined(CIRBUFF_NO_CALLOC) || !CIRBUFF_NO_CALLOC)
 #include <stdlib.h>
#endif
#include <inttypes.h>


typedef struct {
	volatile uint8_t writePosition;  //Where to write, when adding a byte
	volatile uint8_t readPosition;   //Where to read, when getting a byte
	uint8_t length;
	uint8_t *buffer;  //[];
} cirBuff_t;

#if (!defined(CIRBUFF_NO_CALLOC) || !CIRBUFF_NO_CALLOC)
 uint8_t cirBuff_init(cirBuff_t *cirBuff, uint8_t length);
 uint8_t cirBuff_destroy(cirBuff_t *cirBuff);
#else
 uint8_t cirBuff_init(cirBuff_t *cirBuff, uint8_t length, uint8_t *array);
#endif

#define DONTBLOCK	1
#define DOBLOCK		0

//Add a byte to the end of the list
// corresponds to writePosition
// NOTE THAT THIS WILL BLOCK IF THE BUFFER IS FULL AND dontBlock==FALSE
//   If there's not an interrupt (or another thread?) doing reads, it will never exit!
// Returns 1 if the buffer was full and dontBlock == TRUE (the byte was lost. BE CAREFUL!)
uint8_t cirBuff_addByte(cirBuff_t *cirBuff, uint8_t byte, uint8_t dontBlock);

//Get the first byte in the buffer
// corresponds to readPosition
// Returns -1 if no bytes were in the buffer
int16_t cirBuff_getByte(cirBuff_t *cirBuff);

void cirBuff_empty(cirBuff_t *cirBuff);

//Future:
// It might be handy to have the following:
//   isBufferFull()
//   isDataWaiting()
//  (waitForData()?)


/* Not Implemented
 //Return the amount of free space in the buffer... (not tested nor used, IIRC, A/O midi_out 2.00-7)
 uint8_t cirBuff_freeSpace();
 */

#endif
