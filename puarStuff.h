
#include "pinout.h"

extern __inline__ uint8_t puar_readInput(uint8_t puarNum)
{
   if(puarNum == 0)
      return getpinPORT(Rx0pin, Rx0PORT);
   else
      return getpinPORT(Rx1pin, Rx1PORT);
}

