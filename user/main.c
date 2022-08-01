/*
*/
#include "stm32f10x.h"
#include "board_cfg.h"
#include "debug.h"
#include "led.h"
#include "tim.h"

#include "misc.h"

#include "user_adc.h"

#include "ex_key_proc.h"

#include "iic_oled.h"
#include "spi_oled.h"

#include "disp_show.h"

uint16_t tim2_cnt1;
uint16_t tim2_cnt2;
uint16_t tim2_cnt3;
uint16_t tim2_cnt4;

uint16_t tim3_ud_cnt;

extern u8 debug_rx_cnt;
extern u8 debug_rx_cmd_done;

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
							RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//使能AFIO时钟。重映射必须使能AFIO时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				     //LED --- PB0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		 //LED --- PC13
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//key
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				     //PA11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

#ifndef OLED_USE_HW_IIC
	/* Configure I2C2 pins: PB10->SCL and PB11->SDA */
	//PB8 as iic reset pin
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif

#ifndef OLED_USE_HW_SPI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure PA04 as Output push-pull, used as	Chip select */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_4);			//SPI1 NSS

	/* Configure PA03  as Output push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_3);

	/* Configure PB00, PB01 as Output push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, (GPIO_Pin_0 | GPIO_Pin_1));
#endif
	//usart1_init----------------------------------------------------
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
Group0 > Group1 > Group2 > Group3 > Group4

Group > PreemptionPriority > SubPriority

Group 0:
PreemptionPriority 0
SubPriority 16		      [0 ~ 15]

Group 1:
PreemptionPriority 2	  [0 ~ 1]
SubPriority 8             [0 ~ 7]

Group 2:
PreemptionPriority 4      [0 ~ 3] 
SubPriority 4             [0 ~ 3]

Group 3:
PreemptionPriority 8      [0 ~ 7]
SubPriority 2             [0 ~ 1]

Group 4:
PreemptionPriority 16     [0 ~ 15]
SubPriority 0
*/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
#ifdef ADC_TIM2_DMA_MODE
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
#endif
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
#if 1
	/* Enable the TIM2 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
#if 1
	/* Enable the TIM2 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

void SysTickInit(void)
{
	/* SystemFrequency / 1000    1ms
	* SystemFrequency / 100000  10us
	* SystemFrequency / 1000000 1us
	*/
	#if 1
	if ( SysTick_Config(SystemCoreClock / 1000000) )
	{
		/* Capture error */
		while (1);
	}
	//关闭滴答定时器 
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	#else
    RCC_ClocksTypeDef rccClk = {0};
    RCC_GetClocksFreq(&rccClk);
    SysTick_Config(rccClk.HCLK_Frequency / 1000);	//时基1ms
	#endif
}

void USART1_InitConfig(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Configure USART1 */
	USART_Init(USART1, &USART_InitStructure);

	/* Enable the USART1 */
	USART_Cmd(USART1, ENABLE);

	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

int main(void)
{
	u16 icolor;
	SystemCoreClockUpdate();
	GPIO_Configuration();
	NVIC_Configuration();
	#ifdef CPU_FREQ_72MHZ
	delay_init(72);
	#endif
	#ifdef CPU_FREQ_48MHZ
	delay_init(48);
	#endif
	#ifdef CPU_FREQ_56MHZ
	delay_init(56);
	#endif
	USART1_InitConfig(115200);
	dbg_uart_print_str(" DOLED_0.5A\r\n");		//uart1
	//dbgu_PrintLongInt(SystemCoreClock);
	//dbg_uart_print_str("\r\n");
	tim2_cnt2 = 0;
	tim2_cnt3 = 0;
	debug_rx_cmd_done = 0;
	debug_rx_cnt = 0;
	tim3_ud_cnt = 0;
	
#ifdef ADC_NORMAL_MODE
	user_adc_Init();
	TIM2_fixed_freq_Init();
#endif
#ifdef ADC_TIM2_DMA_MODE
	TIM2_4_ADC_Init();
	app_c_adc_dma_init();
	user_adc_Init();
#endif
#ifdef OLED_USE_SPI
	Spi_OLED_Init();
#endif
#ifdef OLED_USE_IIC
	IIC_OLED_Init();
#endif
#ifdef SPI_OLED_0P96_BW_PANEL
	SPI_OLED_ColorTurn(0);//0正常显示，1 反色显示
	SPI_OLED_DisplayTurn(1);//0正常显示 1 屏幕翻转显示
#endif
#ifdef OLED_0P96_BW_PANEL
	OLED_ColorTurn(0);//0正常显示，1 反色显示
	OLED_DisplayTurn(1);//0正常显示 1 屏幕翻转显示
	//dbg_uart_print_str("Showing.\r\n");
#endif
	TIM3_fixed_freq_Init();
	TIM4_Init();
	disp_show_init();
	TIM3_Set(1);
	while(1)
	{
		if(tim3_ud_cnt > 0)
		{
			tim3_ud_cnt = 0;
			if(Get_disp_run_st() == DISP_RUN_ST_NONE)
			{
				is_disp_show_period = 1;
				//dbg_uart_print_str("T3x\r\n");
			}
		}
		if(is_disp_show_period)
		{
			disp_shwo_loop_st_machine();
			//dbg_uart_print_str("T4a\r\n");
			//is_disp_show_period = 0;
		}
		#ifdef ENABLE_KEYIN
		key_in_proc();
		#endif
	}
    return 0;
}



