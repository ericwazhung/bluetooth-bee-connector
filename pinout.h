#ifndef __PINOUT_H__
#define __PINOUT_H__


#define HEART_PINNUM PB1
#define HEART_PINPORT PORTB

//Apparently the new pinout is identical to the old as far as Rx/Tx, 
//  but puart0 and puart1 are swapped
//  and puart1 is on the programming-header
//      (adding RST to the tablet, to enable programming while attached)

//J2ish puart0 (to BTBee):
//   uC					BTBee
//--------------------------------
//1 +						+
//2 Rx0 = PB3			Tx
//3 Tx0 = PB4			Rx
//4 GND					GND

#define Rx0pin    PB3
#define Rx0PORT   PORTB

#define Tx0pin    PB4
#define Tx0PORT   PORTB

#define PUAT_TO_BTB     0
#define PUAR_FROM_BTB   0

//J4 puart1 (To Tablet) (also programming header)
//   uC						Tablet
//---------------------------------
//1 GND						GND=14
//2 +							+ = 13
//3 Rx1 = SCK = PB2		Tx = 9
//4 Tx1 = MOSI = PB0		Rx = 10
//5 /RST						Rst = 12
//6 (heart/GPIO) MISO = PB1
//
//(5=/RST, add onboard 1.5k pull up

#define Rx1pin    PB2
#define Rx1PORT   PORTB

#define Tx1pin    PB0
#define Tx1PORT   PORTB

#define PUAT_TO_DEVICE     1
#define PUAR_FROM_DEVICE   1


/* OLD:
//J1 puart0: (To Bluetooth Bee)
// 1 +
// 2 Rx0 = PB2
// 3 Tx0 = PB0
// 4 GND

#define Rx0pin    PB2 //SCL_PIN
#define Rx0PORT   PORTB //SCLPORT

#define Tx0pin    PB0 //SDA_PIN
#define Tx0PORT   PORTB //SDAPORT

#define PUAT_TO_BTB     0
#define PUAR_FROM_BTB   0

//J2 puart1: (To Tablet)
// 1 +
// 2 Rx1 = PB3
// 3 Tx1 = PB4
// 4 GND

#define Rx1pin    PB3
#define Rx1PORT   PORTB

#define Tx1pin    PB4
#define Tx1PORT   PORTB

#define PUAT_TO_DEVICE     1
#define PUAR_FROM_DEVICE   1
*/

#endif

