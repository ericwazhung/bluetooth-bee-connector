// This needn't (shouldn't?) be included anywhere in the project code...
//  It's included in polled_uat.c
// Not sure how to tie it all together with main.h or whatever, for e.g.
//   the #defines for Tx0pin, or if global variables were used, etc.

//#define Tx0pin    PB0 //SDA_PIN
//#define Tx0PORT   PORTB //SDAPORT
/*
#define Tx0pin    PB0 //SDA_PIN
#define Tx0PORT   PORTB //SDAPORT

#define Tx1pin    PB4
#define Tx1PORT   PORTB
*/
#include "pinout.h"

//void puat_writeOutput(uint8_t puatNum, uint8_t value)
//	   __attribute__((__always_inline__));

//static 
extern __inline__ void puat_writeOutput(uint8_t puatNum, uint8_t value)
{
	if(puatNum == 0)
		writepinPORT(Tx0pin, Tx0PORT, value);
	else
		writepinPORT(Tx1pin, Tx1PORT, value);
}

