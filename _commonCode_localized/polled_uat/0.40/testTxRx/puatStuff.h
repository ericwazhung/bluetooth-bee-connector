

extern __inline__ void puat_writeOutput(uint8_t puatNum, uint8_t value)
{
	extern int lineVal[];

	lineVal[puatNum]=value;
	printf("writeOutput[%d]: %d\n", puatNum, value);
}  

