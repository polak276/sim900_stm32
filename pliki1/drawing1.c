#include "stm32f30x.h"
#include "drawing1.h"
#include "ssd1963.h"

uint8_t Red, Green, Blue;
uint8_t Bk_red, Bk_green, Bk_blue;

void Set_color(uint8_t R, uint8_t G, uint8_t B)
{
	Red = R & 0xfe;
	Green = G & 0xfe;
	Blue = B & 0xfe;
}

void Set_bk_color(uint8_t R, uint8_t G, uint8_t B)
{
	Bk_red = R & 0xfe;
	Bk_green = G & 0xfe;
	Bk_blue = B & 0xfe;
}

void Set_color32(uint32_t color)
{
	Red = (color>>16) & 0xfe;
	Green = (color>>8) & 0xfe;
	Blue = (color) & 0xfe;
}

void Set_bk_color32(uint32_t color)
{
	Bk_red = (color>>16) & 0xfe;
	Bk_green = (color>>8) & 0xfe;
	Bk_blue = (color) & 0xfe;
}

void put_pixel(int x, int y)
{
	SSD1963_SetArea(x, x, y, y);
	SSD1963_WriteCommand(0x2c);
	Draw_pixel();
}

void put_bk_pixel(int x, int y)
{
	SSD1963_SetArea(x, y, x, y);
	SSD1963_WriteCommand(0x2c);
	Draw_bk_pixel();
}

void Draw_pixel(void)
{
	SSD1963_WriteData(Red);
	SSD1963_WriteData(Green);
	SSD1963_WriteData(Blue);
}

void Draw_bk_pixel(void)
{
	SSD1963_WriteData(Bk_red);
	SSD1963_WriteData(Bk_green);
	SSD1963_WriteData(Bk_blue);
}

