#include "projInfo.h"	//Don't include in main.h 'cause that's included in other .c's?
#include "main.h"


#warning "TODO: Use Disconnect. E.G. btbc resets while btb is connected. Reset Command is interpretted by connected BTB as data to transmit"
#if !defined(__AVR_ARCH__)
 #error "__AVR_ARCH__ not defined!"
#endif

#include "pinout.h"
/*
//J1 puart0: (To Bluetooth Bee)
// 1 +
// 2 Rx0 = PB2
// 3 Tx0 = PB0
// 4 GND

#define Rx0pin    PB2 //SCL_PIN
#define Rx0PORT   PORTB //SCLPORT

#define Tx0pin		PB0 //SDA_PIN
#define Tx0PORT	PORTB //SDAPORT

#define PUAT_TO_BTB		0
#define PUAR_FROM_BTB	0

//J2 puart1: (To Tablet)
// 1 +
// 2 Rx1 = PB3
// 3 Tx1 = PB4
// 4 GND

#define Rx1pin    PB3
#define Rx1PORT   PORTB

#define Tx1pin		PB4
#define Tx1PORT	PORTB

#define PUAT_TO_DEVICE		1
#define PUAR_FROM_DEVICE	1
*/

// Heartbeat on PB1 / MISO
//J4 AVRProg:
// 1 GND
// 2 +
// 3 SCK		PB2 / Rx0
// 4 MOSI	PB0 / Tx0
// 5 /RST	(PB5 unusable)
// 6 MISO 	PB1 / Heartbeat / HeartInput (active-low)
//uint8_t heartBlinkInternal = 0;

//#define  LED_R PB4
//#define  LED_G PB3
//#define	LED_B HEART_PINNUM   //NYI
//#define LED_PORT PORTB

/*
//static __inline__
void puat_writeOutput(uint8_t puatNum, uint8_t value)
	__attribute__((__always_inline__));

void puat_writeOutput(uint8_t puatNum, uint8_t value)
{
//	if(puatNum == 0)
		writepinPORT(Tx0pin, Tx0PORT, value);
//	else
//		writepinPORT(Tx1pin, Tx1PORT, value);
}
*/
//extern __inline__ uint8_t puar_readInput(uint8_t puarNum)
//{
//	if(puarNum == 0)
//		return getpinPORT(Rx0pin, Rx0PORT);
//	else
//		return getpinPORT(Rx1pin, Rx1PORT);
//}


//int main(void) __attribute__((flatten));

int main(void)
{
	//Found experimentally: assuming the free-running ADC is always 13
	// cycles per interrupt...
	// The default value was read to be 0x9f
	// This is of course device-specific
//	OSCCAL = 0x9a;

	//*** Initializations ***

	//!!! WDT could cause problems... this probably should be inited earlier and called everywhere...
	//INIT_HEARTBEAT(HEARTBEATPIN, HEARTBEAT, HEARTCONNECTION);


	setoutPORT(Tx0pin, Tx0PORT);
	setinpuPORT(Rx0pin, Rx0PORT);

	setoutPORT(Tx1pin, Tx1PORT);
	setinpuPORT(Rx1pin, Rx1PORT);

//	init_heartBeat();
//	setHeartRate(0);



	//Timer0 TCNT0 increments at 1MHz, 0-255->0-255 (OCR value not used)
	// Not using DMS, because I'd rather not use interrupts
	// so there's no lag-time in the main-loop (for polling)
	// TODO: timer_common doesn't address this device, specifically
	//   so there would be some wisdom in verifying its functionality
	//   esp. 8-bit, etc.
	timer_init(0, CLKDIV8, WGM_NORMAL);
	tcnter_init();

	btbc_init();

	myTcnter_t puar0_bitTcnt;


	//Just add a delay so the pull-up works, etc...
	// ALSO check the heart-input, if low, go into puatTest mode
	uint8_t puatTest = FALSE;
	puar0_bitTcnt = tcnter_get();
	while(tcnter_get() - puar0_bitTcnt < 3000000)
	{
		tcnter_update();
//		if(!heartPinInputPoll())
//			puatTest = TRUE;
//		heartUpdate();

	}

/*
	puar0_bitTcnt = puar_calibrateBitTcnt(0);

	//Don't forget, nibbles are reversed, so 0x23 blinks as
	// 3 blinks, then 2
	set_heartBlink(puar0_bitTcnt);
*/

	//1MHz/38400 = 26.04 (104=9600)
	// Now running at 16MHz->2MHz 
	puar0_bitTcnt = 104; //52; //26; //104; //26;

	uint8_t puar;
	for(puar=0; puar<NUMPUARS; puar++)
	{
		puar_setBitTcnt(puar, puar0_bitTcnt);
		puar_init(puar);
	}

	uint8_t puat;
	for(puat=0; puat<NUMPUATS; puat++)
	{
		puat_setBitTcnt(puat, puar0_bitTcnt);
		puat_init(puat);
	}

/*
	uint8_t osccalling = TRUE;
	myTcnter_t nextOsccalTime = tcnter_get() + 2000000;
	myTcnter_t nextByteTime = tcnter_get() + 20000;


	if(puatTest)
	{
		set_heartBlink(1);
		while(1)
		{
		 //	heartUpdate();	
			tcnter_update();
			puat_update(0);
			puar_update(0);
			if(!puat_dataWaiting(0) && (tcnter_get() >= nextByteTime))
			{
				//10ms...
				nextByteTime = tcnter_get() + 20000;
				puat_sendByte(0, 'V');
			}

			if((osccalling) && (tcnter_get() >= nextOsccalTime))
			{
				OSCCAL++;
				nextOsccalTime += 2000000;	//a second for each value...
			}
			
			if(!heartPinInputPoll())
			{
				nextOsccalTime = tcnter_get() + 2000000;
				osccalling = !osccalling; //FALSE;
			}
		}
	}
*/

	//Timer0 TCNT0 increments at 1MHz, 0-255->0-255 (OCR value not used)
	// Not using DMS, because I'd rather not use interrupts
	// so there's no lag-time in the main-loop (for polling)
	// TODO: timer_common doesn't address this device, specifically
	//   so there would be some wisdom in verifying its functionality
	//   esp. 8-bit, etc.
	//timer_init(0, CLKDIV8, WGM_NORMAL);

	//tcnter_init();



//	init_heartBeat();

//	setHeartRate(0);	

	//Blink until the EDID is read...
	//set_heartBlink(1);

	//This was only necessary for debugging timer initialization bugs...
	// which have been resolved
//	set_heartBlink(retVal);

	setoutPORT(HEART_PINNUM, HEART_PINPORT);

	while(1)
	{
		togglepinPORT(HEART_PINNUM, HEART_PINPORT);
		tcnter_update();
	/*	
		for(puat=0; puat<NUMPUATS; puat++)
		{
			puat_update(puat);
			
	//for(puar=0; puar<NUMPUARS; puar++)
		   puar_update(puat);
		}
		*/
		//MUCH faster than for-looping...
		puat_update(0);
		puat_update(1);
		puar_update(0);
		puar_update(1);

		int16_t rxByteFromBTB = -1;
		int16_t rxByteFromDevice = -1;

		// If there's data received, then retransmit it
		if(puar_dataWaiting(PUAR_FROM_BTB))
			rxByteFromBTB = puar_getByte(PUAR_FROM_BTB);
	
		if(puar_dataWaiting(PUAR_FROM_DEVICE))
			rxByteFromDevice = puar_getByte(PUAR_FROM_DEVICE);

		//Echo...
		if(puatTest && (rxByteFromBTB >= 0))
			puat_sendByte(PUAT_TO_BTB, rxByteFromBTB);

		btbc_update(rxByteFromBTB, rxByteFromDevice);
	}

/*
				//If the buffer's full, the only thing we can do is not
				// retransmit... seems unlikely since the baud rates are
				// identical, but who knows... so let's indicate an error
				if(puat_dataWaiting(puar))
				{
//					set_heartBlink(1);
				}
				else
					puat_sendByte(puar, byte);
		   }
		}
*/
//		if(!puat_dataWaiting(0))
//			puat_sendByte(0, 'U');		

		// Yep, definitely interferes...
//		heartUpdate();
//	}

}


