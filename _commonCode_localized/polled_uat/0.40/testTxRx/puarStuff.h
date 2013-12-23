
//extern int lineVal[];

extern __inline__ uint8_t puar_readInput(uint8_t puarNum)
{
	extern int lineVal[];
	// CFLAGS += -D'getbit(...)=(lineVal)'
	   return lineVal[puarNum];
}  

