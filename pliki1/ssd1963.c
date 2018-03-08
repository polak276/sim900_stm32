//=============================================================================
#include "stm32f30x.h"
#include "ssd1963.h"
#include "delay_1.h"
#include "time.h"

//=============================================================================
// Write command to SSD1963
//=============================================================================
void SSD1963_WriteCommand(unsigned int commandToWrite)
{
	SSD1963_DATAPORT->ODR  = commandToWrite;
	SSD1963_CTRLPORT->BSRR = SSD1963_PIN_RD;
	SSD1963_CTRLPORT->BRR  = SSD1963_PIN_A0 | SSD1963_PIN_CS | SSD1963_PIN_WR;
	__ASM("nop");
	SSD1963_CTRLPORT->BSRR = SSD1963_PIN_A0 | SSD1963_PIN_CS | SSD1963_PIN_WR;
}
//=============================================================================
// Write data to SSD1963
//=============================================================================
void SSD1963_WriteData(unsigned int dataToWrite)
{
	SSD1963_DATAPORT->ODR  = dataToWrite;
	SSD1963_CTRLPORT->BSRR = SSD1963_PIN_RD | SSD1963_PIN_A0;
	SSD1963_CTRLPORT->BRR  = SSD1963_PIN_CS | SSD1963_PIN_WR;
	__ASM("nop");
	SSD1963_CTRLPORT->BSRR = SSD1963_PIN_CS | SSD1963_PIN_WR;
}
//=============================================================================
// 
//=============================================================================


void SSD1963_Init (void)
{
	volatile unsigned int dly;
	SSD1963_CTRLPORT->BRR = SSD1963_PIN_RST;
	for(dly = 0; dly < 1000; dly++);

	SSD1963_CTRLPORT->BSRR = SSD1963_PIN_RST;
	for(dly = 0; dly < 1000; dly++);

	SSD1963_WriteCommand(SSD1963_SOFT_RESET);
	SSD1963_WriteCommand(SSD1963_SET_PLL_MN);
	SSD1963_WriteData(49);	// PLLclk = REFclk * 50 (500MHz)
	SSD1963_WriteData(4);	// SYSclk = PLLclk / 5  (100MHz)
	SSD1963_WriteData(4);  // dummy
	SSD1963_WriteCommand(SSD1963_SET_PLL);
	SSD1963_WriteData(0x01);
	for(dly = 0; dly < 100000; dly++);

	SSD1963_WriteCommand(SSD1963_SET_PLL);
	SSD1963_WriteData(0x03);
	SSD1963_WriteCommand(SSD1963_SET_LCD_MODE); 
	SSD1963_WriteData(0x0C);			
	SSD1963_WriteData(0x00);			
	SSD1963_WriteData(mHIGH((TFT_WIDTH-1)));
	SSD1963_WriteData(mLOW((TFT_WIDTH-1)));
	SSD1963_WriteData(mHIGH((TFT_HEIGHT-1)));
	SSD1963_WriteData(mLOW((TFT_HEIGHT-1)));
	SSD1963_WriteData(0x00);
	SSD1963_WriteCommand(SSD1963_SET_PIXEL_DATA_INTERFACE);
	SSD1963_WriteData(SSD1963_PDI_16BIT565);
	SSD1963_WriteCommand(SSD1963_SET_LSHIFT_FREQ); 
	SSD1963_WriteData((LCD_FPR >> 16) & 0xFF);
	SSD1963_WriteData((LCD_FPR >> 8) & 0xFF);
	SSD1963_WriteData(LCD_FPR & 0xFF);
	SSD1963_WriteCommand(SSD1963_SET_HORI_PERIOD);
	SSD1963_WriteData(mHIGH(TFT_HSYNC_PERIOD));
	SSD1963_WriteData(mLOW(TFT_HSYNC_PERIOD));
	SSD1963_WriteData(mHIGH((TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH)));
	SSD1963_WriteData(mLOW((TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH)));
	SSD1963_WriteData(TFT_HSYNC_PULSE);
	SSD1963_WriteData(0x00);			
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x00);			
	SSD1963_WriteCommand(SSD1963_SET_VERT_PERIOD); 		
	SSD1963_WriteData(mHIGH(TFT_VSYNC_PERIOD));
	SSD1963_WriteData(mLOW(TFT_VSYNC_PERIOD));
	SSD1963_WriteData(mHIGH((TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH)));
	SSD1963_WriteData(mLOW((TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH)));
	SSD1963_WriteData(TFT_VSYNC_PULSE);
	SSD1963_WriteData(0x00);			
	SSD1963_WriteData(0x00);
	SSD1963_WriteCommand(SSD1963_SET_DISPLAY_ON);		//SET display on
}


//=============================================================================
//
//=============================================================================
void SSD1963_SetArea(unsigned int sx, unsigned int ex, unsigned int sy, unsigned int ey)
{
	SSD1963_WriteCommand(SSD1963_SET_COLUMN_ADDRESS);	
	SSD1963_WriteData((sx >> 8) & 0xFF);
	SSD1963_WriteData((sx >> 0) & 0xFF);
	SSD1963_WriteData((ex >> 8) & 0xFF);
	SSD1963_WriteData((ex >> 0) & 0xFF);

	SSD1963_WriteCommand(SSD1963_SET_PAGE_ADDRESS);	
	SSD1963_WriteData((sy >> 8) & 0xFF);
	SSD1963_WriteData((sy >> 0) & 0xFF);
	SSD1963_WriteData((ey >> 8) & 0xFF);
	SSD1963_WriteData((ey >> 0) & 0xFF);
}
//=============================================================================
// Fill area of specified color
//=============================================================================
void SSD1963_FillArea(unsigned int sx, unsigned int ex, unsigned int sy, unsigned int ey, int color)
{
	int i,j;
	SSD1963_SetArea(sx, ex, sy, ey);
	SSD1963_WriteCommand(SSD1963_WRITE_MEMORY_START);
	for(i = 0; i < TFT_WIDTH; i++)
		{
			for(j=0;j<TFT_HEIGHT;j++)
			{
				SSD1963_WriteData(color>>16);
				SSD1963_WriteData(color>>8);
				SSD1963_WriteData(color);
			}
		}
}
//=============================================================================
// Fills whole screen specified color
//=============================================================================

void SSD1963_ClearScreen(unsigned long color)
{
	unsigned int x,y;

	SSD1963_SetArea(0, TFT_WIDTH-1 , 0, TFT_HEIGHT-1);
	SSD1963_WriteCommand(0x2c);
		
	for(x = 0; x < TFT_WIDTH; x++)
		{
			for(y=0;y<TFT_HEIGHT;y++)
			{
				SSD1963_WriteData(color>>16);
				SSD1963_WriteData(color>>8);
				SSD1963_WriteData(color);
			}
		}
}

//=============================================================================
//
//=============================================================================
void GLCD_SetPixel(int x, int y, int color)
{
	SSD1963_SetArea(x, x, y, y);
	SSD1963_WriteCommand(0x2c);
	SSD1963_WriteData(color);
	SSD1963_WriteCommand(0x0);
}
//=============================================================================
//
//=============================================================================

void tft_init(void) {

	uint32_t i=0;

	SSD1963_DATAPORT -> BSRR = 0xffff;
	SSD1963_CTRLPORT -> BSRR = 0xffff;
	
	delay_ms(100);
	for(i=0;i<5;i++)
		{
			SSD1963_CTRLPORT->BRR = SSD1963_PIN_RST;
			delay_ms(10);
			SSD1963_CTRLPORT->BSRR = SSD1963_PIN_RST;
			delay_ms(10);
		}

	SSD1963_WriteCommand(0x01);
	delay_ms(10);
	SSD1963_WriteCommand(0x01);
	delay_ms(10);
	SSD1963_WriteCommand(0x01);
	delay_ms(10);

	SSD1963_WriteCommand(0xe0);
	SSD1963_WriteData(0x01);
	delay_ms(10);
	SSD1963_WriteCommand(0xe0);
	SSD1963_WriteData(0x03);
	delay_ms(10);
	
	SSD1963_WriteCommand(0xb0);
	SSD1963_WriteData(0x08);
	SSD1963_WriteData(0x80);
	SSD1963_WriteData(0x01);
	SSD1963_WriteData(0xdf);
	SSD1963_WriteData(0x01);
	SSD1963_WriteData(0x0f);
	SSD1963_WriteData(0x00);

	SSD1963_WriteCommand(0xf0);
	SSD1963_WriteData(0x00);
	
	SSD1963_WriteCommand(0xe2);
	SSD1963_WriteData(0x22);
	SSD1963_WriteData(0x03);
	SSD1963_WriteData(0x04);
	
	SSD1963_WriteCommand(0xe6);
	SSD1963_WriteData(0x01);
	SSD1963_WriteData(0x45);
	SSD1963_WriteData(0x47);
	
	SSD1963_WriteCommand(0xb4);
	SSD1963_WriteData(0x02);
	SSD1963_WriteData(0x0d);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x2b);
	SSD1963_WriteData(0x28);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x00);
	
	SSD1963_WriteCommand(0xb6);
	SSD1963_WriteData(0x01);
	SSD1963_WriteData(0x1d);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x0c);
	SSD1963_WriteData(0x09);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x00);
	
	SSD1963_WriteCommand(0x2a);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x01);
	SSD1963_WriteData(0xdf);

	SSD1963_WriteCommand(0x2b);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x01);
	SSD1963_WriteData(0x0f);

	SSD1963_WriteCommand(0x29);
	
	SSD1963_WriteCommand(0x2c);
}





