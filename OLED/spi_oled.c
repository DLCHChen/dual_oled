/*
CFP
2022 - 
*/
#include "board_cfg.h"
#include "iic_oled.h"
#include "spi_oled.h"
#include "debug.h"

extern const unsigned char asc2_1608[];
extern const unsigned char AsciiLib[];
#ifdef SPI_OLED_0P96_BW_PANEL
extern u8 OLED_GRAM[128*8];
#endif

extern const uint8_t Spec_string[];
extern const uint8_t peak_freq_string[];

u16 txwbuff[32*16];

#ifdef OLED_USE_SPI

#ifdef OLED_USE_HW_SPI
void DMA_Configuration(DMA_Channel_TypeDef *DMA_CHx, u32 src_addr, u32 des_buf)
{
	//// DMA1 channel 2 src_addr 3 des_buf 4 direction 5 DMA_CMD
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA_CHx); ////1//

	DMA_InitStructure.DMA_PeripheralBaseAddr = src_addr; ////2/
	DMA_InitStructure.DMA_MemoryBaseAddr = des_buf;			 //3
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	 //4
	DMA_InitStructure.DMA_BufferSize = 16;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE); //5
}

void Drv_SPI_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	SPI_InitTypeDef   SPI_InitStructure; 

	//SPI1 Periph clock enable 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE ) ;
	//PA05 - TFT --SCL/SCK
	//PA07 - TFT --SDA/DIN
	//PA04 - TFT --CS
	
	//PB01 - TFT --RST/RES
	//PA06 - TFT --RS/DC
	//PB00 - TFT --BLK/BL

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure PA03,PA04 as Output push-pull, used as  Chip select */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, (GPIO_Pin_3 | GPIO_Pin_4));			//SPI1 NSS

	/* Configure PA06  as intput push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure PB00, PB01 as Output push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, (GPIO_Pin_0 | GPIO_Pin_1));

	// SPI1 Config  
	#ifdef OLED_SPI_ONLY_TX
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	#else
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	#endif
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	#ifdef OLED_SPI_CS_SOFT
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //SPI_NSS_Hard
	#else
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //SPI_NSS_Hard
	#endif
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//SPI_BaudRatePrescaler_8; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1,&SPI_InitStructure);
	// SPI1 enable
	SPI_Cmd(SPI1,ENABLE);
	//DMA_Configuration(DMA1_Channel2, (u32)&SPI1->DR, (u32)spi1_rx_buf);
}

uint16_t SPI_RW_Byte(uint16_t data)
{
	//uint16_t temp;	//定义变量读取数据
	uint16_t timeout = 0x2710;   //10,000
	/* 等待发送缓存器空 */
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	/* 发送数据 */
	//SPI_I2S_SendData(SPI1, data);
	SPI1->DR = data;

	timeout = 0x2710;
	while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}

	timeout = 0x2710;
	while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	return timeout;
}

/**
  * @函数名       SPI_RW_Byte
  * @功  能       使用SPIx读写1Byte
  * @参  数       data：发送的数据
  * @返回值       temp：读取的数据
  */
uint16_t SPI_xRW_Byte(uint16_t data)
{
	//uint16_t temp;	//定义变量读取数据
	uint16_t timeout = 0x2710;   //10,000
	/* 等待发送缓存器空 */
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	/* 发送数据 */
	//SPI_I2S_SendData(SPI1, data);
	SPI1->DR = data;
	
	#ifdef OLED_SPI_ONLY_TX
	//SPI1->SR &= ~SPI_I2S_FLAG_RXNE;
	//timeout = SPI1->SR;
	//while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)== SET){}
	timeout = 0x2710;
	while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	return timeout;
	#else
	timeout = 0x2710;
	/* 等待接收缓存器非空 */
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	while((SPI1->SR & SPI_I2S_FLAG_RXNE)== RESET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	/* 接收一字节数据 */
	//temp = SPI_I2S_ReceiveData(SPI1);
	return SPI1->DR;
	#endif
}

uint16_t SPI_RW_2Byte(uint16_t data)
{
	//uint16_t temp;	//定义变量读取数据
	u8 shu1,shu2;
	uint16_t timeout = 0x2710;   //10,000
#if 0
	keep_2byte.Spi_u16Data = data;
	shu1 = keep_2byte.Spi_u8Data[1];
	shu2 = keep_2byte.Spi_u8Data[0];
#else
	shu1 = (u8)(data>>8);
	shu2 = (u8)(data);
#endif
	/* 等待发送缓存器空 */
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	/* 发送数据 */
	//SPI_I2S_SendData(SPI1, data);
	SPI1->DR = shu1;
	#if 1//ndef OLED_SPI_DATA_NO_CHK
	timeout = 0x2710;
	while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	#endif
	//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)== SET){}
	timeout = 0x2710;
	while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	
	/* 等待发送缓存器空 */
	timeout = 0x2710;
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	/* 发送数据 */
	//SPI_I2S_SendData(SPI1, data);
	SPI1->DR = shu2;
	#if 1//ndef OLED_SPI_DATA_NO_CHK
	timeout = 0x2710;
	while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	#endif
	//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)== SET){}
	timeout = 0x2710;
	while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	return data;
}

/**
  * @函数名       SPI_RW_Byte
  * @功  能       使用SPIx读写1Byte
  * @参  数       data：发送的数据
  * @返回值       temp：读取的数据
  */
uint16_t SPI_xRW_2Byte(uint16_t data)
{
	//uint16_t temp;	//定义变量读取数据
	u8 shu1,shu2;
	uint16_t timeout = 0x2710;   //10,000
#if 0
	keep_2byte.Spi_u16Data = data;
	shu1 = keep_2byte.Spi_u8Data[1];
	shu2 = keep_2byte.Spi_u8Data[0];
#else
	shu1 = (u8)(data>>8);
	shu2 = (u8)(data);
#endif
	/* 等待发送缓存器空 */
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	/* 发送数据 */
	//SPI_I2S_SendData(SPI1, data);
	SPI1->DR = shu1;
	
	#ifdef OLED_SPI_ONLY_TX
	//SPI1->SR &= ~SPI_I2S_FLAG_RXNE;
	timeout = SPI1->SR;
	//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)== SET){}
	timeout = 0x2710;
	while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	#else
	timeout = 0x2710;
	/* 等待接收缓存器非空 */
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	while((SPI1->SR & SPI_I2S_FLAG_RXNE)== RESET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	timeout = SPI1->DR;
	#endif
	
	/* 等待发送缓存器空 */
	timeout = 0x2710;
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	/* 发送数据 */
	//SPI_I2S_SendData(SPI1, data);
	SPI1->DR = shu2;
	
	#ifdef OLED_SPI_ONLY_TX
	//SPI1->SR &= SPI_I2S_FLAG_RXNE;
	timeout = SPI1->SR;
	//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)== SET){}
	timeout = 0x2710;
	while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	return data;
	#else
	timeout = 0x2710;
	/* 等待接收缓存器非空 */
	//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	while((SPI1->SR & SPI_I2S_FLAG_RXNE)== RESET)
	{
		if((timeout--)==0)
		{
			return 0;
		}
	}
	//shu1 = SPI1->DR;
	/* 接收一字节数据 */
	//temp = SPI_I2S_ReceiveData(SPI1);
	return SPI1->DR;
	#endif
}

uint16_t SPI_RW_Byte_Buffer(uint8_t *pdata, uint16_t len)
{
	uint16_t xlen = len;
	u16 tx_val;
	u16 rx_val = 0;
	u8 shu1;
	uint16_t timeout;   //10,000

	while(xlen--)
	{
		shu1 = *pdata++;
		tx_val = (u16)shu1;
		timeout = 0x2710;
		/* 等待发送缓存器空 */
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		/* 发送数据 */
		//SPI_I2S_SendData(SPI1, data);
		SPI1->DR = tx_val;
		
		#ifdef OLED_SPI_ONLY_TX
		//SPI1->SR &= SPI_I2S_FLAG_RXNE;
		timeout = SPI1->SR;
		//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)== SET){}
		timeout = 0x2710;
		while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		#else
		timeout = 0x2710;
		/* 等待接收缓存器非空 */
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		while((SPI1->SR & SPI_I2S_FLAG_RXNE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		rx_val = SPI1->DR;
		#endif
	}
	return rx_val;
}

uint16_t SPI_RW_2Byte_Buffer(uint16_t *pdata, uint16_t len)
{
	uint16_t xlen = len;
	u16 tx_val;
	u16 rx_val = 0;
	u8 shu1,shu2;
	uint16_t timeout;   //10,000

	while(xlen--)
	{
		#if 0
		keep_2byte.Spi_u16Data = *pdata++;
		shu1 = keep_2byte.Spi_u8Data[1];
		shu2 = keep_2byte.Spi_u8Data[0];
		#else
		tx_val = *pdata++;
		shu1 = (u8)(tx_val>>8);
		shu2 = (u8)(tx_val);
		#endif
		timeout = 0x2710;
		/* 等待发送缓存器空 */
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		/* 发送数据 */
		//SPI_I2S_SendData(SPI1, data);
		SPI1->DR = shu1;
		
		#ifdef OLED_SPI_ONLY_TX
		//SPI1->SR &= SPI_I2S_FLAG_RXNE;
		timeout = SPI1->SR;
		//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)== SET){}
		timeout = 0x2710;
		while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		#else
		timeout = 0x2710;
		/* 等待接收缓存器非空 */
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		while((SPI1->SR & SPI_I2S_FLAG_RXNE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		rx_val = SPI1->DR;
		#endif
		timeout = 0x2710;
		/* 等待发送缓存器空 */
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		/* 发送数据 */
		//SPI_I2S_SendData(SPI1, data);
		SPI1->DR = shu2;

		#ifdef OLED_SPI_ONLY_TX
		//SPI1->SR &= SPI_I2S_FLAG_RXNE;
		timeout = SPI1->SR;
		//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)== SET){}
		timeout = 0x2710;
		while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		#else
		timeout = 0x2710;
		/* 等待接收缓存器非空 */
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		while((SPI1->SR & SPI_I2S_FLAG_RXNE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		rx_val = SPI1->DR;
		#endif
	}
	return rx_val;
}

uint16_t SPI_RW_Repeat_2Byte(uint16_t data, uint16_t len)
{
	uint16_t xlen = len;
	u16 rx_val = data;
	u8 shu1,shu2;
	uint16_t timeout;   //10,000
#if 0
	keep_2byte.Spi_u16Data = data;
	shu1 = keep_2byte.Spi_u8Data[1];
	shu2 = keep_2byte.Spi_u8Data[0];
#else
	shu1 = (u8)(data>>8);
	shu2 = (u8)(data);
#endif
	while(xlen--)
	{
		timeout = 0x2710;
		/* 等待发送缓存器空 */
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		/* 发送数据 */
		//SPI_I2S_SendData(SPI1, data);
		SPI1->DR = shu1;
		#ifndef OLED_SPI_DATA_NO_CHK
		timeout = 0x2710;
		while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		#endif
		timeout = SPI1->SR;
		//while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)== SET){}
		timeout = 0x2710;
		while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		
		timeout = 0x2710;
		/* 等待发送缓存器空 */
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		/* 发送数据 */
		//SPI_I2S_SendData(SPI1, data);
		SPI1->DR = shu2;
		#if 1//ndef OLED_SPI_DATA_NO_CHK
		timeout = 0x2710;
		while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		#endif
		//while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)== SET){}
		timeout = 0x2710;
		while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
	}
	return rx_val;
}

uint16_t SPI_xRW_Repeat_2Byte(uint16_t data, uint16_t len)
{
	uint16_t xlen = len;
	u16 rx_val = 0;
	u8 shu1,shu2;
	uint16_t timeout;   //10,000
#if 0
	keep_2byte.Spi_u16Data = data;
	shu1 = keep_2byte.Spi_u8Data[1];
	shu2 = keep_2byte.Spi_u8Data[0];
#else
	shu1 = (u8)(data>>8);
	shu2 = (u8)(data);
#endif
	while(xlen--)
	{
		timeout = 0x2710;
		/* 等待发送缓存器空 */
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		/* 发送数据 */
		//SPI_I2S_SendData(SPI1, data);
		SPI1->DR = shu1;
		
		#ifdef OLED_SPI_ONLY_TX
		//SPI1->SR &= ~SPI_I2S_FLAG_RXNE;
		timeout = SPI1->SR;
		//while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)== SET){}
		timeout = 0x2710;
		while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		#else
		timeout = 0x2710;
		/* 等待接收缓存器非空 */
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		while((SPI1->SR & SPI_I2S_FLAG_RXNE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		rx_val = SPI1->DR;
		#endif
		
		timeout = 0x2710;
		/* 等待发送缓存器空 */
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		while((SPI1->SR & SPI_I2S_FLAG_TXE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		/* 发送数据 */
		//SPI_I2S_SendData(SPI1, data);
		SPI1->DR = shu2;
		
		#ifdef OLED_SPI_ONLY_TX
		//SPI1->SR &= ~SPI_I2S_FLAG_RXNE;
		timeout = SPI1->SR;
		//while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)== SET){}
		timeout = 0x2710;
		while((SPI1->SR & SPI_I2S_FLAG_BSY)== SET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		#else
		timeout = 0x2710;
		/* 等待接收缓存器非空 */
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		while((SPI1->SR & SPI_I2S_FLAG_RXNE)== RESET)
		{
			if((timeout--)==0)
			{
				return 0;
			}
		}
		rx_val = SPI1->DR;
		#endif
	}
	return rx_val;
}

//#ifndef OLED_USE_IIC
void SPI_OLED_WR_Byte(u8 dat,u8 cmd)
{
	//u8 i;
	if(cmd)
		OLED_SPI_DC_Set();
	else
		OLED_SPI_DC_Clr();
#ifdef OLED_SPI_CS_SOFT
	OLED_SPI_CS_Clr();
	SPI_RW_Byte(dat);
	OLED_SPI_CS_Set();
#else
	//SPI 写命令时序开始
	SPI_SSOutputCmd(SPI1, ENABLE);	//使能片选信号;
	SPI_RW_Byte(dat);
	SPI_SSOutputCmd(SPI1, DISABLE);
#endif
	//OLED_SPI_DC_Set();
}

void SPI_OLED_WR_Bytes(u8 *pdat,u8 cmd)
{
	u8 i;
	u8 *p2Buf = pdat;
	if(cmd)
		OLED_SPI_DC_Set();
	else
		OLED_SPI_DC_Clr();
#ifdef OLED_SPI_CS_SOFT
	OLED_SPI_CS_Clr();
	for(i = 0; i < 128; i++)
    {
    	SPI_RW_Byte(*p2Buf++);
	}
	OLED_SPI_CS_Set();
#else
	SPI_SSOutputCmd(SPI1, ENABLE);
	for(i = 0; i < 128; i++)
    {
    	SPI_RW_Byte(*p2Buf++);
	}
    SPI_SSOutputCmd(SPI1, DISABLE);
#endif
	//OLED_SPI_DC_Set();
}
//#endif

#else

void SPI_OLED_WR_Byte(u8 dat,u8 cmd)
{
	u8 i;
	if(cmd)
		OLED_SPI_DC_Set();
	else
		OLED_SPI_DC_Clr();
	OLED_SPI_CS_Clr();
	for(i=0; i<8; i++)
	{
		OLED_SPI_SCLK_Clr();
		if(dat&0x80)
			OLED_SPI_SDIN_Set();
		else
			OLED_SPI_SDIN_Clr();
		OLED_SPI_SCLK_Set();
		dat<<=1;
	}
	OLED_SPI_CS_Set();
	OLED_SPI_DC_Set();
}

void SPI_OLED_WR_Bytes(u8 *pdat,u8 cmd)
{
	u8 i;
	u8 j;
	u8 *p2Buf = pdat;
	u8 dat;
	if(cmd)
		OLED_SPI_DC_Set();
	else
		OLED_SPI_DC_Clr();
	OLED_SPI_CS_Clr();
	for(j = 0; j < 128; j++)
	{
		dat = *p2Buf++;
		for(i=0; i<8; i++)
		{
			OLED_SPI_SCLK_Clr();
			if(dat&0x80)
				OLED_SPI_SDIN_Set();
			else
				OLED_SPI_SDIN_Clr();
			OLED_SPI_SCLK_Set();
			dat<<=1;
		}
	}
	OLED_SPI_CS_Set();
	OLED_SPI_DC_Set();
}

#endif
#endif


#ifdef OLED_USE_HW_SPI
void Write_Command(unsigned char Cmd)
{
#ifdef OLED_SPI_CS_SOFT
	OLED_SPI_DC_Clr();  //DC拉低，发送命令
	SPI_NSS_LOW(); //片选拉低，选通器件
	SPI_RW_Byte(Cmd); //发送数据
	SPI_NSS_HIGH(); //片选拉高，关闭器件
	OLED_SPI_DC_Set(); //DC拉高，空闲时为高电平
#else
	OLED_SPI_DC_Clr();
	SPI_SSOutputCmd(SPI1, ENABLE);	//使能片选信号;
	SPI_RW_Byte(Cmd);
	SPI_SSOutputCmd(SPI1, DISABLE);
	OLED_SPI_DC_Set();
#endif
}

void Write_Data(unsigned char Data)
{
#ifdef OLED_SPI_CS_SOFT
	OLED_SPI_DC_Set();  //DC拉高，发送数据
	SPI_NSS_LOW(); //片选拉低，选通器件
	SPI_RW_Byte(Data); //发送数据
	SPI_NSS_HIGH(); //片选拉高，关闭器件
	//OLED_SPI_DC_Set(); //DC拉高，空闲时为高电平
#else
	OLED_SPI_DC_Set();
	SPI_SSOutputCmd(SPI1, ENABLE);	//使能片选信号;
	SPI_RW_Byte(Data);
	SPI_SSOutputCmd(SPI1, DISABLE);
	//OLED_SPI_DC_Set();
#endif
}

void Write_Data16(u16 Data)
{
	//u16 shu1,shu2;
	//shu1 = (u8)(Data>>8);
	//shu2 = (u8)(Data);
	OLED_SPI_DC_Set();
	#ifdef OLED_SPI_CS_SOFT
	SPI_NSS_LOW();
	#else
	SPI_SSOutputCmd(SPI1, ENABLE);	//使能片选信号;
	#endif
	//SPI_RW_Byte(shu1);
	//SPI_RW_Byte(shu2);
	SPI_RW_2Byte(Data);
	#ifdef OLED_SPI_CS_SOFT
	SPI_NSS_HIGH();
	#else
	SPI_SSOutputCmd(SPI1, DISABLE);
	#endif
	//OLED_SPI_DC_Set();
}

void Write_pData16(u16 Data, u16 len)
{
	u16 nums;
	//u16 odata;
	//u16 shu1,shu2;
	if(len == 0)
	{
		return;
	}
	//shu1 = (u8)(Data>>8);
	//shu2 = (u8)(Data);
	OLED_SPI_DC_Set();
	#ifdef OLED_SPI_CS_SOFT
	SPI_NSS_LOW();
	#else
	SPI_SSOutputCmd(SPI1, ENABLE);	//使能片选信号;
	#endif
	#if 1
	SPI_RW_Repeat_2Byte(Data,len);
	#else
	for(nums = 0; nums < len; nums++)
	{
    	//SPI_RW_Byte(shu1);
		//SPI_RW_Byte(shu2);
		SPI_RW_2Byte(Data);
	}
	#endif
    #ifdef OLED_SPI_CS_SOFT
	SPI_NSS_HIGH();
	#else
	SPI_SSOutputCmd(SPI1, DISABLE);
	#endif
	/////OLED_SPI_DC_Set();
}

void Write_Buffer16(u16 *pData, u16 len)
{
	u16 nums;
	//u16 odata;
	//u16 shu1,shu2;
	if(len == 0)
	{
		return;
	}
	OLED_SPI_DC_Set();
	#ifdef OLED_SPI_CS_SOFT
	SPI_NSS_LOW();
	#else
	SPI_SSOutputCmd(SPI1, ENABLE);	//使能片选信号;
	#endif
	#if 1
	SPI_RW_2Byte_Buffer(pData,len);
	#else
	for(nums = 0; nums < len; nums++)
	{
		//odata = *pData++;
		//shu1 = (u8)(odata>>8);
		//shu2 = (u8)(odata);
    	//SPI_RW_Byte(shu1);
		//SPI_RW_Byte(shu2);
		SPI_RW_2Byte(*Data++);
	}
	#endif
    #ifdef OLED_SPI_CS_SOFT
	SPI_NSS_HIGH();
	#else
	SPI_SSOutputCmd(SPI1, DISABLE);
	#endif
	//OLED_SPI_DC_Set();
}

#else

void Write_Command(unsigned char Cmd)
{
	u8 i;
	OLED_SPI_DC_Clr();
	OLED_SPI_CS_Clr();
	for(i=0; i<8; i++)
	{
		OLED_SPI_SCLK_Clr();
		if(Cmd&0x80)
			OLED_SPI_SDIN_Set();
		else
			OLED_SPI_SDIN_Clr();
		OLED_SPI_SCLK_Set();
		Cmd<<=1;
	}
	OLED_SPI_CS_Set();
	OLED_SPI_DC_Set();
}

void Write_Data(unsigned char Data)
{
	u8 i;
	OLED_SPI_DC_Set();
	OLED_SPI_CS_Clr();
	for(i=0; i<8; i++)
	{
		OLED_SPI_SCLK_Clr();
		if(Data&0x80)
			OLED_SPI_SDIN_Set();
		else
			OLED_SPI_SDIN_Clr();
		OLED_SPI_SCLK_Set();
		Data<<=1;
	}
	OLED_SPI_CS_Set();
	//OLED_SPI_DC_Set();
}

void Write_Data16(u16 Data)
{
	u8 i;
	OLED_SPI_DC_Set();
	OLED_SPI_CS_Clr();
	for(i=0; i<16; i++)
	{
		OLED_SPI_SCLK_Clr();
		if(Data&0x8000)
			OLED_SPI_SDIN_Set();
		else
			OLED_SPI_SDIN_Clr();
		OLED_SPI_SCLK_Set();
		Data<<=1;
	}
	OLED_SPI_CS_Set();
	//OLED_SPI_DC_Set();
}

void Write_pData16(u16 Data, u16 len)
{
	u16 nums;
	u16 odata;
	u8 i;
	if(len == 0)
	{
		return;
	}
	OLED_SPI_DC_Set();
	OLED_SPI_CS_Clr();
	for(nums = 0; nums < len; nums ++)
	{
		odata = Data;
		for(i=0; i<16; i++)
		{
			OLED_SPI_SCLK_Clr();
			if(odata&0x8000)
				OLED_SPI_SDIN_Set();
			else
				OLED_SPI_SDIN_Clr();
			OLED_SPI_SCLK_Set();
			odata<<=1;
		}
	}
	OLED_SPI_CS_Set();
	//OLED_SPI_DC_Set();
}

void Write_Buffer16(u16 *Data, u16 len)
{
	u16 nums;
	u16 odata;
	u8 i;
	if(len == 0)
	{
		return;
	}
	OLED_SPI_DC_Set();
	OLED_SPI_CS_Clr();
	for(nums = 0; nums < len; nums ++)
	{
		odata = *Data++;
		for(i=0; i<16; i++)
		{
			OLED_SPI_SCLK_Clr();
			if(odata&0x8000)
				OLED_SPI_SDIN_Set();
			else
				OLED_SPI_SDIN_Clr();
			OLED_SPI_SCLK_Set();
			odata<<=1;
		}
	}
	OLED_SPI_CS_Set();
	//OLED_SPI_DC_Set();
}

#endif

/*
0.96 oled - SSD1306 -- 128x64 (each dot has a bit)
1.3  oled(color) - SSD1351 -- 128x96 (each dot has 16bit RGB565 data)
*/

#ifdef OLED_0P96_COLOR_PANEL
void ST7735_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end)
{
	u16 txu16Data[2];
//xy = 160x80
	Write_Command(0x2a);
	//Write_Data(0x00);
	//Write_Data(x_start+1);//1~160不是0~159   实际传进的参数是0~159
	//Write_Data(0x00);
	//Write_Data(x_end+1);
	txu16Data[0] = x_start+1;
	txu16Data[1] = x_end+1;
	Write_Buffer16(txu16Data,2);

	Write_Command(0x2b);
	//Write_Data(0x00);
	//Write_Data(y_start+0x1A);//这个 26 (0x1A)与硬件有关
	//Write_Data(0x00);
	//Write_Data(y_end+0x1A);//这个 26 (0x1A)与硬件有关
	txu16Data[0] = y_start+0x1A;
	txu16Data[1] = y_end+0x1A;
	Write_Buffer16(txu16Data,2);

	Write_Command(0x2c);
}

void ST7735_SetXY(u16 x,u16 y)
{
  	ST7735_SetRegion(x,y,x,y);
}

void ST7735_DrawPoint(u16 x,u16 y,u16 color)
{
    if((x >= SPI_X_MAX_PIXEL) || (y >= SPI_Y_MAX_PIXEL))
        return;

	ST7735_SetRegion(x,y,x+1,y+1);
	//ST7735_SetRegion(x,y,x,y);
	Write_Data16(color);
}
unsigned int ST7735_ReadPoint(u16 x,u16 y)
{
	unsigned int Data;
	ST7735_SetXY(x,y);

	//Data = Read_Data();
	Write_Data(Data);
	return Data;
}

void ST7735_Clear(u16 Color)               
{	
	int i,m;
	ST7735_SetRegion(0,0,SPI_X_MAX_PIXEL-1,SPI_Y_MAX_PIXEL-1);
#if 1
	Write_pData16(Color,(SPI_X_MAX_PIXEL*SPI_Y_MAX_PIXEL));
#else
	for(i = 0; i < SPI_X_MAX_PIXEL; i++)
	{
		for(m = 0; m < SPI_Y_MAX_PIXEL; m++)
		{	
			Write_Data16(Color);
		}
	}
#endif
}

void ST7735_AllWin(void)
{
    Write_Command(0x28);
    Write_Command(0x13);
    Write_Command(0x29);
}

void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    // clipping
    if((x >= SPI_X_MAX_PIXEL) || (y >= SPI_Y_MAX_PIXEL))
		return;
    if((x + w - 1) >= SPI_X_MAX_PIXEL)
		w = SPI_X_MAX_PIXEL - x;
    if((y + h - 1) >= SPI_Y_MAX_PIXEL)
		h = SPI_Y_MAX_PIXEL - y;

    ST7735_SetRegion(x, y, x+w-1, y+h-1);
	#if 1
	Write_pData16(color,(w*h));
	#else
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            Write_Data16(color);
        }
    }
	#endif
}

void ST7735_WriteChar_8x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor)
{
    u8 i, j;
	u8 b;
	//u8 n_shift;
	u8 *pHzData;
	u16 *ptxwb;
	ptxwb = txwbuff;

	//pHzData = (u8 *)(&(asc2_1608[(num-' ')*16]));
	pHzData = (u8 *)(&(AsciiLib[(num-' ')*16]));
	pHzData += 16;

    ST7735_SetRegion(x, y, x+8-1, y+16-1);
#if 1
	for(i = 0; i < 16; i++) {
		b = *pHzData--;
		//n_shift = 0x80;
        for(j = 0; j < 8; j++) {
			if(b & 0x01)
			{
				*ptxwb++ = color;
			}
			else
			{
				//Write_Data16(bgcolor);
				*ptxwb++ = bgcolor;
			}
			b >>= 1;
        }
    }
	Write_Buffer16(txwbuff,(8*16));
#else
    for(i = 0; i < 16; i++) {
		b = *pHzData--;
		//n_shift = 0x80;
        for(j = 0; j < 8; j++) {
			if(b & 0x01)
			{
				Write_Data16(color);
			}
			else
			{
				Write_Data16(bgcolor);
			}
			b >>= 1;
        }
    }
#endif
}

void ST7735_WriteChar_R8x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor)
{
    u8 i, j;
	u8 b;
	u8 n_shift;
	u8 *pHzData;
	u16 *ptxwb;
	ptxwb = txwbuff;

	//pHzData = (u8 *)(&(asc2_1608[(num-' ')*16]));
	pHzData = (u8 *)(&(AsciiLib[(num-' ')*16]));

    ST7735_SetRegion(x, y, x+8-1, y+16-1);
#if 1
    for(i = 0; i < 16; i++) {
		b = *pHzData++;
		n_shift = 0x80;
        for(j = 0; j < 8; j++) {
			if(b & n_shift)
			{
				//Write_Data16(color);
				*ptxwb++ = color;
			}
			else
			{
				//Write_Data16(bgcolor);
				*ptxwb++ = bgcolor;
			}
			n_shift >>= 1;
        }
    }
	Write_Buffer16(txwbuff,(8*16));
#else
    for(i = 0; i < 16; i++) {
		b = *pHzData++;
		n_shift = 0x80;
        for(j = 0; j < 8; j++) {
			if(b & n_shift)
			{
				Write_Data16(color);
			}
			else
			{
				Write_Data16(bgcolor);
			}
			n_shift >>= 1;
        }
    }
#endif
}

void ST7735_WriteChar_16x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor)
{
    uint16_t i, j;
	uint16_t fx;
	uint16_t bx;
	//uint16_t n_shift;
	uint8_t *pHzData;
	u16 *ptxwb;
	ptxwb = txwbuff;

	pHzData = (u8 *)(&(Spec_string[num*32]));
	//pHzData = &(peak_freq_string[num*32]);
	pHzData +=31;

    ST7735_SetRegion(x, y, x+16-1, y+16-1);
#if 1
	for(i = 0; i < 16; i++) {
		bx = (u16)(*pHzData--);
		fx = (u16)(*pHzData--);
		//dbg_uart_print_str("fy-");
		//dbgu_PrintLongIntHex(bx);
		//dbg_uart_print_str("\r\n");
		///////bx = (bx << 8)| fx;
		bx = (fx << 8)| bx;
		//dbg_uart_print_str("by-");
		//dbgu_PrintLongIntHex(bx);
		//dbg_uart_print_str("\r\n");
		//n_shift = 0x8000;
		for(j = 0; j < 16; j++) {
			if(bx & 0x0001)
			{
				//Write_Data16(color);
				*ptxwb++ = color;
			}
			else
			{
				//Write_Data16(bgcolor);
				*ptxwb++ = bgcolor;
			}
			bx >>= 1;
		}
	}
	Write_Buffer16(txwbuff,(16*16));
#else
    for(i = 0; i < 16; i++) {
		bx = (u16)(*pHzData--);
		fx = (u16)(*pHzData--);
		//dbg_uart_print_str("fy-");
		//dbgu_PrintLongIntHex(bx);
		//dbg_uart_print_str("\r\n");
        ///////bx = (bx << 8)| fx;
        bx = (fx << 8)| bx;
		//dbg_uart_print_str("by-");
		//dbgu_PrintLongIntHex(bx);
		//dbg_uart_print_str("\r\n");
		//n_shift = 0x8000;
        for(j = 0; j < 16; j++) {
			if(bx & 0x0001)
			{
				Write_Data16(color);
			}
			else
			{
				Write_Data16(bgcolor);
			}
			bx >>= 1;
        }
    }
#endif
}

void ST7735_WriteChar_R16x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor)
{
    uint16_t i, j;
	uint16_t fx;
	uint16_t bx;
	uint16_t n_shift;
	uint8_t *pHzData;
	u16 *ptxwb;
	ptxwb = txwbuff;

	pHzData = (u8 *)(&(Spec_string[num*32]));
	//pHzData = &(peak_freq_string[num*32]);

    ST7735_SetRegion(x, y, x+16-1, y+16-1);
#if 1
	for(i = 0; i < 16; i++) {
		bx = (u16)(*pHzData++);
		fx = (u16)(*pHzData++);
		//dbg_uart_print_str("fy-");
		//dbgu_PrintLongIntHex(bx);
		//dbg_uart_print_str("\r\n");
        bx = (bx << 8)| fx;
		//dbg_uart_print_str("by-");
		//dbgu_PrintLongIntHex(bx);
		//dbg_uart_print_str("\r\n");
		n_shift = 0x8000;
        for(j = 0; j < 16; j++) {
			if(bx & n_shift)
			{
				//Write_Data16(color);
				*ptxwb++ = color;
			}
			else
			{
				//Write_Data16(bgcolor);
				*ptxwb++ = bgcolor;
			}
			n_shift >>= 1;
        }
    }
	Write_Buffer16(txwbuff,(16*16));
#else
    for(i = 0; i < 16; i++) {
		bx = (u16)(*pHzData++);
		fx = (u16)(*pHzData++);
		//dbg_uart_print_str("fy-");
		//dbgu_PrintLongIntHex(bx);
		//dbg_uart_print_str("\r\n");
        bx = (bx << 8)| fx;
		//dbg_uart_print_str("by-");
		//dbgu_PrintLongIntHex(bx);
		//dbg_uart_print_str("\r\n");
		n_shift = 0x8000;
        for(j = 0; j < 16; j++) {
			if(bx & n_shift)
			{
				Write_Data16(color);
			}
			else
			{
				Write_Data16(bgcolor);
			}
			n_shift >>= 1;
        }
    }
#endif
}

void ST7735_FillDraw_Test(uint8_t x, uint8_t y, uint16_t len,uint16_t color)
{
    if((x >= SPI_X_MAX_PIXEL) || (y >= SPI_Y_MAX_PIXEL))
        return;
	if((y + len - 1) >= SPI_Y_MAX_PIXEL)
		len = SPI_Y_MAX_PIXEL - y;

	ST7735_SetRegion(x, y, x+4, SPI_Y_MAX_PIXEL);
	#if 1
	Write_pData16(color,len);
	Write_pData16(RGB565_Black,(SPI_Y_MAX_PIXEL-len-y));
	#else
	for(; y < SPI_Y_MAX_PIXEL; y++)
	{
		if(y< len)
		{
			Write_Data16(color);
		}
		else
		{
			Write_Data16(RGB565_Black);
		}
	}
	#endif
}

//垂直画线
void ST7735_DrawVertical_Column(uint8_t x, uint8_t y, uint16_t len,uint16_t color)
{
    if((x >= SPI_X_MAX_PIXEL) || (y >= SPI_Y_MAX_PIXEL))
        return;
	if((y + len - 1) >= SPI_Y_MAX_PIXEL)
		len = SPI_Y_MAX_PIXEL - y;

	ST7735_SetRegion(x, y, x, SPI_Y_MAX_PIXEL);
	#if 1
	Write_pData16(color,len);
	Write_pData16(RGB565_Black,(SPI_Y_MAX_PIXEL - len - y));
	#else
	for(; y < SPI_Y_MAX_PIXEL; y++)
	{
		if(y< len)
		{
			Write_Data16(color);
		}
		else
		{
			Write_Data16(RGB565_Black);
		}
	}
	#endif
}

void ST7735_DrawHorizontal_Row(uint8_t x, uint8_t y, uint8_t w,uint16_t color)
{
    if((x >= SPI_X_MAX_PIXEL) || (y >= SPI_Y_MAX_PIXEL))
        return;
	if((x + w - 1) >= SPI_X_MAX_PIXEL)
		w = (SPI_X_MAX_PIXEL-1) - x;
	#if 1
	ST7735_SetRegion(x, y, x+w-1, y);
	Write_pData16(color,w);
	#else
    ST7735_SetRegion(x, y, x+w-1, y+1);
	for(x = w; x > 0; x--)
	{
		Write_Data16(color);
	}
	#endif
}

void ST7735_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2, u16 color)
{
	s32 deltaX = ((x1 < x2) ? (x2-x1) : (x1-x2));    //i_abs(x2 - x1);
	s32 deltaY = ((y1 < y2) ? (y2-y1) : (y1-y2));    //i_abs(y2 - y1);
	s32 signX = ((x1 < x2) ? 1 : -1);
	s32 signY = ((y1 < y2) ? 1 : -1);
	s32 error = deltaX - deltaY;
	s32 error2;

	ST7735_DrawPoint(x2, y2,color);
	while((x1 != x2) || (y1 != y2))
	{
		ST7735_DrawPoint(x1, y1,color);
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

#endif

#ifdef OLED_1P3_COLOR_PANEL
void Set_Column_Address(u8 Clo_Start, u8 Clo_End)
{
	u16 txu16Data;
	Write_Command(0x15);  // 设置列地址
	//Write_Data(Clo_Start);  // 列起始地址
	//Write_Data(Clo_End);  // 列结束地址
	txu16Data = (Clo_Start<<8)|Clo_End;
	Write_Data16(txu16Data);
}

void Set_Row_Address(u8 Row_Start, u8 Row_End)
{
	Write_Command(0x75);  // 设置行地址
	Write_Data(Row_Start);  // 行起始地址
	Write_Data(Row_End);  // 行结束地址
}

void Set_Write_RAM(void)
{
	Write_Command(0x5C);  //使能MCU写RAM
}

void Set_Read_RAM()
{
	Write_Command(0x5D);  //使能MCU读RAM
}

static void SSD1351_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	u16 txu16Data;
    // column address set
    Write_Command(0x15); // SETCOLUMN
    //Write_Data(x0);
	//Write_Data(x1);
	txu16Data = (x0<<8)|x1;
	Write_Data16(txu16Data);

    // row address set
    Write_Command(0x75); // SETROW
    //Write_Data(y0);
	//Write_Data(y1); 
	txu16Data = (y0<<8)|y1;
	Write_Data16(txu16Data);

    // write to RAM
    Write_Command(0x5C); // WRITERAM
}

void OLED_DrawPoint_Color(unsigned char x, unsigned char y, int Colour_RGB)
{
	Write_Command(0x15);  // 设置列地址
	Write_Data(x);    

	Write_Command(0x75);  // 设置行地址
	Write_Data(y);    

	Set_Write_RAM();  //使能MCU写RAM

	//Write_Data(Colour_RGB >> 8);
	//Write_Data(Colour_RGB);
	Write_Data16(Colour_RGB);
}

void SSD1351_InvertColors(u8 invert)
{
    Write_Command(invert ? 0xA7 /* INVERTDISPLAY */ : 0xA6 /* NORMALDISPLAY */);
}

void SSD1351_DrawPixel(uint8_t x, uint8_t y, uint16_t color)
{
    if((x >= SPI_X_MAX_PIXEL) || (y >= SPI_Y_MAX_PIXEL))
        return;

    SSD1351_SetAddressWindow(x, y, x+1, y+1);
    //Write_Data(color >> 8);
	//Write_Data(color & 0xFF);
	Write_Data16(color);
}

void SSD1351_DrawVertical_Column(uint8_t x, uint8_t y, uint8_t len,uint16_t color)
{
	u8 i;
    if((x >= SPI_X_MAX_PIXEL) || (y >= SPI_Y_MAX_PIXEL))
        return;
	if((y + len) >= SPI_Y_MAX_PIXEL)
		len = SPI_Y_MAX_PIXEL - y;

    //SSD1351_SetAddressWindow(x, y, x+1, y+len-1);
    //SSD1351_SetAddressWindow(x, y, x+1, y+96-1);
    SSD1351_SetAddressWindow(x, y, x, SPI_Y_MAX_PIXEL);
	#if 1
	Write_pData16(color,len);
	Write_pData16(RGB565_Black,(SPI_Y_MAX_PIXEL - len - y + 1));
	#else
	for(i = 0; i < (SPI_Y_MAX_PIXEL - y); i++)
	{
		if(i < len)
		{
			Write_Data(color >> 8);
			Write_Data(color & 0xFF);
		}
		else
		{
			Write_Data(0);
			Write_Data(0);
		}
	}
	#endif
}

void SSD1351_DrawHorizontal_Row(uint8_t x, uint8_t y, uint8_t w,uint16_t color)
{
    if((x >= SPI_X_MAX_PIXEL) || (y >= SPI_Y_MAX_PIXEL))
        return;
	if((x + w - 1) >= SPI_X_MAX_PIXEL)
		w = SPI_X_MAX_PIXEL - x;

    //SSD1351_SetAddressWindow(x, y, x+w-1, y+1);
    SSD1351_SetAddressWindow(x, y, x+w-1, y);
	#if 1
	Write_pData16(color,w);
	#else
	for(x = w; x > 0; x--)
	{
		Write_Data(color >> 8);
		Write_Data(color & 0xFF);
	}
	#endif
}

void SSD1351_FillRectangleX(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color)
{
    // clipping
    if((x >= SPI_X_MAX_PIXEL) || (y >= SPI_Y_MAX_PIXEL))
		return;
    if((x + w) >= SPI_X_MAX_PIXEL)
		w = SPI_X_MAX_PIXEL - x;
    if((y + h) >= SPI_Y_MAX_PIXEL)
		h = SPI_Y_MAX_PIXEL - y;

    //SSD1351_SetAddressWindow(x, y, x+w-1, y+h-1);
    SSD1351_SetAddressWindow(x, y, x+w-1, y+SPI_Y_MAX_PIXEL-1);
	#if 1
	Write_pData16(color,(w*h));
	Write_pData16(RGB565_Black,(w*(SPI_Y_MAX_PIXEL-h)));
	#else
    for(y = 0; y < SPI_Y_MAX_PIXEL; y++) {
        for(x = w; x > 0; x--) {
			if(y < h)
			{
            	Write_Data(color >> 8);
            	Write_Data(color & 0xFF);
			}
			else
			{
				Write_Data(0);
            	Write_Data(0);
			}
        }
    }
	#endif
}

void SSD1351_FillRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color)
{
    // clipping
    if((x >= SPI_X_MAX_PIXEL) || (y >= SPI_Y_MAX_PIXEL))
		return;
    if((x + w) >= SPI_X_MAX_PIXEL)
		w = SPI_X_MAX_PIXEL - x;
    if((y + h) >= SPI_Y_MAX_PIXEL)
		h = SPI_Y_MAX_PIXEL - y;

    SSD1351_SetAddressWindow(x, y, x+w-1, y+h-1);
    //SSD1351_SetAddressWindow(x, y, x+w, y+h);
	#if 1
	Write_pData16(color,(w*h));
	#else
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            Write_Data(color >> 8);
            Write_Data(color & 0xFF);
        }
    }
	#endif
}

void SSD1351_WriteChar_16x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor)
{
    u8 i, j;
	u16 f;
	u16 b;
	//u16 n_shift;
	u8 *pHzData;
	u16 *ptxwb;
	ptxwb = txwbuff;
	pHzData = (u8 *)(&(Spec_string[num*32]));

    SSD1351_SetAddressWindow(x, y, x+16-1, y+16-1);
	pHzData += 31;
#if 1
	for(i = 0; i < 16; i++) {
		f = *pHzData--;
		b =  ((*pHzData--)<< 8)|(f);
		for(j = 0; j < 16; j++) {
			if(b & 0x0001)
			{
				*ptxwb++ = color;
			}
			else
			{
				*ptxwb++ = bgcolor;
			}
			b >>= 1;
		}
	}
	Write_Buffer16(txwbuff,(16*16));
#else
    for(i = 0; i < 16; i++) {
		f = *pHzData--;
        b = (f<<8) | (*pHzData--);
        for(j = 0; j < 16; j++) {
			if(b & 0x0001)
			{
				//Write_Data(color >> 8);
				//Write_Data(color & 0xFF);
				Write_Data16(color);
			}
			else
			{
				//Write_Data(bgcolor >> 8);
				//Write_Data(bgcolor & 0xFF);
				Write_Data16(bgcolor);
			}
			b >>= 1;
        }
    }
#endif
}

void SSD1351_WriteChar_R16x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor)
{
    u8 i, j;
	u16 f;
	u16 b;
	u16 n_shift;
	u8 *pHzData;

	pHzData = (u8 *)(&(Spec_string[num*32]));

    SSD1351_SetAddressWindow(x, y, x+16-1, y+16-1);

    for(i = 0; i < 16; i++) {
		f = *pHzData++;
        b = (f<<8) | (*pHzData++);
		n_shift = 0x8000;
        for(j = 0; j < 16; j++) {
			if(b & n_shift)
			{
				//Write_Data(color >> 8);
				//Write_Data(color & 0xFF);
				Write_Data16(color);
			}
			else
			{
				//Write_Data(bgcolor >> 8);
				//Write_Data(bgcolor & 0xFF);
				Write_Data16(bgcolor);
			}
			n_shift >>= 1;
        }
    }
}

void SSD1351_WriteChar_8x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor)
{
    u8 i, j;
	u8 b;
	//u8 n_shift;
	u8 *pHzData;
	u16 *ptxwb;
	ptxwb = txwbuff;

	//pHzData = (u8 *)(&(asc2_1608[(num-' ')*16]));
	pHzData = (u8 *)(&(AsciiLib[(num-' ')*16]));
	pHzData += 15;

    SSD1351_SetAddressWindow(x, y, x+8-1, y+16-1);
#if 1
	for(i = 0; i < 16; i++) {
		b = *pHzData--;
		//n_shift = 0x80;
        for(j = 0; j < 8; j++) {
			if(b & 0x01)
			{
				*ptxwb++ = color;
			}
			else
			{
				*ptxwb++ = bgcolor;
			}
			b >>= 1;
        }
    }
	Write_Buffer16(txwbuff,(8*16));
#else
    for(i = 0; i < 16; i++) {
		b = *pHzData--;
		//n_shift = 0x80;
        for(j = 0; j < 8; j++) {
			if(b & 0x01)
			{
				//Write_Data(color >> 8);
				//Write_Data(color & 0xFF);
				Write_Data16(color);
			}
			else
			{
				//Write_Data(bgcolor >> 8);
				//Write_Data(bgcolor & 0xFF);
				Write_Data16(bgcolor);
			}
			b >>= 1;
        }
    }
#endif
}

void SSD1351_WriteChar_R8x16(uint8_t x, uint8_t y, u8 num, uint16_t color, uint16_t bgcolor)
{
    u8 i, j;
	u8 b;
	u8 n_shift;
	u8 *pHzData;

	//pHzData = (u8 *)(&(asc2_1608[(num-' ')*16]));
	pHzData = (u8 *)(&(AsciiLib[(num-' ')*16]));

    SSD1351_SetAddressWindow(x, y, x+8-1, y+16-1);

    for(i = 0; i < 16; i++) {
		b = *pHzData++;
		n_shift = 0x80;
        for(j = 0; j < 8; j++) {
			if(b & n_shift)
			{
				//Write_Data(color >> 8);
				//Write_Data(color & 0xFF);
				Write_Data16(color);
			}
			else
			{
				//Write_Data(bgcolor >> 8);
				//Write_Data(bgcolor & 0xFF);
				Write_Data16(bgcolor);
			}
			n_shift >>= 1;
        }
    }
}

void SSD1351_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2, u16 color)
{
	s32 deltaX = ((x1 < x2) ? (x2-x1) : (x1-x2));    //i_abs(x2 - x1);
	s32 deltaY = ((y1 < y2) ? (y2-y1) : (y1-y2));    //i_abs(y2 - y1);
	s32 signX = ((x1 < x2) ? 1 : -1);
	s32 signY = ((y1 < y2) ? 1 : -1);
	s32 error = deltaX - deltaY;
	s32 error2;

	SSD1351_DrawPixel(x2, y2,color);
	while((x1 != x2) || (y1 != y2))
	{
		SSD1351_DrawPixel(x1, y1,color);
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

void Set_Remap_Format(unsigned char d)
{
	Write_Command(0xA0);			// Set Re-Map / Color Depth
	Write_Data(d);				    //   Default => 0x40
						            //     Horizontal Address Increment
                                    //     Column Address 0 Mapped to SEG0
                                    //     Color Sequence: A => B => C
                                    //     Scan from COM0 to COM[N-1]
                                    //     Disable COM Split Odd Even
                                    //     65,536 Colors
}

void Set_Start_Line(unsigned char d)
{
	Write_Command(0xA1);			// Set Vertical Scroll by RAM
	Write_Data(d);				    //   Default => 0x00
}

void Set_Display_Offset(unsigned char d)
{
	Write_Command(0xA2);			// Set Vertical Scroll by Row
	Write_Data(d);				    //   Default => 0x60
}

void Set_Display_Brightness(unsigned char d)
{
    Write_Command(0xc1);
    Write_Command(d);

}

void Set_Display_Mode(unsigned char d)
{
	Write_Command(0xA4|d);			// Set Display Mode
						            //   Default => 0xA6
						            //     0xA4 (0x00) => Entire Display Off, All Pixels Turn Off
						            //     0xA5 (0x01) => Entire Display On, All Pixels Turn On at GS Level 63
						            //     0xA6 (0x02) => Normal Display
						            //     0xA7 (0x03) => Inverse Display
}

void Set_Function_Selection(unsigned char d)
{
	Write_Command(0xAB);			// Function Selection
	Write_Data(d);				    //   Default => 0x01
						            //     Enable Internal VDD Regulator
						            //     Select 8-bit Parallel Interface
}

void Set_Display_On_Off(unsigned char d)
{
	Write_Command(0xAE|d);			// Set Display On/Off
						            //   Default => 0xAE
						            //     0xAE (0x00) => Display Off (Sleep Mode On)
						            //     0xAF (0x01) => Display On (Sleep Mode Off)
}

void Set_Phase_Length(unsigned char d)
{
	Write_Command(0xB1);			// Phase 1 (Reset) & Phase 2 (Pre-Charge) Period Adjustment
	Write_Data(d);				    //   Default => 0x82 (8 Display Clocks [Phase 2] / 5 Display Clocks [Phase 1])
						            //     D[3:0] => Phase 1 Period in 5~31 Display Clocks
					            	//     D[7:4] => Phase 2 Period in 3~15 Display Clocks
}

void Set_Display_Enhancement(unsigned char d)
{
	Write_Command(0xB2);			// Display Enhancement
	Write_Data(d);				    //   Default => 0x00 (Normal)
	Write_Data(0x00);
	Write_Data(0x00);
}

void Set_Display_Clock(unsigned char d)
{
	Write_Command(0xB3);			// Set Display Clock Divider / Oscillator Frequency
	Write_Data(d);				    //   Default => 0x00
						            //     A[3:0] => Display Clock Divider
						            //     A[7:4] => Oscillator Frequency
}

void Set_VSL(unsigned char d)
{
	Write_Command(0xB4);			// Set Segment Low Voltage
	Write_Data(0xA0|d);			    //   Default => 0xA0
						            //     0xA0 (0x00) => Enable External VSL
						            //     0xA2 (0x02) => Enable Internal VSL (Kept VSL Pin N.C.)
	Write_Data(0xB5);
	Write_Data(0x55);
}

void Set_GPIO(unsigned char d)
{
	Write_Command(0xB5);			// General Purpose IO
	Write_Data(d);				    //   Default => 0x0A (GPIO Pins output Low Level.)
}

void Set_Precharge_Period(unsigned char d)
{
	Write_Command(0xB6);			// Set Second Pre-Charge Period
	Write_Data(d);				    //   Default => 0x08 (8 Display Clocks)
}

void Set_Precharge_Voltage(unsigned char d)
{
	Write_Command(0xBB);			// Set Pre-Charge Voltage Level
	Write_Data(d);				    //   Default => 0x17 (0.50*VCC)
}

void Set_VCOMH(unsigned char d)
{
	Write_Command(0xBE);			// Set COM Deselect Voltage Level
	Write_Data(d);				    //   Default => 0x05 (0.82*VCC)
}

void Set_Contrast_Color(unsigned char a, unsigned char b, unsigned char c)
{
	Write_Command(0xC1);			// Set Contrast Current for Color A, B, C
	Write_Data(a);				    //   Default => 0x8A (Color A)
	Write_Data(b);				    //   Default => 0x51 (Color B)
	Write_Data(c);				    //   Default => 0x8A (Color C)
}

void Set_Master_Current(unsigned char d)
{
	Write_Command(0xC7);			// Master Contrast Current Control
	Write_Data(d);				    //   Default => 0x0F (Maximum)
}

void Set_Multiplex_Ratio(unsigned char d)
{
	Write_Command(0xCA);			// Set Multiplex Ratio
	Write_Data(d);				    //   Default => 0x7F (1/128 Duty)
}

void Set_Command_Lock(unsigned char d)
{
	Write_Command(0xFD);			// Set Command Lock
	Write_Data(d);				    //   Default => 0x12
						            //     0x12 => Driver IC interface is unlocked from entering command.
						            //     0x16 => All Commands are locked except 0xFD.
						            //     0xB0 => Command 0xA2, 0xB1, 0xB3, 0xBB & 0xBE are inaccessible.
						            //     0xB1 => All Commands are accessible.
}

void Set_Gray_Scale_Table(void)
{
	Write_Command(0xB8);
	Write_Data(0x02);	  // Gray Scale Level 1
	Write_Data(0x03);	  // Gray Scale Level 2
	Write_Data(0x04);	  // Gray Scale Level 3
	Write_Data(0x05);	  // Gray Scale Level 4
	Write_Data(0x06);	  // Gray Scale Level 5
	Write_Data(0x07);	  // Gray Scale Level 6
	Write_Data(0x08);	  // Gray Scale Level 7
	Write_Data(0x09);	  // Gray Scale Level 8
	Write_Data(0x0A);	  // Gray Scale Level 9
	Write_Data(0x0B);	  // Gray Scale Level 10
	Write_Data(0x0C);	  // Gray Scale Level 11
	Write_Data(0x0D);	  // Gray Scale Level 12
	Write_Data(0x0E);	  // Gray Scale Level 13
	Write_Data(0x0F);	  // Gray Scale Level 14
	Write_Data(0x10);	  // Gray Scale Level 15
	Write_Data(0x11);	  // Gray Scale Level 16
	Write_Data(0x12);	  // Gray Scale Level 17
	Write_Data(0x13);	  // Gray Scale Level 18
	Write_Data(0x15);	  // Gray Scale Level 19
	Write_Data(0x17);	  // Gray Scale Level 20
	Write_Data(0x19);	  // Gray Scale Level 21
	Write_Data(0x1B);	  // Gray Scale Level 22
	Write_Data(0x1D);	  // Gray Scale Level 23
	Write_Data(0x1F);	  // Gray Scale Level 24
	Write_Data(0x21);	  // Gray Scale Level 25
	Write_Data(0x23);	  // Gray Scale Level 26
	Write_Data(0x25);	  // Gray Scale Level 27
	Write_Data(0x27);	  // Gray Scale Level 28
	Write_Data(0x2A);	  // Gray Scale Level 29
	Write_Data(0x2D);	  // Gray Scale Level 30
	Write_Data(0x30);	  // Gray Scale Level 31
	Write_Data(0x33);	  // Gray Scale Level 32
	Write_Data(0x36);	  // Gray Scale Level 33
	Write_Data(0x39);	  // Gray Scale Level 34
	Write_Data(0x3C);	  // Gray Scale Level 35
	Write_Data(0x3F);	  // Gray Scale Level 36
	Write_Data(0x42);	  // Gray Scale Level 37
	Write_Data(0x45);	  // Gray Scale Level 38
	Write_Data(0x48);	  // Gray Scale Level 39
	Write_Data(0x4C);	  // Gray Scale Level 40
	Write_Data(0x50);	  // Gray Scale Level 41
	Write_Data(0x54);	  // Gray Scale Level 42
	Write_Data(0x58);	  // Gray Scale Level 43
	Write_Data(0x5C);	  // Gray Scale Level 44
	Write_Data(0x60);	  // Gray Scale Level 45
	Write_Data(0x64);	  // Gray Scale Level 46
	Write_Data(0x68);	  // Gray Scale Level 47
	Write_Data(0x6C);	  // Gray Scale Level 48
	Write_Data(0x70);	  // Gray Scale Level 49
	Write_Data(0x74);	  // Gray Scale Level 50
	Write_Data(0x78);	  // Gray Scale Level 51
	Write_Data(0x7D);	  // Gray Scale Level 52
	Write_Data(0x82);	  // Gray Scale Level 53
	Write_Data(0x87);	  // Gray Scale Level 54
	Write_Data(0x8C);	  // Gray Scale Level 55
	Write_Data(0x91);	  // Gray Scale Level 56
	Write_Data(0x96);	  // Gray Scale Level 57
	Write_Data(0x9B);	  // Gray Scale Level 58
	Write_Data(0xA0);	  // Gray Scale Level 59
	Write_Data(0xA5);	  // Gray Scale Level 60
	Write_Data(0xAA);	  // Gray Scale Level 61
	Write_Data(0xAF);	  // Gray Scale Level 62
	Write_Data(0xB4);	  // Gray Scale Level 63
}

void Fill_RAM(u16 Colour_RGB)
{
	u8 i,j;  //定义变量

	Set_Column_Address(0x00,0x7F);  //设置列地址0~127
	//Set_Row_Address(0x00,0x7F);   //设置行地址0~127
	Set_Row_Address(0x00,0x5F);   //设置行地址0~95
	Set_Write_RAM();  //使能MCU写RAM
#if 1
	Write_pData16(Colour_RGB,(SPI_X_MAX_PIXEL*SPI_Y_MAX_PIXEL));
	/*
	for(i=0;i<SPI_X_MAX_PIXEL;i++)  //填充128列*128行
	{
		for(j=0;j<SPI_Y_MAX_PIXEL;j++)
		{
			Write_Data(Colour_RGB >> 8);	//写入填充颜色高字节
			Write_Data(Colour_RGB);			//写入填充颜色低字节
		}
	}
	*/
#else
	for(i=0;i<128;i++)  //填充128列*128行
	{
		for(j=0;j<128;j++)
		{
			Write_Data(Colour_RGB >> 8);	//写入填充颜色高字节
			Write_Data(Colour_RGB);			//写入填充颜色低字节
		}
	}
#endif
}

#endif

#ifdef SPI_OLED_0P96_BW_PANEL
//反显函数
void SPI_OLED_ColorTurn(u8 i)
{
	if(i==0)
	{
		SPI_OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
	}
	if(i==1)
	{
		SPI_OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
	}
}

//屏幕旋转180度
void SPI_OLED_DisplayTurn(u8 i)
{
	if(i==0)
	{
		SPI_OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
		SPI_OLED_WR_Byte(0xA1,OLED_CMD);
	}
	if(i==1)
	{
		SPI_OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
		SPI_OLED_WR_Byte(0xA0,OLED_CMD);
	}
}

//画点
//x:0~127
//y:0~63
void SPI_OLED_DrawPoint(u8 x,u8 y)
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

void SPI_OLED_Clear_Buff(void)
{
	u8 y,x;
	u8 *pBuf;
#ifdef OLED_0P96_BW_PANEL
	//do nothing
#else
	pBuf = OLED_GRAM;
	for(y=0;y<8;y++)
	{
	   for(x=0;x<128;x++)
		{
			//OLED_GRAM[x][y]=0;//清除所有数据
			*pBuf++ = 0;
		}
	}
#endif
}

void SPI_OLED_Refresh(void)
{
	u8 y,x;
	u8 *pBuf;
	pBuf = OLED_GRAM;
	for(y=0; y<8; y++)
	{
		SPI_OLED_WR_Byte(0xb0+y,OLED_CMD); //设置行起始地址
		SPI_OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
		SPI_OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
		#if 1
		SPI_OLED_WR_Bytes(pBuf,OLED_DATA);
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

//清屏函数
void SPI_OLED_Clear(void)
{
#ifdef OLED_0P96_BW_PANEL
//do nothing
#else
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
#endif
	SPI_OLED_Refresh();//更新显示
}

void Spi_OLED_Init(void)
{
#ifdef OLED_USE_HW_SPI
	Drv_SPI_Init();
#endif

	OLED_SPI_RST_Set();
	delay_ms(50);
	OLED_SPI_RST_Clr();//复位
	delay_ms(100);
	OLED_SPI_RST_Set();
	delay_ms(200);

	SPI_OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	SPI_OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	SPI_OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	SPI_OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	SPI_OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	SPI_OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	SPI_OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	SPI_OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	SPI_OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	SPI_OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	SPI_OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	SPI_OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	SPI_OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	SPI_OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	SPI_OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	SPI_OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	SPI_OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	SPI_OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	SPI_OLED_WR_Byte(0x12,OLED_CMD);
	SPI_OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	SPI_OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	SPI_OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	SPI_OLED_WR_Byte(0x02,OLED_CMD);//
	SPI_OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	SPI_OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	SPI_OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	SPI_OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	SPI_OLED_WR_Byte(0xAF,OLED_CMD);
	SPI_OLED_Clear();
}

#endif

#define	Brightness	0x0f //0xFF
#ifdef OLED_0P96_COLOR_PANEL
//OLED的初始化
void Spi_OLED_Init(void)
{
	OLED_BLK_Clr();
#ifdef OLED_USE_HW_SPI
	Drv_SPI_Init();
//	SPI_SSOutputCmd(SPI1, DISABLE);
//#else
//	OLED_SPI_CS_Clr();
#endif
	OLED_SPI_RST_Set();
	delay_ms(50);
	OLED_SPI_RST_Clr();//复位
	delay_ms(100);
	OLED_SPI_RST_Set();
	delay_ms(200);
	//init
	Write_Command(0x11); //sleep out
	delay_ms(100);

	Write_Command(0xB1);
	Write_Data(0x05);
	Write_Data(0x3A);
	Write_Data(0x3A);

	Write_Command(0xB2);
	Write_Data(0x05);
	Write_Data(0x3A);
	Write_Data(0x3A);

	Write_Command(0xB3);
	Write_Data(0x05);
	Write_Data(0x3A);
	Write_Data(0x3A);
	Write_Data(0x05);
	Write_Data(0x3A);
	Write_Data(0x3A);

	Write_Command(0x21);     //0x21:反色关; 0x20:反色开

	Write_Command(0xB4);
	Write_Data(0x03);

	Write_Command(0xC0);
	Write_Data(0x62);
	Write_Data(0x02);
	Write_Data(0x04);

	Write_Command(0xC1);
	Write_Data(0xC0);

	Write_Command(0xC2);
	Write_Data(0x0D);
	Write_Data(0x00);

	Write_Command(0xC3);
	Write_Data(0x8D);
	Write_Data(0x6A);

	Write_Command(0xC4);
	Write_Data(0x8D);
	Write_Data(0x6A);    // 0xee?

	Write_Command(0xC5);   //Vcom
	Write_Data(0x0E);

	Write_Command(0xE0);
	Write_Data(0x10);
	Write_Data(0x0E);
	Write_Data(0x02);
	Write_Data(0x03);
	Write_Data(0x0E);
	Write_Data(0x07);
	Write_Data(0x02);
	Write_Data(0x07);
	Write_Data(0x0A);
	Write_Data(0x12);
	Write_Data(0x27);
	Write_Data(0x37);
	Write_Data(0x00);
	Write_Data(0x0D);
	Write_Data(0x0E);
	Write_Data(0x10);

	Write_Command(0xE1);
	Write_Data(0x10);
	Write_Data(0x0E);
	Write_Data(0x03);
	Write_Data(0x03);
	Write_Data(0x0F);
	Write_Data(0x06);
	Write_Data(0x02);
	Write_Data(0x08);
	Write_Data(0x0A);
	Write_Data(0x13);
	Write_Data(0x26);
	Write_Data(0x36);
	Write_Data(0x00);
	Write_Data(0x0D);
	Write_Data(0x0E);
	Write_Data(0x10);

	Write_Command(0x3A);    //颜色格式 16bit 
	Write_Data(0x05);

	Write_Command(0x36);
	/* 0x78
	         y^
	          |
	 <-x------+
	*/
	Write_Data(0xa8);//(0x78);       //0xA8 or 0x78 180度旋转
	/* 0xa8
      +-----x-->
	  |
	  v  y
	*/
	//Write_Data(0xa8);

	Write_Command(0x28); 	//Display OFF
	ST7735_Clear(RGB565_Black);//RGB565_Dgreen);
	OLED_BLK_Set();
	Write_Command(0x29); //display on
}

#endif

#ifdef OLED_1P3_COLOR_PANEL
//OLED的初始化
void Spi_OLED_Init(void)
{
#ifdef OLED_USE_HW_SPI
	Drv_SPI_Init();
#endif

	OLED_SPI_RST_Set();
	delay_ms(50);
	OLED_SPI_RST_Clr();//复位
	delay_ms(100);
	OLED_SPI_RST_Set();
	delay_ms(200);
#ifdef NEW_SSD1351_INIT
	Set_Command_Lock(0x12); 			// Unlock Driver IC (0x12/0x16/0xB0/0xB1)
	Set_Command_Lock(0xB1); 			// Unlock All Commands (0x12/0x16/0xB0/0xB1)
	Set_Display_On_Off(0x00);			// Display Off (0x00/0x01)
	Set_Display_Clock(0xF1);			// Set Clock as 90 Frames/Sec
	Set_Multiplex_Ratio(0x7F);			// 1/128 Duty (0x0F~0x7F)
	Set_Display_Offset(0x00);			// Shift Mapping RAM Counter (0x00~0x7F)
	Set_Start_Line(0x00);				// Set Mapping RAM Display Start Line (0x00~0x7F)
	Set_Remap_Format(0x74); 			// Set Horizontal Address Increment
										//	   Column Address 0 Mapped to SEG0
										//	   Color Sequence D[15:0]=[RRRRR:GGGGGG:BBBBB]
										//	   Scan from COM127 to COM0
										//	   Enable COM Split Odd Even
										//	   65,536 Colors Mode (0x74)
										//	   * 262,144 Colors Mode (0xB4)
	Set_GPIO(0x00); 					// Disable GPIO Pins Input
	Set_Function_Selection(0x01);		// Enable Internal VDD Regulator
										// Select 8-bit Parallel Interface
	Set_VSL(0xA0);						// Enable External VSL
	Set_Contrast_Color(0xC8,0x80,0xC8); // Set Contrast of Color A (Red)
										// Set Contrast of Color B (Green)
										// Set Contrast of Color C (Blue)
	Set_Master_Current(Brightness); 	// Set Scale Factor of Segment Output Current Control
	Set_Gray_Scale_Table(); 			// Set Pulse Width for Gray Scale Table
	Set_Phase_Length(0x32); 			// Set Phase 1 as 5 Clocks & Phase 2 as 3 Clocks
	Set_Precharge_Voltage(0x17);		// Set Pre-Charge Voltage Level as 0.50*VCC
	Set_Display_Enhancement(0xA4);		// Enhance Display Performance
	Set_Precharge_Period(0x01); 		// Set Second Pre-Charge Period as 1 Clock
	Set_VCOMH(0x05);					// Set Common Pins Deselect Voltage Level as 0.82*VCC
	Set_Display_Mode(0x02); 			// Normal Display Mode (0x00/0x01/0x02/0x03)
	Fill_RAM(RGB565_Blue);					// Clear Screen
	Set_Display_On_Off(0x01);			// Display On (0x00/0x01)
#else
	Write_Command(0xFD);  // 解锁OLED驱动IC（SSD1351）的命令输入
	Write_Data(0x12);

	Write_Command(0xFD);  //设置 命令：A2,B1,B3,BB,BE 为可访问状态
	Write_Data(0xB1);

	Write_Command(0xAE);  //关显示（进入睡眠模式）

	Write_Command(0xB3);  //设置显示时钟分频&内部振荡器频率
	Write_Data(0xF1);     //时钟2分频，振荡器频率最高

	Write_Command(0xCA);  //设置 MUX Ratio
	Write_Data(0x5F);      //0x5F = 95; 0x7F = 127

	Write_Command(0xA2);  //设置显示偏移
	Write_Data(0x00);   //无偏移

	Write_Command(0xA1);  //设置显示起始行
	Write_Data(0x00);   //0x00

	Write_Command(0xA0);  //设置重映射格式
	/*
	0x62 -> BGR => ABC
	0x66 -> RGB => CBA
	*/
	Write_Data(0x66);		//把屏幕旋转了180度
	//Write_Data(0x74);     //水平地址增长模式、列地址0映射到SEG0、颜色数据传输顺序D[15:0] = [RRRRR:GGGGGG:BBBBB]、扫描方向：COM127~COM0、使能奇偶分离、颜色模式65K

	Write_Command(0xB5);  //GPIO0&GPIO1设置
	Write_Data(0x00);   //GPIO0&GPIO1设为高阻抗（硬件电路中两引脚没有连接）

	Write_Command(0xAB);  //功能选择设置
	Write_Data(0x01);   //使能VDD内部稳压器、选择8位并行接口

	Write_Command(0xB4);  //VSL设置
	Write_Data(0xA0);   //使能外部VSL
	Write_Data(0xB5);
	Write_Data(0x55);

	Write_Command(0xC1);  //设置色彩的对比电流
	/*
	0x62
	A - B
	B - G
	C - R
	0x66
	A - R
	B - G
	C - B
	*/
	#if 0
	Write_Data(0xC8);   //A-->Red
	Write_Data(0x80);   //B-->Green
	Write_Data(0xC8);   //C-->Blue
	#else
	Write_Data(0x80);   //A-->Red
	Write_Data(0xc8);   //B-->Green
	Write_Data(0xc8);   //C-->Blue
	#endif

	Write_Command(0xC7);  //主对比度电流控制
	Write_Data(0x0f);   //0x0f ? 0xFF?

	Set_Gray_Scale_Table(); // 设置灰度表脉冲宽度

	Write_Command(0xB1);  //设置阶段1（复位阶段）&阶段2（预充电）的周期
	Write_Data(0x32);   //阶段1为5个时钟周期、阶段2为3个时钟周期

	Write_Command(0xBB);  //设置预充电电压
	Write_Data(0x17);   //0.5*VCC

	#if 1
	Write_Command(0xB2);	// Enhance Display Performance
	Write_Data(0xA4);
	Write_Data(0);
	Write_Data(0);
	#endif

	Write_Command(0xB6);  //设置Second预充周期
	Write_Data(0x01);   //1个时钟周期（1DCLKS）

	Write_Command(0xBE);  //设置VCOMH电压
	Write_Data(0x05);   //0.82*VCC

	Write_Command(0xA6);  //设置显示模式：正常模式

	Fill_RAM(RGB565_Navy);//RGB565_Green);  //清屏

	Write_Command(0xAF);  //开显示
#endif
}

#endif

