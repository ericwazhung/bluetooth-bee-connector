//Taken from stdinNonBlock
//  Taken from serIOtest20, and modified slightly

#include <stdio.h>
#include <errno.h>
#include <string.h> //needed for strerror()
#include <fcntl.h> //file control for open/read
#include _TCNTER_HEADER_


//These need to be global because they're used by polled_uar.c
// via #defines...
//uint8_t 
tcnter_source_t timerCounter = 0;


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

int main(void)
{

	int stepSize = 1;
	int quit = 0;
	int timer = 0;

	// Set STDIN non-blocking (Still requires Return)
	int flags = fcntl(0, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(0, F_SETFL, flags);

	handleError("Couldn't open stdin.", 1);

	printf("---Press q and press return to quit---\n");
	printf("   Press 1-8 and press return to change the step-size\n");

	//POLLED_UAR Specific Stuff
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
				case '1':
					stepSize = 1;
					break;
				case '2':
					stepSize = 2;
					break;
				case '3':
					stepSize = 3;
					break;
				case '4':
					stepSize = 4;
					break;
				case '5':
					stepSize = 5;
					break;
				case '6':
				   stepSize = 6;
				   break;
				case '7':
				   stepSize = 7;
					break;
				case '8':
					stepSize = 8;
					break;

				case '\n':
					break;
				//^^^ TO HERE
				default:
					printf("Received: '%c'", kbChar);
			}
		}


	//	tcnter_update();

		timer++;

		if(timer == 100000)
		{
			timerCounter+=stepSize;
			if(timerCounter >= TCNTER_SOURCE_OVERFLOW_VAL)
				timerCounter -= TCNTER_SOURCE_OVERFLOW_VAL;
			timer=0;
			printf("TCNT: %d, tcnter=%d\n",timerCounter, tcnter_get());
		}

		tcnter_update();

	}


	return 0;
}

