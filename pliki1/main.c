#include <string.h>
#include "stm32f30x.h"
#include "ssd1963.h"
#include "colors.h"
#include "delay_1.h"
#include "ff.h"
#include "diskio.h"
#include "config.h"
#include "sd1.h"
#include "uart.h"

static FATFS g_sFatFS;
FIL plik;
FRESULT fresult;
UINT zapisanych_bajtow; 
FILINFO PlikInfo;
DIR Dir;
char indeks = 0;
volatile char licznik=0;
char bufor[BUFFER_SIZE];
volatile char * komenda=0;
volatile char sms_received=0;
volatile char ask_for_names=0;
volatile char nr_smsa=0;
char tmp_komenda[40];
extern volatile char new_message;
volatile tryb_wyswietlacza tryb=STOP;
volatile tryb_wyswietlacza last_tryb;
char nazwy_plikow[255];
char nazwa_pliku_bmp[20];
char * lista_plikow[30];//tablica 30 wskaznikow na chary zawierajaca liste wszystkich plikow na karcie

//gotowe bufory z komendami AT
char AT_Wyslij_sms[] = "AT+CMGS=\x22+48796657106\x22\x0D";
char AT_AT[] = "AT\x0D";
char AT_Usun_all[] = "AT+CMGDA=\x22\x44\x45L ALL\x22\x0D";
char AT_Odbierz_sms[] = "AT+CMGR=1"; 
char odebrano_sms[] = "+CMTI: \x22SM\x22,";
char odebrano_sms_1[] = "+CMTI";
char get_sms[15] = "AT+CMGR=";

//obslugiwane komendy
char next[] = "Zmien";
char rewind[] = "Przew";
char stop[] = "Stop1";
char names[] = "Nazwy";
char wybor[] = "Wybie";

//gotowe odpowiedzi
char ustawiono_zdjecie[40]="Pomyslnie ustawiono zdjecie "; //na 28 wpisujemy nazwe ustawionego zdjecia
char brak_pliku[40]="Brak na karcie pliku "; //na 21 wpisujemy nazwe zdjecia


int main (void)
{
	
	RCC_Config();//konfiguracja systemu
	GPIO_Config();
	NVIC_Config();
	SPI_Config();
	tft_init();
	GPIO_WriteBit(GPIOE, GPIO_Pin_13, (BitAction)1);	//SIM900 on
	delay_ms(4000);
	GPIO_WriteBit(GPIOE, GPIO_Pin_13, (BitAction)0);
	delay_ms(6000);
	USART_Config();
	SSD1963_ClearScreen(black);
	GPIO_WriteBit(GPIOE, GPIO_Pin_1, Bit_RESET); //sd on
	
	if(SysTick_Config_Mod(SysTick_CLKSource_HCLK_Div8, 90000ul)) //systick 10ms
		{
			while(1);
		}
 
	f_mount(0, &g_sFatFS); //sd
	odczytaj_nazwy_plikow();
	
	while(1)
	{
		f_opendir(&Dir, "0:/"); 
		f_opendir(&Dir, "0:/");
		f_readdir(&Dir, &PlikInfo);//otwiera katalog glowny

		send_command_to_sim900(&AT_Usun_all[0], 1000);		//usun wszystkie stare smsy
		
		while(1)
		{
			delay_ms(300);

			if( sms_received )
				{
					sms_received=0;
					get_message();
				}
			
			if(ask_for_names)
				{
					ask_for_names=0;
					GPIO_WriteBit(GPIOE, GPIO_Pin_14, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_14)));
					send_names();
				}
		
			switch(tryb)
			{
				case DO_PRZODU:
					bmp_forward();
					break;
			
				case PRZEWIJAJ:
					bmp_rewind();
					break;
				
				case KONKRETNA:
					if(!(search_for_file(&nazwa_pliku_bmp[0]))) //jesli plik istnieje
						bmp_select();
					else
						odeslij_blad();
					
					break;
				
				default:
					break;
			}
			
		}
	}
}



