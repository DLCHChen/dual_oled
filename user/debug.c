/*
CFP
2022 - 
*/

#include "stm32f10x.h"

#define SELF_TEST
#include "debug.h"
#include "board_cfg.h"

const char HexTable[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
unsigned char debug_rx_buf[DEBUG_BUF_LEN];

u8 debug_rx_cnt;
u8 debug_rx_cmd_done;

//使用SysTick的普通计数模式对延迟进行管理
//包括delay_us,delay_ms 
static u32 fac_us = 0;  //us延时倍乘数
static u32 fac_ms = 0;  //ms延时倍乘数

//初始化延迟函数
void delay_init(u32 SYSCLK)
{
	/* SYSCLK   = HCLK/1000000 */
	//SysTick->CTRL &= 0xfffffffb; //选择内部时钟 HCLK/8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);    //选择外部时钟  HCLK/8
	fac_us = SYSCLK / 8;     
	fac_ms = fac_us * 1000;
}

//延时Nms
//注意Nms的范围
//Nms<=0xffffff*8/SYSCLK
//对72M条件下,Nms<=1864
void delay_ms(u32 Nms)
{   
	SysTick->LOAD = Nms * fac_ms;   //时间加载 
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;               //开始倒数   
	while(!(SysTick->CTRL & (1<<16)))
	{
		;   //等待时间到达
	}
	SysTick->CTRL &= 0XFFFFFFFE;         //关闭计数器
	SysTick->VAL = 0X00000000;           //清空计数器    
}

//延时us          
void delay_us(u32 Nus)
{ 
	SysTick->LOAD = Nus*fac_us;          //时间加载     
	SysTick->CTRL |= 0x01;               //开始倒数   
	while(!(SysTick->CTRL & (1<<16)))
		;   //等待时间到达
	SysTick->CTRL = 0X00000000;          //关闭计数器
	SysTick->VAL = 0X00000000;           //清空计数器    
}

void dbgu_send_char_uart(uint8_t data)
{
	/* Write a character to the UART1 */
	USART_SendData(USART1, (uint16_t)data);
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
		;
	}
}

void dbg_uart_print_str(char *pd)
{
	int i;
	i = 0;
	while((*pd)!='\0') //发送字符串，直到遇到0才结束
	{
		//dbgu_send_char(*pd);
		dbgu_send_char_uart(*pd);
		pd++;  //移动到下一个字符
		i++;
		if(i > 256)
			break;
	}
}

uint8_t char2dec(uint8_t data)
{
	if(data >= '0' && data <= '9')
		return (data-'0');
	else if(data >= 'a' && data <= 'f')
		return ((data-'a') + 10);
	else
		return 0;
}

#if 1
void dbgu_PrintLongIntHex(uint32_t x)
{
	uint8_t i;
	char display_buffer[11];
	display_buffer[10]=0;
	display_buffer[0]='0';
	display_buffer[1]='x';
	for(i=9;i>=2;i--) //将整数转换为4个字节的HEX值
	{
		display_buffer[i]=HexTable[(x&0xf)];
		x>>=4;
	}
	dbg_uart_print_str(display_buffer);
}

void dbgu_PrintHex(uint8_t x)
{
	dbgu_send_char_uart('0');
	dbgu_send_char_uart('x');
	dbgu_send_char_uart(HexTable[x>>4]);
	dbgu_send_char_uart(HexTable[x&0xf]);
	//dbgu_send_char_uart(' ');
}
#endif
void dbgu_PrintLongInt(int x)
{
	int y;
	int8_t i;
	uint8_t display_buffer[12];

	if(x < 0)
		y = -x;
	else
		y = x;

	for(i=11;i>=0;i--)
	{
		display_buffer[i]='0'+y%10;
		y/=10;
		if(y == 0)
			break;
	}
	if(x < 0)
		dbgu_send_char_uart('-');

	for(;i<12;i++)
		dbgu_send_char_uart(display_buffer[i]);
}

void debug_data_in(uint8_t data)
{
	if(data == '\r')
	{
		dbgu_send_char_uart('\n');
		debug_rx_cmd_done = 1;
		debug_rx_buf[debug_rx_cnt] = 0;
		//debug_rx_cnt = 0;
	}
	else if(data != '\b')
	{
		debug_rx_buf[debug_rx_cnt] = data;
		debug_rx_cnt++;
		if(debug_rx_cnt >= DEBUG_BUF_LEN)
			debug_rx_cnt = 0;
	}
	dbgu_send_char_uart(data);
	if(data == '\b')
	{
		if(debug_rx_cnt)
			debug_rx_cnt--;
		dbgu_send_char_uart(' ');
		dbgu_send_char_uart(data);
	}
}



