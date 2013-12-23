// btbConnector Tester
//		- A program (PC-based) for interfacing with the Blootooth Bee
//      Attempting to automate connection, etc.
//		  Eventually to be merged into tabletBluetoother
//

#include <stdio.h>
#include <stdint.h>

#include "../../../__std_wrappers/stdin_nonBlock/0.10/stdin_nonBlock.h"
#include "../../../__std_wrappers/serIn_nonBlock/0.10/serIn_nonBlock.h"
#include "../../../__std_wrappers/serOut_nonBlock/0.10/serOut_nonBlock.h"
#include _BTBCONNECTOR_HEADER_


char serPort[100] = "/dev/cu.usbserial";
speed_t baud = 19200;


int main(void)
{
   int quit = 0;
   int timer = 0;
	char txChar = 'a';

	btbc_init();

   // Set STDIN non-blocking (Still requires Return)
	stdinNB_init();
	serInNB_init();
	serOutNB_init();

	printf(" Expects a Bluetooth Bee to be connected:\n"
			 "  /dev/cu.usbserial @ 19200\n");
   printf("---Press q and press return to quit---\n");

   while(!quit)
   {
      errno_handleError(" Unhandled Error.", 0);
				
		int kbChar = stdinNB_getChar();

      switch(kbChar)
      {
			case -1:		//Nothing received...
				break;
         case 'q':
            quit = 1;
            break;
         default:
            printf("KB Received: '%c'", (unsigned char)kbChar);
      }

		int rxTemp = serInNB_getByte();
		if(rxTemp >= 0)
			printf("SerIn Received: 0x%x='%c'\n", rxTemp, (unsigned char)rxTemp);

		btbc_update(rxTemp);

      timer++;

      if(timer == 100000)
      {
         timer=0;
         printf(".\n");
      }

   }


   return 0;
}


