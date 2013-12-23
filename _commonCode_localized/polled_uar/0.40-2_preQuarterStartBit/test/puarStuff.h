
extern int lineVal[];

extern __inline__ uint8_t puar_readInput(uint8_t puarNum)
{
	// CFLAGS += -D'getbit(...)=(lineVal)'
	   return lineVal[puarNum];
}  

