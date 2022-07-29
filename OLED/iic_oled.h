/*
CFP
2022 - 
*/

#ifndef _IIC_OLED_H_
#define _IIC_OLED_H_

#include "board_cfg.h"
//#include "sys.h"
//#include "stdlib.h"	

//-----------------测试LED端口定义---------------- 


//-----------------OLED端口定义---------------- 
//#ifdef OLED_USE_IIC
#define OLED_SCLK_Clr() I2C_SCL_GPIO_PORT->BRR = I2C_SCL_GPIO_PIN
#define OLED_SCLK_Set() I2C_SCL_GPIO_PORT->BSRR = I2C_SCL_GPIO_PIN

#define OLED_SDIN_Clr() I2C_SDA_GPIO_PORT->BRR = I2C_SDA_GPIO_PIN
#define OLED_SDIN_Set() I2C_SDA_GPIO_PORT->BSRR = I2C_SDA_GPIO_PIN

#define IIC_OLED_RST_Clr() IIC_RES_GPIO_PORT->BRR = IIC_RES_GPIO_PIN
#define IIC_OLED_RST_Set() IIC_RES_GPIO_PORT->BSRR = IIC_RES_GPIO_PIN

#define SCL_H         I2C_SCL_GPIO_PORT->BSRR = I2C_SCL_GPIO_PIN
#define SCL_L         I2C_SCL_GPIO_PORT->BRR = I2C_SCL_GPIO_PIN
   
#define SDA_H         I2C_SDA_GPIO_PORT->BSRR = I2C_SDA_GPIO_PIN
#define SDA_L         I2C_SDA_GPIO_PORT->BRR = I2C_SDA_GPIO_PIN

#define SDA_read  GPIO_ReadInputDataBit(I2C_SDA_GPIO_PORT , I2C_SDA_GPIO_PIN)

//IO方向设置
//PB07
#define SDA_IN()  {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=(u32)8<<(4*7);}
#define SDA_OUT() {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=(u32)3<<(4*7);}

//PB11   (11-8) = 3
#define SDAx_IN()  {GPIOB->CRH&=0xFFFF0FFF;GPIOB->CRH|=(u32)8<<(4*(11-8));}
#define SDAx_OUT() {GPIOB->CRH&=0xFFFF0FFF;GPIOB->CRH|=(u32)3<<(4*(11-8));}

//IO操作函数	 
//#define IIC_SCL    PBout(6) //SCL
//#define IIC_SDA    PBout(7) //SDA	 
//#define READ_SDA   PBin(7)  //输入SDA 

//#endif

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
//#define u8 unsigned char
//#define u32 unsigned int

#ifdef OLED_0P96_BW_PANEL
//屏幕分辨率
#define IIC_X_MAX_PIXEL	        128
#define IIC_Y_MAX_PIXEL	        64
#endif

void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_Clear_Buff(void);
void OLED_DrawPoint(u8 x,u8 y);
void OLED_DrawSegment(u8 x,u8 y,u8 segs);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1);
void OLED_ShowChar_8x16(u8 x,u8 y,u8 chr);
void OLED_ShowCharR_8x16(u8 x,u8 y,u8 chr);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1);
void OLED_ShowPF_16x16(u8 x,u8 y,u8 num);


void OLED_WR_BP(u8 x,u8 y);
void OLED_ShowPicture(u8 x0,u8 y0,u8 x1,u8 y1,u8 *BMP);
void IIC_OLED_Init(void);
void OLED_ShowTest(u8 pattern);

#endif

