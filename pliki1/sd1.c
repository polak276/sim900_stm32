#include <stdlib.h>
#include <string.h>

#include "stm32f30x.h"
#include "ff.h"
#include "sd1.h"
#include "ff.h"
#include "drawing1.h"
#include "ssd1963.h"
#include "colors.h"
#include "config.h"
#include "delay_1.h"
#include "uart.h"

extern uint8_t Red;
extern uint8_t Green;
extern uint8_t Blue;
extern FILINFO PlikInfo;
extern FILINFO (*p)[10];
extern FILINFO PlikInfo;
extern DIR Dir;
extern FRESULT fresult;
extern volatile tryb_wyswietlacza tryb;
extern char nazwa_pliku_bmp[20];
extern char * lista_plikow[30];

void sd_power_on()
{
	GPIO_WriteBit(GPIOE, GPIO_Pin_1, Bit_RESET);
}


//czytanie obrazu bmp z pliku na karcie SD
void lcd_bmp(char * nazwa_pliku)
{
	uint32_t i=0, j=0, liczba_bajtow=0, ile_bajtow=0, ilosc_bajtow_graficznych=0, szerokosc_poczatek=0, wysokosc_poczatek=0;
	uint8_t temp[4];
	FIL plik;
	uint16_t a=0, b=0;
	uint16_t szerokosc_obrazka=0, wysokosc_obrazka=0;
	
	f_open(&plik,(const char *) nazwa_pliku, FA_READ);
	f_open(&plik,(const char *) nazwa_pliku, FA_READ);
	f_read(&plik, (uint8_t *) &temp[0], 2, &ile_bajtow); //2 puste
	f_read (&plik, (u8*) &liczba_bajtow, 4, &ile_bajtow);
	f_read(&plik, (uint8_t *) &temp[0], 4, &ile_bajtow); //4 omijamy
	f_read(&plik, (u8*) &i, 4, &ile_bajtow);
	f_read(&plik, (u8*) &temp[0], 4, &ile_bajtow); //offset 54 bajty
	f_read(&plik, (u8*) &szerokosc_obrazka, 4, &ile_bajtow);
	f_read(&plik, (u8*) &wysokosc_obrazka, 4, &ile_bajtow); // szer x wys
	
	for(j = 0; j < i-26; j++)
		f_read(&plik, &temp[0], 1, &ile_bajtow);	//
	
  ilosc_bajtow_graficznych = (liczba_bajtow - i);// Liczba pikseli obrazu = (rozmiar pliku - offset)/3 bajty na pisel
	ilosc_bajtow_graficznych=ilosc_bajtow_graficznych/3;
	szerokosc_poczatek = (TFT_WIDTH - szerokosc_obrazka)/2; // poczatek zapisu obrazka tak zeby byl wycentrowany
	wysokosc_poczatek = (TFT_HEIGHT - wysokosc_obrazka)/2;
	SSD1963_SetArea(szerokosc_poczatek, szerokosc_poczatek + szerokosc_obrazka-1 , wysokosc_poczatek, wysokosc_poczatek + wysokosc_obrazka-1);
	SSD1963_WriteCommand(0x2c); //start
	
	for(i = 0; i < ilosc_bajtow_graficznych; i++)	// odczyt z sd, wyslanie na lcd
	{
		f_read (&plik, &Blue, 1, &ile_bajtow);
		f_read (&plik, &Green, 1, &ile_bajtow);
		f_read (&plik, &Red, 1, &ile_bajtow);
		Set_color(Red, Green, Blue);
		Draw_pixel();
		
		a++;
		if(a == szerokosc_obrazka)
			{
				a=0;
				b=b+1;
			}
	}

	f_close (&plik);
}


void SD_rewind(void)
{
	fresult = f_opendir(&Dir, "0:/");
	fresult = f_opendir(&Dir, "0:/");
	fresult = f_readdir(&Dir, &PlikInfo);
	fresult = f_readdir(&Dir, &PlikInfo);
}

void bmp_forward()
{
	tryb = STOP;
	SSD1963_ClearScreen(white);
	fresult = f_readdir(&Dir, &PlikInfo);
	if(!PlikInfo.fname[0]) //gdy koniec katalogu
		{
			SD_rewind();
		}
	lcd_bmp(PlikInfo.fname);	
}

void bmp_rewind()
{
	SSD1963_ClearScreen(white);
	fresult = f_readdir(&Dir, &PlikInfo);
	
	if(!PlikInfo.fname[0])
		{
			SD_rewind();
		}
	lcd_bmp(PlikInfo.fname);	
	delay_ms(3000);
}

void bmp_select()
{
	tryb = STOP;
	SSD1963_ClearScreen(white);
	lcd_bmp(nazwa_pliku_bmp);	
	odeslij_potwierdzenie();
}

void clear_nazwa_pliku_bmp()
{
	int i=0;
	
	for(i=0; i< 20; i++)
		nazwa_pliku_bmp[i]=0;
}

void odczytaj_nazwy_plikow()
{
	char indeks=0;
	
	fresult = f_opendir(&Dir, "0:/");
	fresult = f_opendir(&Dir, "0:/");
	fresult = f_readdir(&Dir, &PlikInfo);
	fresult = f_readdir(&Dir, &PlikInfo);
	
	while(PlikInfo.fname[0])
	{	
		lista_plikow[indeks] = (char *) malloc(strlen(&PlikInfo.fname[0])+1); //rezerwuj miejsce na plik
		strcpy(lista_plikow[indeks],&PlikInfo.fname[0]);
		indeks++;
		fresult = f_readdir(&Dir, &PlikInfo);
	}
}

int search_for_file(char * looked_file)
{
	int wynik=1;
	
	fresult = f_opendir(&Dir, "0:/");
	fresult = f_opendir(&Dir, "0:/");
	fresult = f_readdir(&Dir, &PlikInfo);
	fresult = f_readdir(&Dir, &PlikInfo);
	
	while(PlikInfo.fname[0])
	{
		wynik = strcmp((char *)&PlikInfo.fname,looked_file);
		if(!wynik)
			break;
		fresult = f_readdir(&Dir, &PlikInfo);
	}
	
	return wynik;
}



