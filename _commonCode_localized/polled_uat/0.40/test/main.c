//Taken from polled_uat/test 0.10-5
//  Taken from stdinNonBlock
//  Taken from serIOtest20, and modified slightly

#include <stdio.h>
#include <errno.h>
#include <string.h> //needed for strerror()
#include <fcntl.h> //file control for open/read
#include _POLLED_UAT_HEADER_


//These need to be global because they're used by polled_uat.c
// via #defines...
int lineVal = 1;
tcnter_source_t timerCounter = 0;

//These have been moved to the makefile
//#define PU_PC_DEBUG	1
//#define getbit(a) (lineVal)
//#define TCNT0	(timerCounter)
//#define OCR0A	(9)
//#define BIT_TCNT 6
//#include "../polled_uat.c"


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

//void puat_writeOutput(uint8_t puatNum, uint8_t value)
//{
//	printf("writeOutput[%d]: %d\n", puatNum, value);
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
	printf("   Press another letter and return to start sending.\n");
	printf("   (upper-case letters sent on puat 1, lower-case on 0).\n");

	//POLLED_UAT Specific Stuff
	uint8_t puat;

	for(puat=0; puat<NUMPUATS; puat++)
		puat_init(puat);


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
				//POLLED_UAT TESTS:
				case '\n':
					break;
				//^^^ TO HERE
				default:
					//Send Upper-case chars to puat 1
					//     Lower-case to puat 0
					if((kbChar >= 'A') && (kbChar <= 'Z'))
						puat = 1;
					else
						puat = 0;

					if(puat_dataWaiting(puat))
						printf("Buffer[%d] Full; Ignoring '%c'\n", 
								(int)puat, kbChar);
					else
						puat_sendByte(puat,kbChar);
					//printf("Received: '%c'", kbChar);
			}
		}


		for(puat=0; puat<NUMPUATS; puat++)
			puat_update(puat);

//		if(puat_dataWaiting())
//		{
//			unsigned char byte = POLLED_UAT_getByte();
//			printf("POLLED_UAT Received: '%c'=0x%x\n", byte, (int)byte);
//		}

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

