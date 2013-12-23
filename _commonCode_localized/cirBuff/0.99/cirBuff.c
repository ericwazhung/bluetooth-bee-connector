//cirBuff 0.97

#include "cirBuff.h"

//Give a read in the middle of a write-block... to test.
//#define __TESTING__


//This should probably be renamed... som'n like bufferEmpty...
//volatile uint8_t UART_Ready[NUMUARTS] = UARTARRAYINIT;

//Returns non-zero if there's an error
#if (!defined(CIRBUFF_NO_CALLOC) || !CIRBUFF_NO_CALLOC)
 uint8_t cirBuff_init(cirBuff_t *cirBuff, uint8_t length)
#else
 uint8_t cirBuff_init(cirBuff_t *cirBuff, uint8_t length, uint8_t *array)
#endif
{
	//length+1 because one byte is unused to indicate full/empty (see below)
#if (!defined(CIRBUFF_NO_CALLOC) || !CIRBUFF_NO_CALLOC)
	cirBuff->buffer = (uint8_t *)calloc(length+1, sizeof(uint8_t));

	//Check if malloc failed
	if(cirBuff->buffer == NULL)
		return 1;

	cirBuff->length = length + 1;
#else
	cirBuff->buffer = array;
	cirBuff->length = length;
#endif
	
	
	cirBuff->writePosition = 0;
	cirBuff->readPosition = 0;
	
	return 0;	
}

#if (!defined(CIRBUFF_NO_CALLOC) || !CIRBUFF_NO_CALLOC)
//In most cases I'm using this for there's no reason to destroy it...
// But one should never alloc without free...
// Returns 1 if the buffer was already NULL...
uint8_t cirBuff_destroy(cirBuff_t *cirBuff)
{
	if(cirBuff->buffer != NULL)
	{
		free(cirBuff->buffer);
		cirBuff->buffer = NULL;
		return 0;
	}
	else
		return 1;
}
#endif

//This loads the byte into the send buffer 
// (in midi_out the buffer is flushed one-by-one whenever the TX-complete interrupt is called)
// this is the old midiOut_sendByte:
// returns 1 if the buffer was full AND dontBlock was true (i.e. the byte was lost!), 0 otherwise
uint8_t cirBuff_addByte(cirBuff_t *cirBuff, uint8_t Data, uint8_t dontBlock)
{   
	uint8_t nextWritePosition = (cirBuff->writePosition + 1)%(cirBuff->length);
#ifdef __TESTING__
	int i = 0;
#endif
	
	//Wait if the buffer's full
	//   can check this by determining if the writePosition is one less than the sendPosition
	//   (or if the nextWritePosition is the same as the readPosition, makes more sense...)
	while(!dontBlock && (nextWritePosition == cirBuff->readPosition))
	{		
#ifdef __TESTING__
		if(i == 60000000)
			cirBuff_getByte(cirBuff);
		i++;
#endif
		//This probably isn't necessary... it wasn't in midiOut, but I don't know for certain.
		// I've heard of optimizers optimizing out empty loops (even if a test is volatile?!)
		asm("nop;");
	}
	
	//***This is where the midiOut code checked if the UART was ready to receive another byte to transmit***
	//    It would then load the byte directly to the UART instead of the buffer
	//    This doesn't make a whole lot of sense... why not test this outside and call addByte if it's busy?
	//     I think it was more efficient to have a single function, but this test could occur anywhere in this function
	//     (no sense in all that math above... right? Briefly:
	//if(UART_Ready[uartNum])
	//{
	//	UDR = Data;
	//	UART_Ready[uartNum] = FALSE;
	//}
	//else //Otherwise load to the buffer
	
	//Note, these values may differ from the ones in the while loop...
	// This *should* be OK, and maybe even handy
	// Note, also, that there is always one byte that's unused... 
	//  so writePosition == readPosition => empty
	if(nextWritePosition != cirBuff->readPosition)
	{
		//Load to the buffer
		cirBuff->buffer[ cirBuff->writePosition ] = Data;
		//Set up the writePosition for the next call
		cirBuff->writePosition = nextWritePosition;
		return 0;
	}
	else	//Indicate that the buffer was full
		return 1;
}

//If there's data in the buffer, return it (it's a single unsigned byte, 0-255)
// If there was no data in the buffer, return -1
int16_t cirBuff_getByte(cirBuff_t *cirBuff)
{
	uint8_t byte;
	
	//There's no data in the buffer
	if(cirBuff->writePosition == cirBuff->readPosition)
		return -1;
	else
	{
		byte = cirBuff->buffer[ cirBuff->readPosition ];
		
		(cirBuff->readPosition)++;
		(cirBuff->readPosition) %= (cirBuff->length);
		return byte;
	}
}

void cirBuff_empty(cirBuff_t *cirBuff)
{
	cirBuff->writePosition = cirBuff->readPosition;
}

/* More UART examples simplified here originally from midiOut... 
SIGNAL(SIG_USART_TRANS)     // UART0 Transmit Complete Interrupt Function
{
	//Check if we're done sending everything in the buffer...
	if(buffer_writePosition[uartNum] == buffer_readPosition[uartNum])
		UART_Ready[uartNum] = TRUE;
	else
	{
		//Send the next character
		UDR = cirBuff_getByte();

		//Already is... but ahwell (is this even true?)
		UART_Ready[uartNum] = FALSE;
	}
}
*/

/*  I dunno if this ever worked...
//Return an integer indicating the amount of free space in the buffer
// so we can prevent sending data if it's unimportant, so it won't halt.
uint8_t midiOut_bufferFreeSpace(uint8_t uartNum)
{
	//	uint8_t nextLoadPosition = (buffer_writePosition[uartNum] + 1)%MIDIOUT_BUFFERLENGTH;
	//buffer_writePosition is the actual position in the array 
	//	where the next character will be placed 
	//  (it is currently empty unless the buffer is full)
	//buffer_readPosition is the actual position in the array 
	//  where the /next/ character will be sent from
	//i.e. if buffer_readPosition == bufferLoadPosition, 
	//  the array is empty (?) (according to other code)
	// or full?!
	// I think the intent was to leave one byte free, always...
	//   so if lp is one less than sp it's "full"
	//   and if lp == sp, then it's empty
	
	uint8_t freeSpace;
	int16_t lp = buffer_writePosition[uartNum];
	int16_t sp = buffer_readPosition[uartNum];
	
	if(lp < sp)		//the data to be sent is wrapped-around...
		freeSpace = sp - lp - 1;
	else if (lp == sp)	//there is no data to be sent... the whole array is empty
		freeSpace = MIDIOUT_BUFFERLENGTH - 1;
	else // if (lp > sp)	//the empty space is wrapped-around
		freeSpace = MIDIOUT_BUFFERLENGTH - lp + sp - 1;
	
	return freeSpace;
	
}
*/

