
#ifndef _DRAWING1_H
#define _DRAWING1_H

#include "stm32f30x.h"

void Set_color(uint8_t R, uint8_t G, uint8_t B);
void Set_bk_color(uint8_t R, uint8_t G, uint8_t B);
void Set_color32(uint32_t color);
void Set_bk_color32(uint32_t color);
void put_pixel(int x, int y);
void put_bk_pixel(int x, int y);
void Draw_pixel(void);
void Draw_bk_pixel(void);

#endif
