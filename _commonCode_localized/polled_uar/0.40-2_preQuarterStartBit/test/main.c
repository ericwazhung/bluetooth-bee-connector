//Taken from stdinNonBlock
//  Taken from serIOtest20, and modified slightly

#include <stdio.h>
#include <errno.h>
#include <string.h> //needed for strerror()
#include <fcntl.h> //file control for open/read
#include _POLLED_UAR_HEADER_



//These need to be global because they're used by polled_uar.c
// via #defines...
int lineVal[NUMPUARS]; // = 1;
tcnter_source_t timerCounter = 0;

//These have been moved to the makefile
//#define PU_PC_DEBUG	1
//#define getbit(a) (lineVal)
//#define TCNT0	(timerCounter)
//#define OCR0A	(9)
//#define BIT_TCNT 6
//#include "../polled_uar.c"


#define handleError(string, exit) \
	if(errno)   \
{\
	   printf( string " Error %d: '%s'\n", errno, strerror(errno)); \
	   if(exit) \
	      return 1; \
	   else \
			errno = 0; \
}

//errno = 0 seems to work, whereas clearerr(stdin) doesn't


//uint8_t puar_readInput(uint8_t puarNum)
//{
//	CFLAGS += -D'getbit(...)=(lineVal)'
//	return lineVal[puarNum];
//}

int main(void)
{

	int quit = 0;
	int timer = 0;

	// Set STDIN non-blocking (Still requires Return)
	int flags = fcntl(0, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(0, F_SETFL, flags);

	handleError("Couldn't open stdin.", 1);

	printf("---Press q and press return to quit---\n");
	printf("   Press '0' and return to start sampling puar 0\n");
	printf("    Toggle between 0 and 1 as appropriate\n");
	printf("   Press 'L' and return to start sampling puar 1\n");
	printf("    Toggle between L and H as appropriate\n");


	//POLLED_UAR Specific Stuff
	//puar_init();

	uint8_t puar;
	for(puar=0; puar<NUMPUARS; puar++)
	{
		puar_init(puar);
		lineVal[puar] = 1;
	}
	tcnter_init();

	while(!quit)
	{
		//This worked in 0.10-3! But now 0.10-4?!
//		handleError(" Unhandled Error.", 0);

		char kbChar;
		kbChar = getchar();

		//"Resource Temporarily Unavailable" isn't an error in this case
		if(errno == 35)
			errno = 0;
		else
		{
			switch(kbChar)
			{
				case 'q':
					quit = 1;
					break;
				//POLLED_UAR TESTS:
				case '1':
					lineVal[0] = 1;
					printf("lineVal[0]: 1\n");
					break;
				case '0':
					lineVal[0] = 0;
					printf("lineVal[0]: 0\n");
					break;
				case 'H':
				case 'h':
					lineVal[1] = 1;
					printf("lineVal[1]: 1\n");
					break;
				case 'L':
				case 'l':
					lineVal[1] = 0;
					printf("lineVal[1]: 0\n");
					break;
				case '\n':
					break;
				//^^^ TO HERE
				default:
					printf("Received: '%c'", kbChar);
			}
		}


		tcnter_update();
		for(puar=0; puar<NUMPUARS; puar++)
		{
			puar_update(puar);

			if(puar_dataWaiting(puar))
			{
				unsigned char byte = puar_getByte(puar);
				printf("POLLED_UAR[%d] Received: '%c'=0x%x\n", 
						(int)puar, byte, (int)byte);
			}
		}

		timer++;

		if(timer == 100000)
		{
			timerCounter++;
			if(timerCounter == TCNTER_SOURCE_MAX) //OCR0A)
				timerCounter = 0;
			timer=0;
			printf("TCNT: %d\n",timerCounter);
		}

	}


	return 0;
}

