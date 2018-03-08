#include "stm32f30x.h"

void delay_ms(int time)
{
int i=0;
	for(i=0; i<time*7200; i++)
		{
			__ASM("nop"); //1 sec
		}
}
