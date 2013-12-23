// btbConnector.c 0.10

#include _BTBCONNECTOR_HEADER_
#include "btbcStuff.h"


#if (defined(BTBC_PC_DEBUG) && BTBC_PC_DEBUG)
   #include <stdio.h>
   #define DPRINT printf
#else
   #define DPRINT(...) {}
#endif

//Can't think of a name for this...
// data coming from the Bluetooth Bee, itself, rather than from a host
#define BTBBUFFLEN 20
uint8_t btbBuffer[BTBBUFFLEN];
cirBuff_t btbCirBuff;
uint8_t btbDeviceBuffer[BTBBUFFLEN];
cirBuff_t btbDeviceBuff;

//Inlining saves 6 Bytes
BTBC_INLINEABLE
void btbc_init(void)
{
	cirBuff_init(&btbCirBuff, BTBBUFFLEN, btbBuffer);
	cirBuff_init(&btbDeviceBuff, BTBBUFFLEN, btbDeviceBuffer);
}

char btbcCmd_Reset[]="\r\n+STWMOD=0\r\n";
char btbcCmd_Inquire[]="\r\n+INQ=1\r\n";
char *btbcCommand = "\0";

//These are the actual responses, the manual says differently
char btbcErrorResponse[]="ERROR";
// Doesn't work when prefixed with "\r\n" ?!
// ALSO: detecting +BTSTATE:1\r\n as an indicator for disconnect->reset
//       doesn't work, since \r is used to restart detection
//       so remove \r\n from the end...
char btbcResponse_BTSTATE1[]="\n+BTSTATE:1"; //\r\n";	//Ready
char btbcResponse_BTSTATE2[]="+BTSTATE:2\r\n";	//Inquiring
char btbcResponse_BTSTATE4[]="+BTSTATE:4\r\n";	//Connected
char *btbcResponse = "\0";
// Cannot increment the pointer, because it might need to be reset
//  e.g. BTSTATE:0 received, when looking for BTSTATE:1
uint8_t btbcResponseIndex = 0;
uint8_t btbcErrorResponseIndex = 0;

uint8_t btbcState;
#define BTBCSTATE_SENDRESET				0
#define BTBCSTATE_SENDINGRESET			1
#define BTBCSTATE_AWAITBTSTATE1 			2
#define BTBCSTATE_AWAITINGBTSTATE1		3
#define BTBCSTATE_SENDINQUIRE				4
#define BTBCSTATE_SENDINGINQUIRE			5
#define BTBCSTATE_AWAITBTSTATE2			6
#define BTBCSTATE_AWAITINGBTSTATE2		7
#define BTBCSTATE_AWAITCONNECTION		8
#define BTBCSTATE_AWAITINGCONNECTION	9
#define BTBCSTATE_CONNECTRECEIVED		10
#define BTBCSTATE_CONNECTED				11
#define BTBCSTATE_VERIFYINGCONNECTION	12


//4106 pre-inlining, 4132 after
// 4120 inlined WITH puat_dataWaiting inlined
// 4094 not inlined WITH puat_dataWaiting inlined
// 4072 not inlined WITH puat_sendByte inlined
// 4098 inlined WITH puat_sendByte inlined
BTBC_INLINEABLE	//This INCREASES code size (WTF?) 26Bytes
void btbc_update(int16_t rxByteFromBTB, int16_t rxByteFromDevice)
{
#warning "THIS SHOULD NOT BE TESTED WHILE CONNECTED! Or..?"
	//OOOh FANCY! This could be merged into a single function
	// to be made inline... with AWATING... states' functionality
	// duh.
	if(btbcErrorResponse[btbcErrorResponseIndex] == '\0')
	{
		DPRINT("ERROR Received\n");
		btbcErrorResponseIndex = 0;
		btbcState = BTBCSTATE_SENDRESET;
	}
	else if(rxByteFromBTB >= 0)
	{
		//Who knows where "Error" might come from... probably a good idea
		// to look for it (?)
		// But maybe not while connected...?
		//Luckily 'E' is only used once in the error response
		if(rxByteFromBTB == btbcErrorResponse[0])
			btbcErrorResponseIndex = 1;
		else if(rxByteFromBTB == btbcErrorResponse[btbcErrorResponseIndex])
			btbcErrorResponseIndex++;
		else
			btbcErrorResponseIndex = 0;
	}


	switch(btbcState)
	{
		case BTBCSTATE_SENDRESET:
			btbcCommand = btbcCmd_Reset;
			//btbcState++;
			btbcState = BTBCSTATE_SENDINGRESET;
			DPRINT("Sending Reset\n");
			break;
		//Sending States:
		case BTBCSTATE_SENDINGRESET:
		case BTBCSTATE_SENDINGINQUIRE:
			//if(uatReady)
			//	if(*btbcCommand != '\0')
			//	{
			// Assumes btbcCommand[0] != '\0'
			// should be safe state-wise
					if(btbc_btbReadyForData())
					{
						btbc_sendByteToBTB(*btbcCommand);
						btbcCommand++;

						if(*btbcCommand == '\0')
							btbcState++;
					}
			//	}
			//	else
			//		btbcState++;
			break;
		case BTBCSTATE_AWAITBTSTATE1:
			btbcResponse = btbcResponse_BTSTATE1;
			btbcResponseIndex = 0;
			//btbcState++;
			btbcState = BTBCSTATE_AWAITINGBTSTATE1;
			DPRINT("Awaiting BTSTATE:1 (Ready)\n");
			break;
		//Receiving States:
		case BTBCSTATE_AWAITINGBTSTATE1:
		case BTBCSTATE_AWAITINGBTSTATE2:
		case BTBCSTATE_AWAITINGCONNECTION:
			if(btbcResponse[btbcResponseIndex] == '\0')
				btbcState++;
			else if(rxByteFromBTB >= 0)
			{
				if(rxByteFromBTB == btbcResponse[btbcResponseIndex])
					btbcResponseIndex++;
				else
					btbcResponseIndex = 0;
			}
			break;
		case BTBCSTATE_SENDINQUIRE:
			DPRINT("BTSTATE1, Bluetooth Bee ready to receive INQ\n");
			btbcCommand = btbcCmd_Inquire;
			//btbcState++;
			btbcState = BTBCSTATE_SENDINGINQUIRE;
			break;
		//BTBCSTATE_SENDINGINQUIRE is merged above
		case BTBCSTATE_AWAITBTSTATE2:
			btbcResponse = btbcResponse_BTSTATE2;
			btbcResponseIndex = 0;
			//btbcState++;
			btbcState = BTBCSTATE_AWAITINGBTSTATE2;
			DPRINT("Awaiting BTSTATE:2 (Inquiring)\n");
			break;
		//BTBCSTATE_AWAITINGBTSTATE2 is merged above
		case BTBCSTATE_AWAITCONNECTION:
			DPRINT("BTSTATE2, Bluetooth Bee ready to receive connection\n");
			btbcResponse = btbcResponse_BTSTATE4;
			btbcResponseIndex = 0;
			//btbcState++;
			btbcState = BTBCSTATE_AWAITINGCONNECTION;
			break;
		//BTBCSTATE_AWAITINGCONNECTION is merged above
		case BTBCSTATE_CONNECTRECEIVED:
			DPRINT("Bluetooth Bee connected (via bluetooth) to host\n");
			//btbcState++;
			btbcState = BTBCSTATE_CONNECTED;
			break;
		case BTBCSTATE_CONNECTED:
			{
				//static uint8_t withholding = FALSE;

				//In most cases, this'll be retransmitted immediately after
				// making sure it's not possibly the start of a
				// btb disconnect message
				if(rxByteFromBTB >= 0)
					cirBuff_addByte(&btbCirBuff, rxByteFromBTB, TRUE);

				//While connected, we repeat the data from the device
				// to the host
				// BUT it's halted if we're receiving a message from btb
				if(rxByteFromDevice >= 0)
					cirBuff_addByte(&btbDeviceBuff, rxByteFromDevice, TRUE);

				if(rxByteFromBTB == '\r')
				{
				//	withholding = TRUE;
					DPRINT("'\\r' received, witholding data"
						  "to verify it's not a bt notice (e.g. disconnect)\n");
					//Data should be withheld in *both* directions...
			
					// When the host disconnects from the "serial port"
					//  (via bluetooth)
					// OR when bluetooth is disabled on the host
					// the btb sends BTSTATE:1
					// WHAT ABOUT OTHER DISCONNECTS?
					btbcResponse = btbcResponse_BTSTATE1;
					btbcResponseIndex = 0;	
					btbcState = BTBCSTATE_VERIFYINGCONNECTION;
				}
				else
				{
					//int16_t cirBuffByte = cirBuff_getByte(&btbCirBuff);

//#warning "need to resend data in the other direction!"
					if(btbc_deviceReadyForData())
					{
						int16_t cirBuffByte = cirBuff_getByte(&btbCirBuff);

						if(cirBuffByte >= 0)
						{
							btbc_sendByteToDevice(cirBuffByte);
							DPRINT("Retransmitting to device 0x%x='%c'\n", 
									(int)(cirBuffByte), (char)(cirBuffByte));
						}
					}

					if(btbc_btbReadyForData())
					{
						int16_t cirBuffByte = cirBuff_getByte(&btbDeviceBuff);

						if(cirBuffByte >= 0)
						{
							btbc_sendByteToBTB(cirBuffByte);
							DPRINT("Retransmitting to host 0x%x='%c'\n",
									(int)(cirBuffByte), (char)(cirBuffByte));
						}
					}
				}

			}
			break;
		case BTBCSTATE_VERIFYINGCONNECTION:
			//Keep buffering Tablet Data in case we're still connected
			if(rxByteFromDevice >= 0)
				   cirBuff_addByte(&btbDeviceBuff, rxByteFromDevice, TRUE);

			if(btbcResponse[btbcResponseIndex] == '\0')
			{
				DPRINT("Host disconnected\n");
				//AWAITING_CONNECTION should work for most cases
				// except if the bluetooth was disabled at the host
				// then inquiry would be required...
				btbcState = BTBCSTATE_SENDRESET; // or reset?

				//Clear the circular buffer...
				cirBuff_empty(&btbCirBuff);
				cirBuff_empty(&btbDeviceBuff);
			}
			else if(rxByteFromBTB >= 0)
			{
				//Buffer the received bytes, they may be host commands to the
				// tablet...
				cirBuff_addByte(&btbCirBuff, rxByteFromBTB, TRUE);

				// Unusual case, but it's happened in testing...
				//  Device Sends '\r' then disconnects
				//  (a second '\r' is sent, indicating the start of the 
				//   disconnect message, but it won't be read, without this)
				if(rxByteFromBTB == '\r')
					btbcResponseIndex = 0;
				if(rxByteFromBTB == btbcResponse[btbcResponseIndex])
					btbcResponseIndex++;
				else	//Not an expected btb response...
				{
					DPRINT("... apparently wasn't a disconnect notice."
							 " Need to Resend the buffered data\n");
					btbcState = BTBCSTATE_CONNECTED;
				}
			}
			break;
		default:
			break;
	}
}


