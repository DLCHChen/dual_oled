/*
*/

#ifndef __SPI_OLED_H__
#define __SPI_OLED_H__

//PA05 - TFT --SCL/SCK
//PA07 - TFT --SDA/DIN
//PA04 - TFT --CS
//PA06 - TFT --Dout

//PB01 - TFT --RST/RES
//PA03 - TFT --RS/DC
//PB00 - TFT --BLK/BL

//#ifdef OLED_USE_SPI
#define OLED_SPI_SCLK_Clr() SPI_CLK_GPIO_PORT->BRR = SPI_CLK_GPIO_PIN
#define OLED_SPI_SCLK_Set() SPI_CLK_GPIO_PORT->BSRR = SPI_CLK_GPIO_PIN

#define OLED_SPI_SDIN_Clr() SPI_DATA_GPIO_PORT->BRR = SPI_DATA_GPIO_PIN
#define OLED_SPI_SDIN_Set() SPI_DATA_GPIO_PORT->BSRR = SPI_DATA_GPIO_PIN

#define OLED_SPI_RST_Clr() SPI_RES_GPIO_PORT->BRR = SPI_RES_GPIO_PIN
#define OLED_SPI_RST_Set() SPI_RES_GPIO_PORT->BSRR = SPI_RES_GPIO_PIN

//#endif

#define OLED_SPI_DC_Clr() SPI_DC_GPIO_PORT->BRR = SPI_DC_GPIO_PIN
#define OLED_SPI_DC_Set() SPI_DC_GPIO_PORT->BSRR = SPI_DC_GPIO_PIN
 		     
#define OLED_SPI_CS_Clr()  SPI_CS_GPIO_PORT->BRR = SPI_CS_GPIO_PIN
#define OLED_SPI_CS_Set()  SPI_CS_GPIO_PORT->BSRR = SPI_CS_GPIO_PIN

#define  SPI_NSS_LOW() SPI_CS_GPIO_PORT->BRR = SPI_CS_GPIO_PIN
#define  SPI_NSS_HIGH() SPI_CS_GPIO_PORT->BSRR = SPI_CS_GPIO_PIN

#define OLED_BLK_Clr() SPI_BLK_GPIO_PORT->BRR = SPI_BLK_GPIO_PIN
#define OLED_BLK_Set() SPI_BLK_GPIO_PORT->BSRR = SPI_BLK_GPIO_PIN

#ifdef OLED_1P3_COLOR_PANEL
//128x96
// default orientation
#define SSD1351_WIDTH  128
#define SSD1351_HEIGHT 128

#define SPI_X_MAX_PIXEL	        128
#define SPI_Y_MAX_PIXEL	        96

#endif

#ifdef OLED_0P96_COLOR_PANEL
//屏幕分辨率
#define SPI_X_MAX_PIXEL	        160
#define SPI_Y_MAX_PIXEL	        80
#endif

/* RGB565 - format */
#define		RGB565_Black		0x0000		//黑色（不亮）
#define		RGB565_Navy			0x000F		//深蓝色
#define		RGB565_Dgreen		0x03E0		//深绿色
#define		RGB565_Dcyan		0x03EF		//深青色
#define		RGB565_Maroon		0x7800		//深红色
#define		RGB565_Purple		0x780F		//紫色
#define		RGB565_Olive		0x7BE0		//橄榄绿
#define		RGB565_Lgray		0xC618		//灰白色
#define		RGB565_Dgray		0x7BEF		//深灰色
#define		RGB565_Blue			0x001F		//蓝色
#define		RGB565_Green		0x07E0		//绿色
#define		RGB565_Cyan			0x07FF		//青色
#define		RGB565_Red			0xF800		//红色
#define		RGB565_Magenta		0xF81F		//品红
#define		RGB565_Yellow		0xFFE0		//黄色
#define		RGB565_unkonwn		0xF81F		//----
#define		RGB565_White		0xffff		//白色
#define		RGB565_Grey         0xF7DE
#define		RGB565_Blue2        0x051F
#define		RGB565_PeacockGreen 0x050B
#define		RGB565_PowderBlue   0x071C ///0xB71C
#define		RGB565_PeacockBlue  0x0411
#define		RGB565_Golden       0xFEA0


#ifdef OLED_1P3_COLOR_PANEL
void SSD1351_DrawPixel(uint8_t x, uint8_t y, uint16_t color);
void SSD1351_DrawVertical_Column(uint8_t x, uint8_t y, uint8_t len,uint16_t color);
void SSD1351_FillRectangleX(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void SSD1351_DrawHorizontal_Row(uint8_t x, uint8_t y, uint8_t w,uint16_t color);
void SSD1351_FillRectangleX(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void SSD1351_WriteChar_16x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor);
void SSD1351_WriteChar_R16x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor);
void SSD1351_WriteChar_8x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor);
void SSD1351_WriteChar_R8x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor);
void SSD1351_FillRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void SSD1351_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2, u16 color);

#endif

#ifdef OLED_0P96_COLOR_PANEL
void ST7735_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void ST7735_DrawVertical_Column(uint8_t x, uint8_t y, uint16_t len,uint16_t color);
void ST7735_DrawHorizontal_Row(uint8_t x, uint8_t y, uint8_t w,uint16_t color);
void ST7735_WriteChar_16x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor);
void ST7735_WriteChar_8x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor);
void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7735_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2, u16 color);
void ST7735_FillDraw_Test(uint8_t x, uint8_t y, uint16_t len,uint16_t color);

#endif

void Spi_OLED_Init(void);

#endif

