/*
*/
#include "board_cfg.h"
#include "led.h"

void LED1_Open(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

void LED1_Close(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

void LED1_Toggle(void)
{
	uint8_t val;
	val = GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_0);
	if(val)
		val = 0;
	else
		val = 1;
	GPIO_WriteBit(GPIOB,GPIO_Pin_0,(BitAction)val);
}

void LED2_Open(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void LED2_Close(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void LED2_Toggle(void)
{
	uint8_t val;
	val = GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13);
	if(val)
		val = 0;
	else
		val = 1;
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,(BitAction)val);
}




