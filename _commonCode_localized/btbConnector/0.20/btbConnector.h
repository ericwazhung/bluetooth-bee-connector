// btbConnector.c 0.20-2
//		a state-machine to automate connecting via the bluetooth bee
//
//0.20-2 fixing when INLINE, can't "make localize"
//0.20-1 FIXED: "BTSTATE:1" as disconnect message no longer detected
//              (trailing \r caused reset of detection)
//0.20   repeating data received from the device to the host
//       while connected
//       FIXED: cirBuff read, even if not ready to send
//          (lost cirBuff data)
//0.10-3 replacing state++'s with state='s
//       Apparently the optimizer did the same, I just looked in the wrong
//       place (one of the multi-use states?)
//       compile-size is identical, and now there's *some* risk I mighta
//       put a wrong state somewhere...
//0.10-2 inlining...
//0.10-1 btbc_btbReadyForData, etc.
//			fix minor error: 
//				device sends '\r' while connected, then disconnects
//				second '\r' wouldn't be received as a discon message
//0.10 first step toward commonFiling

// Basic Usage:
// ~/project/main.c:
//  main() {
//   btbc_init();
//   while(1){
//    btbc_update(getReceivedByte())
//   }
//  }
//
// ~/project/btbcStuff.h:
//  extern void uartSendByte(char byte);
//
//  extern __inline__ void btbc_sendByteToBTB(uint8_t byte)
//  {
//			uatSendByte(BTB_UAT, byte)
//  }
//
//  extern __inline__ void btbc_sendByteToDevice(uint8_t byte)
//  {
//			uatSendByte(DEVICE_UAT, byte);
//  }
//
//  extern __inline__ uint8_t btbc_deviceReadyForData(void)
//  {
//     return uat_isEmpty(DEVICE_UAT);
//  }
//
//  extern __inline__ uint8_t btbc_btbReadyForData(void)
//		 return uat_isEmpty(BTB_UAT);

#ifndef __BTBCONNECTOR_H__
#define __BTBCONNECTOR_H__

#include <stdint.h>
#include _CIRBUFF_HEADER_


void btbc_init(void);

//If rxByte <= 0, it means no data has been received
// Redirect data from the receiver to this argument
void btbc_update(int16_t rxByteFromBTB, int16_t rxByteFromDevice);

#if (defined(_BTBC_INLINE_) && _BTBC_INLINE_)
	#define BTBC_INLINEABLE extern __inline__
	#include "btbConnector.c"
#else
	#define BTBC_INLINEABLE	//Nothing Here
#endif


#endif

