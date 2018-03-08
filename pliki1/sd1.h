#ifndef _SD1_H
#define _SD1_H

#define SD_PORT GPIOA
#define POWER_PIN GPIO_Pin_11


void sd_power_on(void);
void lcd_bmp(char * nazwa_pliku);
void SD_rewind(void);
void bmp_forward(void);
void bmp_rewind(void);
void bmp_select(void);
void clear_nazwa_pliku_bmp(void);
void odczytaj_nazwy_plikow(void);
int search_for_file(char * looked_file);

#endif
