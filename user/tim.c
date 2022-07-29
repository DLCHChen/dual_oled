/*
*/
#include "board_cfg.h"
#include "tim.h"
#include "debug.h"

#define TIM2_CCR1_Address 0x40000034
#define TIM2_CCR2_Address 0x40000038
#define TIM2_CCR3_Address 0x4000003C
#define TIM2_CCR4_Address 0x40000040
#define TIM3_CCR1_Address 0x40000434
#define TIM3_CCR2_Address 0x40000438
#define TIM3_CCR3_Address 0x4000043C
#define TIM3_CCR4_Address 0x40000440
#define TIM4_CCR1_Address 0x40000834
#define TIM4_CCR2_Address 0x40000838
#define TIM4_CCR3_Address 0x4000083C
#define TIM4_CCR4_Address 0x40000840

__IO uint16_t TIM1_CCR1_Val;

__IO uint16_t TIM2_CCR1_Val;
__IO uint16_t TIM2_CCR2_Val;
__IO uint16_t TIM2_CCR3_Val;
__IO uint16_t TIM2_CCR4_Val;

__IO uint16_t TIM4_CCR1_Val;

void TIM2_fixed_freq_Init(void)
{
	//NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	u16 u16Prescaler, u16ArrValue;
#ifdef PCLK1_FRQR_56MHZ
	/*
	56MHz / (5599+1) = 10KHz
	10KHz / (99+1) = 100Hz ---> 10ms
	*/
	
#ifdef FFT_SAMPLE_FREG_20KHZ
	/*
	56MHz / 55+1) = 1MHz
	1MHz / (49+1) = 20KHz
	*/
	u16Prescaler = 55;
	u16ArrValue = 49;
#endif
		
#ifdef FFT_SAMPLE_FREG_2P56KHZ	
	/*
	56MHz / (6+1) = 8MHz
	8MHz / (3124+1) = 2.56KHz
	*/
	u16Prescaler = 6;
	u16ArrValue = 3124;
#endif
		
#ifdef FFT_SAMPLE_FREG_38P4KHZ
	/*
	56MHz / (1+1) = 28MHz
	28MHz / (728+1) = ~38.4KHz
	*/
	u16Prescaler = 1;
	u16ArrValue = 728;
#endif
		
#ifdef FFT_SAMPLE_FREG_25P6KHZ
	/*
	56MHz / (6+1) = 8MHz
	8MHz / (311+1) = ~25.6KHz
	*/
	u16Prescaler = 6;
	u16ArrValue = 311;
#endif
		
#ifdef FFT_SAMPLE_FREG_256KHZ
	/*
	56MHz / (1+1) = 28MHz
	28MHz / (108+1) = ~256.8KHz
	*/
	u16Prescaler = 1;
	u16ArrValue = 108;
#endif
		
#ifdef FFT_SAMPLE_FREG_12P8KHZ
	/*
	56MHz / (6+1) = 8MHz
	8MHz / (624+1) = 12.8KHz
	*/
	u16Prescaler = 6;
	u16ArrValue = 624;
#endif
		
#ifdef FFT_SAMPLE_FREG_19P2KHZ
	/*
	56MHz / (1+1) = 28MHz
	28MHz / (1457+1) = ~19.2KHz
	*/
	u16Prescaler = 1;
	u16ArrValue = 1457;
#endif
		
#ifdef FFT_SAMPLE_FREG_6P4KHZ
	/*
	56MHz / (13+1) = 4MHz
	4MHz / (624+1) = 6.4KHz
	*/
	u16Prescaler = 13;
	u16ArrValue = 624;
#endif
		
#ifdef FFT_SAMPLE_FREG_32KHZ
	/*
	56MHz / (13+1) = 4MHz
	4MHz / (124+1) = 32KHz
	*/
	u16Prescaler = 13;
	u16ArrValue = 124;
#endif
#ifdef	FFT_SAMPLE_FREG_16KHZ
	/*
	56MHz / (27+1) = 2MHz
	2MHz / (124+1) = 16KHz
	*/
	u16Prescaler = 27;
	u16ArrValue = 124;
#endif
#ifdef	FFT_SAMPLE_FREG_64KHZ
	/*
	56MHz / (6+1) = 8MHz
	8MHz / (124+1) = 64KHz
	*/
	u16Prescaler = 6;
	u16ArrValue = 124;
#endif
		
#ifdef FFT_SAMPLE_FREG_128KHZ
	/*
	56MHz / (1+1) = 28MHz
	28MHz / (217+1) = ~128.4KHz
	*/
	u16Prescaler = 1;
	u16ArrValue = 217;
#endif
			
#endif

#ifdef PCLK1_FRQR_48MHZ
	/*
	48MHz / (4799+1) = 10KHz
	10KHz / (99+1) = 100Hz ---> 10ms
	*/
	
#ifdef FFT_SAMPLE_FREG_20KHZ
	/*
	48MHz / (47+1) = 1MHz
	1MHz / (49+1) = 20KHz
	*/
	u16Prescaler = 47;
	u16ArrValue = 49;
#endif
		
#ifdef FFT_SAMPLE_FREG_2P56KHZ	
	/*
	48MHz / (5+1) = 8MHz
	8MHz / (3124+1) = 2.56KHz
	*/
	u16Prescaler = 5;
	u16ArrValue = 3124;
#endif
		
#ifdef FFT_SAMPLE_FREG_38P4KHZ
	/*
	48MHz / (1+1) = 24MHz
	24MHz / (624+1) = 38.4KHz
	*/
	u16Prescaler = 1;
	u16ArrValue = 624;
#endif
		
#ifdef FFT_SAMPLE_FREG_25P6KHZ
	/*
	48MHz / (5+1) = 8MHz
	8MHz / (311+1) = ~25.6KHz
	*/
	u16Prescaler = 5;
	u16ArrValue = 311;
#endif
		
#ifdef FFT_SAMPLE_FREG_256KHZ
	/*
	48MHz / (1+1) = 24MHz
	24MHz / (92+1) = ~258KHz
	*/
	u16Prescaler = 1;
	u16ArrValue = 93;
#endif
		
#ifdef FFT_SAMPLE_FREG_12P8KHZ
	/*
	48MHz / (5+1) = 8MHz
	8MHz / (624+1) = 12.8KHz
	*/
	u16Prescaler = 5;
	u16ArrValue = 624;
#endif
		
#ifdef FFT_SAMPLE_FREG_19P2KHZ
	/*
	48MHz / (3+1) = 12MHz
	12MHz / (624+1) = 19.2KHz
	*/
	u16Prescaler = 3;
	u16ArrValue = 624;
#endif
		
#ifdef FFT_SAMPLE_FREG_6P4KHZ
	/*
	48MHz / (11+1) = 4MHz
	4MHz / (624+1) = 6.4KHz
	*/
	u16Prescaler = 11;
	u16ArrValue = 624;
#endif
		
#ifdef FFT_SAMPLE_FREG_32KHZ
	/*
	48MHz / (11+1) = 4MHz
	4MHz / (124+1) = 32KHz
	*/
	u16Prescaler = 11;
	u16ArrValue = 124;
#endif
#ifdef	FFT_SAMPLE_FREG_16KHZ
	/*
	48MHz / (23+1) = 2MHz
	2MHz / (124+1) = 16KHz
	*/
	u16Prescaler = 23;
	u16ArrValue = 124;
#endif
#ifdef	FFT_SAMPLE_FREG_64KHZ
	/*
	48MHz / (5+1) = 8MHz
	8MHz / (124+1) = 64KHz
	*/
	u16Prescaler = 5;
	u16ArrValue = 124;
#endif
		
#ifdef FFT_SAMPLE_FREG_128KHZ
	/*
	48MHz / (1+1) = 24MHz
	24MHz / (186+1) = ~128KHz
	*/
	u16Prescaler = 1;
	u16ArrValue = 186;
#endif
		
#endif

#ifdef PCLK1_FRQR_72MHZ
/*
72MHz / (7199+1) = 10KHz
10KHz / (99+1) = 100Hz ---> 10ms
*/

#ifdef FFT_SAMPLE_FREG_20KHZ
	/*
	72MHz / (71+1) = 1MHz
	1MHz / (49+1) = 20KHz
	*/
	u16Prescaler = 71;
	u16ArrValue = 49;
#endif
		
#ifdef FFT_SAMPLE_FREG_2P56KHZ	
	/*
	72MHz / (8+1) = 8MHz
	8MHz / (3124+1) = 2.56KHz
	*/
	u16Prescaler = 8;
	u16ArrValue = 3124;
#endif
		
#ifdef FFT_SAMPLE_FREG_38P4KHZ
	/*
	72MHz / (2+1) = 24MHz
	24MHz / (624+1) = 38.4KHz
	*/
	u16Prescaler = 2;
	u16ArrValue = 624;
#endif
		
#ifdef FFT_SAMPLE_FREG_25P6KHZ
	/*
	72MHz / (8+1) = 8MHz
	8MHz / (311+1) = ~25.6KHz
	*/
	u16Prescaler = 8;
	u16ArrValue = 311;
#endif
		
#ifdef FFT_SAMPLE_FREG_256KHZ
	/*
	72MHz / (1+1) = 36MHz
	36MHz / (139+1) = ~257KHz
	*/
	u16Prescaler = 1;
	u16ArrValue = 139;
#endif
		
#ifdef FFT_SAMPLE_FREG_12P8KHZ
	/*
	72MHz / (8+1) = 8MHz
	8MHz / (624+1) = 12.8KHz
	*/
	u16Prescaler = 8;
	u16ArrValue = 624;
#endif
		
#ifdef FFT_SAMPLE_FREG_19P2KHZ
	/*
	72MHz / (5+1) = 12MHz
	12MHz / (624+1) = 19.2KHz
	*/
	u16Prescaler = 5;
	u16ArrValue = 624;
#endif
		
#ifdef FFT_SAMPLE_FREG_6P4KHZ
	/*
	72MHz / (17+1) = 4MHz
	4MHz / (624+1) = 6.4KHz
	*/
	u16Prescaler = 17;
	u16ArrValue = 624;
#endif
		
#ifdef FFT_SAMPLE_FREG_32KHZ
	/*
	72MHz / (17+1) = 4MHz
	4MHz / (124+1) = 32KHz
	*/
	u16Prescaler = 17;
	u16ArrValue = 124;
#endif
#ifdef	FFT_SAMPLE_FREG_16KHZ
	/*
	72MHz / (35+1) = 2MHz
	2MHz / (124+1) = 16KHz
	*/
	u16Prescaler = 35;
	u16ArrValue = 124;
#endif
#ifdef	FFT_SAMPLE_FREG_64KHZ
	/*
	72MHz / (8+1) = 8MHz
	8MHz / (124+1) = 64KHz
	*/
	u16Prescaler = 8;
	u16ArrValue = 124;
#endif
		
#ifdef FFT_SAMPLE_FREG_128KHZ
	/*
	72MHz / (1+1) = 36MHz
	36MHz / (280+1) = ~128KHz
	*/
	u16Prescaler = 1;
	u16ArrValue = 280;
#endif
	
#endif

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能//TIM2时钟使能    
	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = u16ArrValue; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = u16Prescaler; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

	TIM_ARRPreloadConfig(TIM2, ENABLE);
	/* TIM IT enable */
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断
}

void TIM4_Init(void)
{
	//NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	/*
	72MHz / (71+1) = 1MHz
	72MHz / (719+1) = 100KHz
	//1MHz / (99+1) = 100Hz ---> 10ms
	*/
	/*
	48MHz / (47+1) = 1MHz
	48MHz / (479+1) = 100KHz
	//1MHz / (99+1) = 100Hz ---> 10ms
	*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能//TIM3时钟使能    
	//定时器TIM4初始化
	TIM_TimeBaseStructure.TIM_Period = 0xffff; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	#ifdef PCLK1_FRQR_48MHZ
	TIM_TimeBaseStructure.TIM_Prescaler = 479; //设置用来作为TIMx时钟频率除数的预分频值
	#endif
	#ifdef PCLK1_FRQR_72MHZ
	TIM_TimeBaseStructure.TIM_Prescaler = 719; //设置用来作为TIMx时钟频率除数的预分频值
	#endif
	#ifdef PCLK1_FRQR_56MHZ
	TIM_TimeBaseStructure.TIM_Prescaler = 559; //设置用来作为TIMx时钟频率除数的预分频值
	#endif
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

	TIM_ARRPreloadConfig(TIM4, ENABLE);
	/* TIM IT enable */
	//TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断
	/* TIM4 enable counter */
	TIM_Cmd(TIM4, ENABLE);
}

void TIM3_Set(u8 Sta)
{
	if(Sta)
	{ 
		TIM_SetCounter(TIM3,0);//计数器清空
		TIM_Cmd(TIM3, ENABLE);  //使能TIMx	
	}
	else
		TIM_Cmd(TIM3, DISABLE);//关闭定时器3
}

void TIM3_fixed_freq_Init(void)
{
	//NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	u16 u16Prescaler, u16ArrValue;
#ifdef PCLK1_FRQR_56MHZ
	/*
	56MHz / (5599+1) = 10KHz
	10KHz / (99+1) = 100Hz ---> 10ms
	*/
#ifdef DISP_FRAME_RATIO_40HZ
	/*
	56MHz / (5599+1) = 10KHz
	10KHz / (249+1) = 40Hz
	*/
	u16Prescaler = 5599;
	u16ArrValue = 249;
#endif
#ifdef DISP_FRAME_RATIO_30HZ
	/*
	56MHz / (6999+1) = 8KHz
	8KHz / (266+1) = ~30Hz
	*/
	u16Prescaler = 6999;
	u16ArrValue = 266;
#endif

#ifdef DISP_FRAME_RATIO_25HZ
	/*
	56MHz / (5599+1) = 10KHz
	10KHz / (399+1) = 25Hz
	*/
	u16Prescaler = 5599;
	u16ArrValue = 399;
#endif

#ifdef DISP_FRAME_RATIO_20HZ
	/*
	56MHz / (5599+1) = 10KHz
	10KHz / (499+1) = 20Hz
	*/
	u16Prescaler = 5599;
	u16ArrValue = 499;
#endif
#ifdef DISP_FRAME_RATIO_16HZ
	/*
	56MHz / (6999+1) = 8KHz
	8KHz / (499+1) = 16Hz
	*/
	u16Prescaler = 6999;
	u16ArrValue = 499;
#endif
#ifdef DISP_FRAME_RATIO_15HZ
	/*
	56MHz / (6999+1) = 8KHz
	8KHz / (532+1) = ~15Hz
	*/
	u16Prescaler = 6999;
	u16ArrValue = 532;
#endif

#ifdef DISP_FRAME_RATIO_10HZ
	/*
	56MHz / (5599+1) = 10KHz
	10KHz / (999+1) = 10Hz
	*/
	u16Prescaler = 5599;
	u16ArrValue = 999;
#endif

#ifdef DISP_FRAME_RATIO_5HZ
	/*
	56MHz / (5599+1) = 10KHz
	10KHz / (1999+1) = 5Hz
	*/
	u16Prescaler = 5599;
	u16ArrValue = 1999;
#endif

#endif

#ifdef PCLK1_FRQR_48MHZ
	/*
	48MHz / (4799+1) = 10KHz
	10KHz / (99+1) = 100Hz ---> 10ms
	*/
#ifdef DISP_FRAME_RATIO_40HZ
	/*
	48MHz / (4799+1) = 10KHz
	10KHz / (249+1) = 40Hz
	*/
	u16Prescaler = 4799;
	u16ArrValue = 249;
#endif
#ifdef DISP_FRAME_RATIO_30HZ
	/*
	48MHz / (7999+1) = 6KHz
	6KHz / (199+1) = 30Hz
	*/
	u16Prescaler = 7999;
	u16ArrValue = 199;
#endif

#ifdef DISP_FRAME_RATIO_25HZ
	/*
	48MHz / (4799+1) = 10KHz
	10KHz / (399+1) = 25Hz
	*/
	u16Prescaler = 7199;
	u16ArrValue = 399;
#endif

#ifdef DISP_FRAME_RATIO_20HZ
	/*
	48MHz / (7199+1) = 10KHz
	10KHz / (499+1) = 20Hz
	*/
	u16Prescaler = 4799;
	u16ArrValue = 499;
#endif
#ifdef DISP_FRAME_RATIO_16HZ
	/*
	48MHz / (5999+1) = 8KHz
	8KHz / (499+1) = 16Hz
	*/
	u16Prescaler = 5999;
	u16ArrValue = 499;
#endif
#ifdef DISP_FRAME_RATIO_15HZ
	/*
	48MHz / (7999+1) = 6KHz
	6KHz / (399+1) = 15Hz
	*/
	u16Prescaler = 7999;
	u16ArrValue = 399;
#endif

#ifdef DISP_FRAME_RATIO_10HZ
	/*
	48MHz / (4799+1) = 10KHz
	10KHz / (999+1) = 10Hz
	*/
	u16Prescaler = 4799;
	u16ArrValue = 999;
#endif

#ifdef DISP_FRAME_RATIO_5HZ
	/*
	48MHz / (4799+1) = 10KHz
	10KHz / (1999+1) = 5Hz
	*/
	u16Prescaler = 4799;
	u16ArrValue = 1999;
#endif

#endif

#ifdef PCLK1_FRQR_72MHZ
/*
72MHz / (7199+1) = 10KHz
10KHz / (99+1) = 100Hz ---> 10ms
*/

#ifdef DISP_FRAME_RATIO_40HZ
	/*
	72MHz / (7199+1) = 10KHz
	10KHz / (249+1) = 40Hz
	*/
	u16Prescaler = 7199;
	u16ArrValue = 249;
#endif
#ifdef DISP_FRAME_RATIO_30HZ
	/*
	72MHz / (7999+1) = 9KHz
	9KHz / (299+1) = 30Hz
	*/
	u16Prescaler = 7999;
	u16ArrValue = 299;
#endif

#ifdef DISP_FRAME_RATIO_25HZ
	/*
	72MHz / (7199+1) = 10KHz
	10KHz / (399+1) = 25Hz
	*/
	u16Prescaler = 7199;
	u16ArrValue = 399;
#endif

#ifdef DISP_FRAME_RATIO_20HZ
	/*
	72MHz / (7199+1) = 10KHz
	10KHz / (499+1) = 20Hz
	*/
	u16Prescaler = 7199;
	u16ArrValue = 499;
#endif
#ifdef DISP_FRAME_RATIO_16HZ
	/*
	72MHz / (8999+1) = 8KHz
	8KHz / (499+1) = 16Hz
	*/
	u16Prescaler = 8999;
	u16ArrValue = 499;
#endif
#ifdef DISP_FRAME_RATIO_15HZ
	/*
	72MHz / (7999+1) = 9KHz
	9KHz / (599+1) = 15Hz
	*/
	u16Prescaler = 7999;
	u16ArrValue = 599;
#endif

#ifdef DISP_FRAME_RATIO_10HZ
	/*
	72MHz / (7199+1) = 10KHz
	10KHz / (999+1) = 10Hz
	*/
	u16Prescaler = 7199;
	u16ArrValue = 999;
#endif

#ifdef DISP_FRAME_RATIO_5HZ
	/*
	72MHz / (7199+1) = 10KHz
	10KHz / (1999+1) = 5Hz
	*/
	u16Prescaler = 7199;
	u16ArrValue = 1999;
#endif

#endif

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能//TIM3时钟使能    
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = u16ArrValue; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = u16Prescaler; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	/* TIM IT enable */
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断
}


