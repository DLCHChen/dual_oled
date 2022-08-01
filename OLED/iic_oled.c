/*
CFP
2022 - 
*/

#include "board_cfg.h"
#include "iic_oled.h"
#include "spi_oled.h"

//#include "stdlib.h"
#include "oledfont.h"
#include "debug.h"


/*
0.96 oled - SSD1306 -- 128x64 (each dot has a bit)
1.3  oled(color) - SSD1351 -- 128x96 (each dot has 16bit RGB565 data)
*/

//u8 OLED_GRAM[144][8];
//u8 OLED_GRAM[132][8];

u8 OLED_GRAM[128*8];

const uint8_t Spec_string[] = 
{
	0x10,0x40,0x10,0x40,0x10,0xFC,0x10,0x88,0x55,0x50,0x54,0x20,0x54,0xD8,0x57,0x26,
	0x54,0xF8,0x54,0x20,0x54,0xF8,0x5C,0x20,0x67,0xFE,0x00,0x20,0x00,0x20,0x00,0x20,/*"峰",726*/
	0x08,0x40,0x08,0x40,0x0F,0xFC,0x10,0x40,0x10,0x40,0x33,0xF8,0x32,0x08,0x53,0xF8,
	0x92,0x08,0x13,0xF8,0x12,0x08,0x13,0xF8,0x12,0x08,0x12,0x08,0x1F,0xFE,0x10,0x00,/*"值",3592*/
	0x02,0x00,0x01,0x00,0x7F,0xFC,0x02,0x00,0x44,0x44,0x2F,0x88,0x11,0x10,0x22,0x48,
	0x4F,0xE4,0x00,0x20,0x01,0x00,0xFF,0xFE,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,/*"率",1733*/
	0x10,0x00,0x11,0xFE,0x50,0x20,0x5C,0x40,0x51,0xFC,0x51,0x04,0xFF,0x24,0x01,0x24,
	0x11,0x24,0x55,0x24,0x55,0x24,0x55,0x44,0x84,0x50,0x08,0x88,0x31,0x04,0xC2,0x02,/*"频",2088*/
	0x02,0x00,0x01,0x00,0x3F,0xF8,0x00,0x00,0x08,0x20,0x04,0x40,0xFF,0xFE,0x00,0x00,
	0x1F,0xF0,0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10,0x10,0x10,0x1F,0xF0,0x10,0x10,/*"音",3279*/
	0x08,0x40,0x08,0x40,0x13,0xF8,0x20,0x48,0x4F,0xFE,0x08,0x48,0x13,0xF8,0x30,0x40,
	0x50,0x40,0x93,0xF8,0x10,0x40,0x10,0x40,0x17,0xFC,0x10,0x40,0x10,0x40,0x10,0x40,/*"律",1732*/
	0x00,0x20,0x44,0x20,0x2B,0xFE,0x10,0x20,0x29,0xFC,0x48,0x20,0x8B,0xFE,0x08,0x00,
	0x19,0xFC,0x29,0x04,0x49,0xFC,0x89,0x04,0x09,0xFC,0x09,0x04,0x51,0x14,0x21,0x08,/*"猜",221*/
	0x02,0x00,0x02,0x00,0x02,0x00,0x03,0xFC,0x02,0x00,0x02,0x00,0x3F,0xF0,0x20,0x10,
	0x20,0x10,0x20,0x10,0x3F,0xF0,0x00,0x00,0x24,0x88,0x22,0x44,0x42,0x44,0x80,0x04,/*"点",536*/
	0x00,0x40,0x00,0x40,0x7C,0x40,0x00,0x40,0x01,0xFC,0x00,0x44,0xFE,0x44,0x20,0x44,
	0x20,0x44,0x20,0x84,0x48,0x84,0x44,0x84,0xFD,0x04,0x45,0x04,0x02,0x28,0x04,0x10,/*"动",578*/
	0x08,0x40,0x08,0x20,0x0B,0xFE,0x10,0x00,0x10,0x00,0x31,0xFC,0x30,0x00,0x50,0x00,
	0x91,0xFC,0x10,0x00,0x10,0x00,0x11,0xFC,0x11,0x04,0x11,0x04,0x11,0xFC,0x11,0x04,/*"信",3044*/
	0x00,0x00,0x1F,0xF0,0x10,0x10,0x10,0x10,0x10,0x10,0x1F,0xF0,0x00,0x00,0xFF,0xFE,
	0x08,0x00,0x10,0x00,0x1F,0xF0,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0xA0,0x00,0x40,/*"号",976*/
	0x3F,0xF8,0x01,0x00,0x7F,0xFE,0x41,0x02,0x9D,0x74,0x01,0x00,0x1D,0x70,0x04,0x00,
	0x0F,0xE0,0x14,0x40,0x03,0x80,0x1C,0x70,0xE2,0x0E,0x0F,0xE0,0x04,0x20,0x18,0x60,/*"雾",2896*/
	0x10,0x20,0x10,0x20,0x10,0x50,0x10,0x88,0xFD,0x04,0x12,0x02,0x11,0xFC,0x14,0x00,
	0x18,0x00,0x31,0xFC,0xD1,0x04,0x11,0x04,0x11,0x04,0x11,0x04,0x51,0xFC,0x21,0x04,/*"拾",2459*/
	0x00,0x00,0x3F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x02,0x00,
	0x04,0x00,0x04,0x00,0x08,0x40,0x10,0x20,0x20,0x10,0x7F,0xF8,0x20,0x08,0x00,0x08,/*"云",3421*/
};

const uint8_t peak_freq_string[] = 
{
	0x00,0x01,0xF2,0x04,0x08,0xE0,0x18,0x04,0xF2,0x01,0x78,0x04,0xF2,0x02,0x71,0x09,
	0x00,0x40,0x4F,0x48,0x48,0x6B,0x58,0x48,0x4F,0x02,0x12,0x12,0xFE,0x02,0x3E,0x02,/*"频",2088*/
	0x00,0x10,0x90,0x10,0x10,0xD0,0x90,0x90,0xBF,0x90,0x90,0x90,0x10,0x10,0x90,0x10,
	0x00,0x00,0x28,0x25,0x22,0x20,0x29,0x24,0x66,0xB5,0x2C,0x24,0x22,0x25,0x28,0x00,/*"率",1733*/
	0x00,0x08,0x08,0xA8,0xA8,0xFF,0xA8,0xA8,0x08,0x08,0xF8,0x10,0xF0,0x08,0xF8,0x00,
	0x00,0x01,0x21,0x32,0x2A,0x25,0xEA,0x32,0x09,0x01,0x0F,0x00,0xFF,0x00,0x0F,0x00,/*"峰",726*/
	0x00,0x02,0x02,0xFE,0x52,0x52,0x52,0x52,0x52,0xFE,0x02,0x02,0xFF,0x00,0x00,0x80,
	0x00,0x00,0x20,0x27,0x25,0x25,0xFD,0x25,0x25,0x27,0x20,0xE0,0x1F,0x06,0x01,0x00,/*"值",3592*/

	0x00,0x00,0x07,0x08,0xE0,0x20,0x26,0x28,0x20,0x26,0x28,0x20,0x20,0xEC,0x02,0x01,
	0x00,0x00,0x10,0x10,0x13,0x12,0x12,0x12,0x12,0xFE,0x02,0x02,0x02,0x03,0x00,0x00,/*"点",536*/
	0x00,0x00,0xFE,0xA9,0xAA,0xA8,0xA8,0xA8,0xFF,0x00,0x00,0xFC,0x82,0x41,0x22,0x10,
	0x00,0x22,0x2A,0x2A,0x2A,0xFE,0x2A,0x2A,0x2A,0x22,0x40,0x2F,0x10,0x28,0x44,0x02,/*"猜",221*/
	0x00,0x00,0x08,0x48,0x48,0x48,0xFF,0x48,0x48,0x48,0x08,0x00,0xFF,0x00,0x80,0x40,
	0x00,0x08,0x08,0x3E,0x2A,0x2A,0xFF,0x2A,0x2A,0x2A,0x08,0xCC,0x23,0x11,0x08,0x00,/*"律",1732*/
	0x00,0x00,0x00,0x00,0xFF,0x92,0x92,0x92,0x92,0x92,0x92,0x92,0xFF,0x00,0x00,0x00,
	0x00,0x02,0x02,0x22,0x22,0x2A,0x26,0x22,0x62,0xA2,0x26,0x2A,0x22,0x22,0x02,0x02,/*"音",3279*/

	0x00,0x00,0x00,0x00,0x3C,0x22,0x21,0x22,0x20,0x20,0x20,0xA0,0x60,0x00,0x00,0x00,
	0x00,0x01,0x01,0x01,0x7D,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x7D,0x01,0x01,0x01,/*"号",976*/
	0x00,0x00,0x9F,0x92,0x92,0x92,0x92,0x92,0x9F,0x00,0x00,0x00,0xFF,0x00,0x00,0x80,
	0x00,0x20,0x24,0x24,0x24,0x64,0xA4,0x24,0x24,0x20,0x00,0xE0,0x1F,0x06,0x01,0x00,/*"信",3044*/
	0x00,0x00,0xFC,0x02,0x01,0x02,0x80,0x70,0x0C,0x02,0x1D,0x28,0x08,0xC8,0x3C,0x08,
	0x00,0x00,0x0F,0x08,0x08,0x08,0xFF,0x08,0x08,0x02,0x22,0x22,0x22,0x23,0x22,0x02,/*"动",578*/
	0x00,0x08,0x08,0x08,0x10,0x97,0xD5,0xA4,0xA4,0xAC,0xD6,0x95,0x51,0x08,0x08,0x08,
	0x00,0x30,0x28,0xA0,0xAA,0xAA,0xAA,0xA0,0xFE,0xA0,0xAB,0xAA,0xAA,0xA0,0x30,0x08,/*"雾",2896*/
	0x00,0x00,0x7F,0x42,0x42,0x42,0x42,0x42,0x7F,0x00,0x00,0x80,0xFE,0x41,0x22,0x20,
	0x00,0x04,0x0A,0x12,0x22,0xC2,0x22,0x12,0x0A,0x04,0x09,0x08,0xFF,0x08,0x08,0x08,/*"拾",2459*/
	0x00,0x00,0x00,0x07,0x0C,0x14,0x24,0x04,0x04,0x04,0xC4,0x24,0x14,0x0E,0x04,0x00,
	0x00,0x02,0x02,0x42,0x42,0x42,0x42,0x42,0x42,0x43,0x42,0x42,0x42,0x42,0x02,0x02,/*"云",3421*/
};

#ifdef OLED_USE_IIC
#ifdef OLED_USE_HW_IIC
uint8_t u8Senddata[4];
//uint8_t u8Recdata[4];
uint8_t u8DevAddr = 0x78;

#define IIC_TIME_OUT_P 5000
#define IIC_TIME_DELAY_CNT 4

void Drv_I2C_Init(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);
	
	/* Configure I2C2 pins: PB10->SCL and PB11->SDA */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		 
	I2C_DeInit(I2C2);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;   // = 400KHz
	I2C_InitStructure.I2C_OwnAddress1 = 0x36;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;// should be less than 400Khz    I2C_SPEED;
	
	I2C_Init(I2C2, &I2C_InitStructure);
	I2C_Cmd(I2C2, ENABLE);
	/*允许1字节1应答模式*/
	I2C_AcknowledgeConfig(I2C2, ENABLE);
}

static void I2C_delay_Ex(uint16_t cnt)
{
	while(cnt--);
}

/*******************************************************************************
* Function Name  : I2C_WriteOneByte
* Description    : 通过指定I2C接口写入一个字节数据
* Input          : - I2Cx:I2C寄存器基址
*                  - I2C_Addr:从器件地址
*                  - addr:预写入字节地址
*                  - value:写入数据
* Output         : None
* Return         : 成功返回0
* Attention		 : None
*******************************************************************************/
uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t value)
{
	int timeout;
    /* 起始位 */
  	I2C_GenerateSTART(I2Cx, ENABLE);
	timeout = 0;
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		I2C_delay_Ex(IIC_TIME_DELAY_CNT);
		timeout++;
		if(timeout > IIC_TIME_OUT_P)
			return 1;
	}

#if 1
  	/* 发送器件地址(写)*/
  	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		I2C_delay_Ex(IIC_TIME_DELAY_CNT);
		timeout++;
		if(timeout > IIC_TIME_OUT_P)
			return 1;
  	}
  
  	/*发送地址*/
  	I2C_SendData(I2Cx, addr);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		I2C_delay_Ex(IIC_TIME_DELAY_CNT);
		timeout++;
		if(timeout > IIC_TIME_OUT_P)
			return 1;
	}
#else	
	I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Transmitter);
 	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
#endif

	/* 写一个字节*/
  	I2C_SendData(I2Cx, value); 
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		I2C_delay_Ex(IIC_TIME_DELAY_CNT);
		timeout++;
		if(timeout > IIC_TIME_OUT_P)
			return 1;
  	}
	
  	/* 停止位*/
  	I2C_GenerateSTOP(I2Cx, ENABLE);
#ifdef IIC_W_POLL_ACK
  	I2C_AcknowledgePolling(I2Cx,I2C_Addr);
	I2C_delay_Ex(100);
#endif
	return 0;
}

uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
	int timeout;
	uint8_t val;
	if(num==0)
		return 1;
    /* 起始位 */
  	I2C_GenerateSTART(I2Cx, ENABLE);
	timeout = 0;
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		I2C_delay_Ex(IIC_TIME_DELAY_CNT);
		timeout++;
		if(timeout > IIC_TIME_OUT_P)
			return 1;
	}
#if 1
  	/* 发送器件地址(写)*/
  	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		I2C_delay_Ex(IIC_TIME_DELAY_CNT);
		timeout++;
		if(timeout > IIC_TIME_OUT_P)
			return 1;
  	}
  
  	/*发送地址*/
  	I2C_SendData(I2Cx, addr);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		I2C_delay_Ex(IIC_TIME_DELAY_CNT);
		timeout++;
		if(timeout > IIC_TIME_OUT_P)
			return 1;
	}
#else	
	I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Transmitter);
 	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
#endif

	while(num)
	{
		/* 写一个字节*/
		val = *buf;
	  	I2C_SendData(I2Cx, val); 
	  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			I2C_delay_Ex(IIC_TIME_DELAY_CNT);
			timeout++;
			if(timeout > IIC_TIME_OUT_P)
				return 1;
	  	}
		buf++;
		num--;
	}
	
  	/* 停止位*/
  	I2C_GenerateSTOP(I2Cx, ENABLE);
#ifdef IIC_W_POLL_ACK
  	I2C_AcknowledgePolling(I2Cx,I2C_Addr);
	I2C_delay_Ex(100);
#endif
	return 0;
}

void OLED_WR_Byte(u8 dat,u8 mode)
{
	u8 RegAddr;
	if(mode)
	{
		RegAddr = 0x40;
	}
	else
	{
		RegAddr = 0x00;
	}
	//u8Senddata[0] = dat;
	//u8Senddata[1] = 0xa5;
	I2C_WriteOneByte(I2C2,u8DevAddr,RegAddr,dat);
}

void OLED_WR_Bytes(u8 *pdat,u8 mode)
{
	u8 RegAddr;
	u8 *p2send_data = pdat;
	if(mode)
	{
		RegAddr = 0x40;
	}
	else
	{
		RegAddr = 0x00;
	}
	I2C_Write(I2C2,u8DevAddr,RegAddr,p2send_data,128);
}

#else

//#define OP_0O    1
#define OP_2O    1

#ifdef OP_0O
#ifdef CPU_FREQ_48MHZ
//#define I2C_delay() {__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");}
#define I2C_delay() {__ASM("nop");__ASM("nop");__ASM("nop");}
#endif

#ifdef CPU_FREQ_56MHZ
//#define I2C_delay() {__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");}
#define I2C_delay() {__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");}
#endif

#ifdef CPU_FREQ_72MHZ
//#define I2C_delay() {__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");}
#define I2C_delay() {__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");}
#endif
#endif

#ifdef OP_2O
#ifdef CPU_FREQ_48MHZ
//#define I2C_delay() {__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");}
#define I2C_delay() {__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");}
#endif

#ifdef CPU_FREQ_56MHZ
//#define I2C_delay() {__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");}
#define I2C_delay() {__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");}
#endif

#ifdef CPU_FREQ_72MHZ
//#define I2C_delay() {__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");}
#define I2C_delay() {__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");__ASM("nop");}
#endif
#endif

//起始信号
void OLED_I2C_Start(void)
{
	OLED_SDIN_Set();
	I2C_delay();
	OLED_SCLK_Set();
	I2C_delay();
	OLED_SDIN_Clr();
	I2C_delay();
	OLED_SCLK_Clr();
	//I2C_delay();
}

void OLED_I2C_Stop(void)
{
	//OLED_SDIN_Clr();
	//I2C_delay();
	OLED_SCLK_Set();
	I2C_delay();
	//OLED_SDIN_Clr();
	//I2C_delay();
	OLED_SDIN_Set();
	I2C_delay();
}

//等待信号响应
void OLED_I2C_WaitAck(void) //测数据信号的电平
{
	//OLED_SCLK_Set();  //???
	I2C_delay();
	OLED_SCLK_Set();
	I2C_delay();
	OLED_SCLK_Clr();
	I2C_delay();
}

//写入一个字节
void OLED_Send_Byte(u8 dat)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		//OLED_SCLK_Clr();//将时钟信号设置为低电平
		//#ifdef CPU_FREQ_48MHZ
		//I2C_delay();
		//#endif
		if(dat&0x80)//将dat的8位从最高位依次写入
		{
			OLED_SDIN_Set();
    	}
		else
		{
			OLED_SDIN_Clr();
		}
		#ifdef OP_2O
		I2C_delay();
		#endif
		//__ASM("nop");
		//__ASM("nop");
		//__ASM("nop");
		//__ASM("nop");
		//__ASM("nop");
		//__ASM("nop");
		//__ASM("nop");
		//__ASM("nop");
		//__ASM("nop");
		OLED_SCLK_Set();//将时钟信号设置为高电平
		I2C_delay();
		//I2C_delay();
		dat<<=1;
		OLED_SCLK_Clr();//将时钟信号设置为低电平
	}
}

//发送一个字节
//向SSD1306写入一个字节。
//mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 mode)
{
	OLED_I2C_Start();
	OLED_Send_Byte(0x78);
	OLED_I2C_WaitAck();
	if(mode)
	{
		OLED_Send_Byte(0x40);
	}
	else
	{
		OLED_Send_Byte(0x00);
	}
	OLED_I2C_WaitAck();
	OLED_Send_Byte(dat);
	OLED_I2C_WaitAck();
	OLED_I2C_Stop();
}

void OLED_WR_Bytes(u8* pdat,u8 mode)
{
	u8 xi;
	OLED_I2C_Start();
	OLED_Send_Byte(0x78);
	OLED_I2C_WaitAck();
	if(mode)
	{
		OLED_Send_Byte(0x40);
	}
	else
	{
		OLED_Send_Byte(0x00);
	}
	for(xi = 0; xi < 128; xi++)
	{
		OLED_I2C_WaitAck();
		OLED_Send_Byte(*pdat++);
	}
	OLED_I2C_WaitAck();
	OLED_I2C_Stop();
}

#endif

#else
//extern void SPI_OLED_WR_Byte(u8 dat,u8 cmd);
//extern void SPI_OLED_WR_Bytes(u8 *pdat,u8 cmd);

void OLED_WR_Byte(u8 dat,u8 cmd)
{
}
void OLED_WR_Bytes(u8 *pdat,u8 cmd)
{
}

#endif

//反显函数
void OLED_ColorTurn(u8 i)
{
	if(i==0)
	{
		OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
	}
	if(i==1)
	{
		OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
	}
}

//屏幕旋转180度
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
	{
		OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
		OLED_WR_Byte(0xA1,OLED_CMD);
	}
	if(i==1)
	{
		OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
		OLED_WR_Byte(0xA0,OLED_CMD);
	}
}

//开启OLED显示 
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
}

//关闭OLED显示 
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//关闭屏幕
}

//更新显存到OLED	
void OLED_Refresh(void)
{
	u8 y,x;
	u8 *pBuf;
	pBuf = OLED_GRAM;
	for(y=0; y<8; y++)
	{
		OLED_WR_Byte(0xb0+y,OLED_CMD); //设置行起始地址
		OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
		OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
		#if 1
		OLED_WR_Bytes(pBuf,OLED_DATA);
		pBuf = pBuf + 128;
		#else
		for(x=0; x<128; x++)
		{
			//OLED_WR_Byte(OLED_GRAM[x][y],OLED_DATA);
			OLED_WR_Byte(*pBuf++,OLED_DATA);
		}
		#endif
	}
}

void OLED_Clear_Buff(void)
{
	u8 y,x;
	u8 *pBuf;
	pBuf = OLED_GRAM;
	for(y=0;y<8;y++)
	{
	   for(x=0;x<128;x++)
		{
			//OLED_GRAM[x][y]=0;//清除所有数据
			*pBuf++ = 0;
		}
	}
}

//清屏函数
void OLED_Clear(void)
{
	u8 y,x;
	u8 *pBuf;
	pBuf = OLED_GRAM;
	for(y=0;y<8;y++)
	{
	   for(x=0;x<128;x++)
		{
			//OLED_GRAM[x][y]=0;//清除所有数据
			*pBuf++ = 0;
		}
	}
	OLED_Refresh();//更新显示
}

void OLED_DrawSegment(u8 x,u8 y,u8 segs)
{
	u16 y0;
	u16 offset;
	u8 *pBuf;
	pBuf = OLED_GRAM;

	if((x > 128)||(y > 64))
	{
		return;
	}

	y0 = (y & 0xF8) << 4;
	offset = y0 + (u16)x;

	//OLED_GRAM[x][y0]=segs;
	pBuf[offset] = segs;
}

//画点
//x:0~127
//y:0~63
void OLED_DrawPoint(u8 x,u8 y)
{
	u8 n;
	u16 y0;
	u8 shift_cnt;
	u16 offset;
	u8 *pBuf;
	pBuf = OLED_GRAM;

	if((x > 128)||(y > 64))
	{
		return;
	}

	y0 = (y & 0xF8) << 4;
	shift_cnt = y&0x07;

	n=1<<shift_cnt;

	offset = y0 + (u16)x;
	//OLED_GRAM[x][i]|=n;
	pBuf[offset] |= n;
}

//清除一个点
//x:0~127
//y:0~63
void OLED_ClearPoint(u8 x,u8 y)
{
	u8 n;
	u16 y0;
	u8 shift_cnt;
	u16 offset;
	u8 *pBuf;
	pBuf = OLED_GRAM;

	if((x > 128)||(y > 64))
	{
		return;
	}
	y0 = (y & 0xF8) << 4;
	shift_cnt = y&0x07;

	n=1<<shift_cnt;

	offset = y0 + (u16)x;
	pBuf[offset] &= (~n);
}

s8 i_abs(s8 n)
{
#if 1
	if (n < 0)
	{
		return -n;
	}
	else
	{
		return n;
	}
#else
	return ((n >> 31) ^ n) - (n >> 31);
#endif
}

//画线
//x:0~128
//y:0~64
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2)
{
	s32 deltaX = ((x1 < x2) ? (x2-x1) : (x1-x2));    //i_abs(x2 - x1);
	s32 deltaY = ((y1 < y2) ? (y2-y1) : (y1-y2));    //i_abs(y2 - y1);
	s32 signX = ((x1 < x2) ? 1 : -1);
	s32 signY = ((y1 < y2) ? 1 : -1);
	s32 error = deltaX - deltaY;
	s32 error2;

	OLED_DrawPoint(x2, y2);
	while((x1 != x2) || (y1 != y2))
	{
		OLED_DrawPoint(x1, y1);
		error2 = error * 2;
		if(error2 > -deltaY)
		{
			error -= deltaY;
			x1 += signX;
		}
		else
		{
			;/*nothing to do*/
		}

		if(error2 < deltaX)
		{
			error += deltaX;
			y1 += signY;
		}
		else
		{
			;/*nothing to do*/
		}
	}
	return;
}

//x,y:圆心坐标
//r:圆的半径
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b);
        OLED_DrawPoint(x - a, y - b);
        OLED_DrawPoint(x - a, y + b);
        OLED_DrawPoint(x + a, y + b);
 
        OLED_DrawPoint(x + b, y + a);
        OLED_DrawPoint(x + b, y - a);
        OLED_DrawPoint(x - b, y - a);
        OLED_DrawPoint(x - b, y + a);

        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}

void OLED_ShowCharR_8x16(u8 x,u8 y,u8 chr)
{
	u16 y0;
	u16 offset;
	u8 *pHzData;
	u8 x0;
	u8 *pBuf;
	pBuf = OLED_GRAM;

	y0 = (y & 0xF8) << 4;
	offset = y0 + (u16)x + 7;
	pHzData = (u8 *)(&(asc2_1608[(chr-' ')*16]));
	for(x0 = 0 ; x0 < 8; x0++)
	{
		pBuf[offset - x0] = pHzData[2*x0 + 1];
	}
	offset += 128;
	for(x0 = 0 ; x0 < 8; x0++)
	{
		pBuf[offset - x0] = pHzData[2*x0];
	}
}

void OLED_ShowChar_8x16(u8 x,u8 y,u8 chr)
{
	u8 y0;
	u8 x0;
	u16 Cur_Data;
	u16 n_shift;
	u8 fChar;
	u8 sChar;
	u8 *pHzData;

	pHzData = (u8 *)(&(asc2_1608[(chr-' ')*16]));
	for(x0 = 0 ; x0 < 8; x0++)
	{
		fChar = *pHzData++;
		sChar = *pHzData++;
		Cur_Data = ((u16)fChar << 8)|(u16)sChar;
		n_shift = 0x8000;
		for(y0 = 0 ; y0 < 16; y0++)
		{
			if(Cur_Data&n_shift)
				OLED_DrawPoint(x+x0,y+y0);
			else
				OLED_ClearPoint(x+x0,y+y0);
			n_shift >>= 1;
		}
	}	
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size:选择字体 12/16/24
//取模方式 逐列式
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1)
{
	u8 i,m,temp,size2,chr1;
	u8 y0=y;
	size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++)
	{
	#if 1
		if(size1 == 16)
        {
        	temp = asc2_1608[chr1*i*16];
		} //调用1608字体
		else
			return ;
	#else
		if(size1==12)
        {
        	temp=asc2_1206[chr1][i];
		} //调用1206字体
		else if(size1==16)
        {
        	temp=asc2_1608[chr1][i];
		} //调用1608字体
		else if(size1==24)
        {
        	temp=asc2_2412[chr1][i];
		} //调用2412字体
		else
			return;
	#endif
		for(m=0; m<8; m++)           //写入数据
		{
			if(temp&0x80)
				OLED_DrawPoint(x,y);
			else
				OLED_ClearPoint(x,y);
			temp<<=1;
			y++;
			if((y-y0)==size1)
			{
				y=y0;
				x++;
				break;
          	}
		}
	}
}

//显示字符串
//x,y:起点坐标  
//size1:字体大小 
//*chr:字符串起始地址 
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		OLED_ShowChar(x,y,*chr,size1);
		x+=size1/2;
		if(x>128-size1)  //换行
		{
			x=0;
			y+=2;
		}
		chr++;
	}
}

//m^n
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
		result*=m;
	}
	return result;
}

////显示2个数字
////x,y :起点坐标	 
////len :数字的位数
////size:字体大小
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1)
{
	u8 t,temp;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
		if(temp==0)
		{
			OLED_ShowChar(x+(size1/2)*t,y,'0',size1);
		}
		else 
		{
			OLED_ShowChar(x+(size1/2)*t,y,temp+'0',size1);
		}
	}
}

void OLED_ShowPF_16x16(u8 x,u8 y,u8 num)
{
	u16 y0;
	u16 offset;
	u8 *pHzData;
	u8 x0;
	u8 *pBuf;
	pBuf = OLED_GRAM;

	y0 = (y & 0xF8) << 4;
	offset = y0 + (u16)x;
	pHzData = (u8 *)(&(peak_freq_string[(u16)num*32]));
	for(y0 = 0 ; y0 < 2; y0++)
	{
		for(x0 = 0 ; x0 < 16; x0++)
		{
			pBuf[offset+x0] = *pHzData++;
			//pBuf[offset+x0] = *pHzData--;
		}
		offset += 128;
	}
}

//配置写入数据的起始位置
void OLED_WR_BP(u8 x,u8 y)
{
	OLED_WR_Byte(0xb0+y,OLED_CMD);//设置行起始地址
	//OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD);
}

//x0,y0：起点坐标
//x1,y1：终点坐标
//BMP[]：要写入的图片数组
void OLED_ShowPicture(u8 x0,u8 y0,u8 x1,u8 y1,u8 *BMP)
{
	u32 j=0;
	u8 x=0,y=0;
	/*
	if(y%8==0)
		y=0;
	else
		y+=1;
	*/
	for(y=y0; y<y1; y++)
	{
		OLED_WR_BP(x0,y);
		for(x=x0; x<x1; x++)
		{
			OLED_WR_Byte(BMP[j],OLED_DATA);
			j++;
    	}
	}
}

void OLED_ShowTest(u8 pattern)//(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[])
{
	u8 x,y;
	u8 rPat;

	rPat = ~pattern;
	for(y=0; y<8; y++)
	{
		OLED_WR_BP(0,y);
		for(x=0; x<128; x++)
		{
			if(x < 64)
				OLED_WR_Byte(pattern,OLED_DATA);
			else
				OLED_WR_Byte(rPat,OLED_DATA);
    	}
	}
}

//OLED的初始化
void IIC_OLED_Init(void)
{
#ifdef OLED_USE_HW_IIC
	Drv_I2C_Init();
#endif

	IIC_OLED_RST_Set();
	delay_ms(50);
	IIC_OLED_RST_Clr();//复位
	delay_ms(100);
	IIC_OLED_RST_Set();
	delay_ms(200);

	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);
	OLED_Clear();
}


