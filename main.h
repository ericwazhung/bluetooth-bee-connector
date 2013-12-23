#ifndef __MAIN_H__
#define __MAIN_H__

#include <avr/io.h>
#include <inttypes.h>
//#include <stdlib.h>

/* Interrupt Handling */
//#include <avr/interrupt.h>

//#include "../../_common/bithandling/0.94/bithandling.h"
#include _BITHANDLING_HEADER_
//#include _PWM_HEADER_
//#include _HEARTBEAT_HEADER_
//#include _ADC_HEADER_
#include _POLLED_UAR_HEADER_
#include _POLLED_UAT_HEADER_
#include _TIMERCOMMON_HEADER_
#include _BTBCONNECTOR_HEADER_

//The EDID Product ID will be reported as <ProductNum><ProjectVersion>

// No longer relevent... see makefile...
//#define HEARTBEAT    	PA3 
//#define HEARTBEATPIN 	PINA
//#define HEARTCONNECTION	LED_TIED_HIGH

#warning "TODO: Try to reduce tcnter int-size... Would probably be *dramatic*"

#endif

