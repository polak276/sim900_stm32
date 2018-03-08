#include <string.h>

#include "stm32f30x.h"
#include "uart.h"
#include "delay_1.h"
#include "config.h"
#include "ff.h"
#include "sd1.h"

extern volatile char licznik;
extern volatile char received;
extern volatile char nr_smsa;
extern volatile char * komenda;
volatile char new_message=0;
extern char bufor[100];
extern char odebrano_sms[];
extern char odebrano_sms_1[];
extern char next[];
extern char rewind[];
extern char stop[];
extern char names[];
extern volatile char sms_received;
extern volatile tryb_wyswietlacza tryb;
extern char tmp_komenda[40];
extern char AT_Usun_all[];
extern char get_sms[15];
extern volatile char ask_for_names;
extern char AT_Wyslij_sms[];
extern char AT_Wiadomosc_sms[];
extern FRESULT fresult;
extern DIR Dir;
extern FILINFO PlikInfo;
extern char nazwy_plikow[255];
extern char wybor[]; //wybieramy konkretne zdjecie
extern char nazwa_pliku_bmp[20];
extern char ustawiono_zdjecie[40];
extern char brak_pliku[40];
extern volatile tryb_wyswietlacza last_tryb;

void uart_receive(char bajt)
{
	char bufor_tmp[6];
	
	switch(licznik)
		{
			case 0:
					if(bajt == 0x0D)
						{
							licznik++;
						}
					else
						licznik=0;
			break;
					
			case 1:
					if(bajt == 0x0A)
						{
							licznik++;
							new_message=1;
						}
					else
						licznik=0;
			break;
					
			default:
					if( (licznik==2) && (bajt==0x0D) )
					{
						licznik=1;
						new_message=0;
					}

					if(new_message)
					{		
							bufor[licznik-2] = bajt;
							licznik++;
							
							if(bajt == 0x0A) //koniec wiadomosci
							{
								memcpy(&bufor_tmp[0], &bufor[0], 5);
								memset(&bufor_tmp[5], '\0', 1); //znak konca
								
								if (strcmp(&odebrano_sms_1[0], &bufor_tmp[0] )== 0)
								{
									nr_smsa = bufor[licznik-2-3]; //numer pod ktorym jest nowy sms
									bufor[licznik-2-3] = '\0';
									bufor[licznik-2-2] = '\0';
									bufor[licznik-2-1] = '\0';
									licznik=0;
									new_message=0;
									sms_received=1;
									clear_rx();
								}
								else if (strcmp(&next[0], &bufor_tmp[0] )== 0)
								{
									clear_rx();
									new_message=0;
									licznik=0;
									tryb = DO_PRZODU;
								}
								else if (strcmp(&rewind[0], &bufor_tmp[0] )== 0)
								{
									clear_rx();
									new_message=0;
									licznik=0;
									tryb= PRZEWIJAJ;
								}
								else if (strcmp(&stop[0], &bufor_tmp[0] )== 0)
								{
									clear_rx();
									new_message=0;
									licznik=0;
									tryb= STOP;
								}		
								else if (strcmp(&names[0], &bufor_tmp[0] )== 0)
								{
									clear_rx();
									new_message=0;
									licznik=0;
									ask_for_names=1;
								}								
								else if (strcmp(&wybor[0], &bufor_tmp[0] )== 0)
								{
									clear_nazwa_pliku_bmp();
									strcpy(&nazwa_pliku_bmp[0], &bufor[6]);
									
									nazwa_pliku_bmp[strlen(&nazwa_pliku_bmp[0]) - 2] = '.';
									nazwa_pliku_bmp[strlen(&nazwa_pliku_bmp[0]) - 1] = 'B';
									nazwa_pliku_bmp[strlen(&nazwa_pliku_bmp[0])] = 'M';
									nazwa_pliku_bmp[strlen(&nazwa_pliku_bmp[0])] = 'P';
									nazwa_pliku_bmp[strlen(&nazwa_pliku_bmp[0])] = 0;
									
									new_message=0;
									licznik=0;
									clear_rx();
									last_tryb=tryb;
									tryb=KONKRETNA;
								}								
								else //komenda nieznana
								{
									licznik=0;
									new_message=0;
									clear_rx();
								}							
							}
					}
			break;
		}
}

void send_command_wait100ms(char * command)
{
	komenda = command;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	delay_ms(100);
}

void clear_rx(void)
{
	int i=0;
	
	for(i=0; i< BUFFER_SIZE; i++)
		bufor[i]=0;
}

void send_command_to_sim900(char * command, int opoznienie)
{
		komenda = &command[0];
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		delay_ms(opoznienie);	
}

void get_message()
{
	sms_received=0;
	new_message=0;
	memcpy(&tmp_komenda[0], &get_sms[0], 8); //
	tmp_komenda[8]=nr_smsa; //dodac indeks smsa w pamieci sim900
	tmp_komenda[9]=0x0D;
	send_command_to_sim900(&tmp_komenda[0], 100); //prosimy o tresc smsa
	send_command_to_sim900(&AT_Usun_all[0], 300); //po otrzymaniu go czyscimy skrzynke sim900
}

void send_names()
{
	char indeks=0;
	memset(&nazwy_plikow[0],0, 255);
	
	//wpisanie nazw plikow do wiadomosci
	fresult = f_opendir(&Dir, "0:/");
	fresult = f_opendir(&Dir, "0:/");
	fresult = f_readdir(&Dir, &PlikInfo);
	fresult = f_readdir(&Dir, &PlikInfo);
	
	while(PlikInfo.fname[0])
	{	
		strcpy(&nazwy_plikow[indeks],&PlikInfo.fname[0]);
		indeks=strlen(&nazwy_plikow[0])-4;
		nazwy_plikow[indeks]=0x20;
		indeks+=1;
		fresult = f_readdir(&Dir, &PlikInfo);
	}
	
	memset(&nazwy_plikow[strlen(&nazwy_plikow[0])-4],0x1A, 1);
	send_command_to_sim900(&AT_Wyslij_sms[0], 500);
	send_command_to_sim900(&nazwy_plikow[0], 500);
}

//funkcja odsylajaca potwierdzenie o ustawieniu zdjecia
void odeslij_potwierdzenie()
{
	strcpy(&ustawiono_zdjecie[28], &nazwa_pliku_bmp[0]);
	memset(&ustawiono_zdjecie[strlen(ustawiono_zdjecie)-4], 0x1A, 1);
	memset(&ustawiono_zdjecie[strlen(ustawiono_zdjecie)-3],0,2);
	send_command_to_sim900(&AT_Wyslij_sms[0], 500);
	send_command_to_sim900(&ustawiono_zdjecie[0], 300);
	GPIO_WriteBit(GPIOE, GPIO_Pin_15, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_15)));
}

void odeslij_blad()
{
	tryb=last_tryb;
	strcpy(&brak_pliku[21], &nazwa_pliku_bmp[0]);
	memset(&brak_pliku[strlen(brak_pliku)-4], 0x1A, 1);
	memset(&brak_pliku[strlen(brak_pliku)-3],0,2);
	send_command_to_sim900(&AT_Wyslij_sms[0], 500);
	send_command_to_sim900(&brak_pliku[0], 300);
}
