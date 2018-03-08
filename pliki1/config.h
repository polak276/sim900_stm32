#ifndef _CONFIG_H
#define _CONFIG_H

//funkcje konfiguracyjne

void RCC_Config(void);
void NVIC_Config(void);
void SPI_Config(void);
void GPIO_Config(void);
unsigned long int SysTick_Config_Mod(unsigned long int SysTick_CLKSource, unsigned long int Ticks);
void USART_Config(void);

typedef enum 
{
	STOP,
	DO_PRZODU,
	PRZEWIJAJ,
	KONKRETNA
}tryb_wyswietlacza;

#endif
