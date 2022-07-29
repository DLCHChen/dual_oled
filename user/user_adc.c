/*
CFP
2022 - 
*/


#include "stm32f10x.h"
#include "board_cfg.h"
#include "debug.h"
#include "led.h"
#include "user_adc.h"
#include "disp_show.h"

u16 adc1_ok;

#ifdef ADC_NORMAL_MODE
void user_adc_Init(void)
{
	ADC_InitTypeDef  ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	// PA2 --- ADC1 channel2
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	ADC_InitStructure.ADC_Mode              = ADC_Mode_Independent;  //独立模式
	ADC_InitStructure.ADC_ScanConvMode      = DISABLE;      //连续多通道模式
	#ifdef USING_CONTINUE_MODE
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;;      //连续转换
	#else
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;      //不连续转换
	#endif
	ADC_InitStructure.ADC_ExternalTrigConv  = ADC_ExternalTrigConv_None; //转换不受外界决定
	ADC_InitStructure.ADC_DataAlign         = ADC_DataAlign_Right;   //右对齐
	ADC_InitStructure.ADC_NbrOfChannel      = 1;       //扫描通道数
	ADC_Init(ADC1,&ADC_InitStructure);
	#ifdef CPU_FREQ_72MHZ
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);   //设置ADC分频因子8   72M/8=9,ADC最大时间不能超过14M
	#endif
	#ifdef CPU_FREQ_48MHZ
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);   //设置ADC分频因子4   48M/8=6,ADC最大时间不能超过14M
	#endif
	#ifdef CPU_FREQ_56MHZ
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);   //设置ADC分频因子8   56M/8=7,ADC最大时间不能超过14M
	#endif

	//ADC_RegularChannelConfig(ADC1,ADC_Channel_2, 1,ADC_SampleTime_55Cycles5);
	//ADC_RegularChannelConfig(ADC1,ADC_Channel_2, 1,ADC_SampleTime_28Cycles5);
	//ADC_RegularChannelConfig(ADC1,ADC_Channel_2, 1,ADC_SampleTime_13Cycles5);
	#ifdef USING_CONTINUE_MODE
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2, 1,ADC_SampleTime_28Cycles5);
	#else
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2, 1,ADC_SampleTime_13Cycles5);
	#endif

	ADC_Cmd(ADC1,ENABLE);				//使能或者失能指定的ADC

	ADC_ResetCalibration(ADC1);			//使能复位校准  
	while(ADC_GetResetCalibrationStatus(ADC1))
	{
		; //等待复位校准结束 
	}
	ADC_StartCalibration(ADC1); //开启AD校准 
	while(ADC_GetCalibrationStatus(ADC1))
	{
		; //等待校准结束
	}
}
#endif

#ifdef ADC_TIM2_DMA_MODE
void user_adc_Init(void)
{
	ADC_InitTypeDef  ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	// PA2 --- ADC1 channel2
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	ADC_InitStructure.ADC_Mode              = ADC_Mode_Independent;  //独立模式
	ADC_InitStructure.ADC_ScanConvMode      = DISABLE;      //连续多通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;      //连续转换
	ADC_InitStructure.ADC_ExternalTrigConv  = ADC_ExternalTrigConv_T2_CC2;//ADC_ExternalTrigConv_None; //转换不受外界决定
	ADC_InitStructure.ADC_DataAlign         = ADC_DataAlign_Right;   //右对齐
	ADC_InitStructure.ADC_NbrOfChannel      = 1;       //扫描通道数
	ADC_Init(ADC1,&ADC_InitStructure);
	#ifdef CPU_FREQ_72MHZ
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);   //设置ADC分频因子8   72M/8=9,ADC最大时间不能超过14M
	#endif
	#ifdef CPU_FREQ_48MHZ
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);   //设置ADC分频因子4   48M/8=6,ADC最大时间不能超过14M
	#endif
	#ifdef CPU_FREQ_56MHZ
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);   //设置ADC分频因子8   56M/8=7,ADC最大时间不能超过14M
	#endif

	//ADC_RegularChannelConfig(ADC1,ADC_Channel_2, 1,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2, 1,ADC_SampleTime_28Cycles5);

	ADC_Cmd(ADC1,ENABLE);				//使能或者失能指定的ADC

	ADC_ResetCalibration(ADC1);			//使能复位校准  
	while(ADC_GetResetCalibrationStatus(ADC1))
	{
		; //等待复位校准结束 
	}
	ADC_StartCalibration(ADC1); //开启AD校准 
	while(ADC_GetCalibrationStatus(ADC1))
	{
		; //等待校准结束
	}
}

#endif

void app_c_adc_start(void)
{
	adc1_ok = 0;
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //连续转换开始
	DMA_Cmd(DMA1_Channel1,ENABLE);
	ADC_DMACmd(ADC1, ENABLE); //开启ADC1 DMA采集
}

void app_c_adc_stop(void)
{
	DMA_Cmd(DMA1_Channel1,DISABLE);
	ADC_DMACmd(ADC1,DISABLE);
	/* Stop ADC1 Software Conversion */ 
	//ADC_SoftwareStartConvCmd(ADC1, DISABLE); //连续转换开始，ADC通过DMA方式不断的更新RAM区。
	//DMA_DeInit(DMA1_Channel1);
	//ADC_Cmd(ADC1,DISABLE);
	//ADC_DeInit(ADC1);
}

void TIM2_4_ADC_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
	u16 u16Prescaler, u16ArrValue;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
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

    //定时器TIM2初始化
    TIM_TimeBaseStructure.TIM_Period = u16ArrValue; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值    
    TIM_TimeBaseStructure.TIM_Prescaler = u16Prescaler; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
//当外部触发信号被选为ADC规则或注入转换时，只有它的上升沿可以启动转换。这跟下面的定时器2的正确配置关系很大。
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = u16ArrValue/2;    // 50%duty PWM
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    //TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //使能指定的TIM2中断,允许更新中断  不需要中断。是用定时器的PWM触发
    /* TIM IT enable */
	//TIM_ITConfig(TIM2,  TIM_IT_CC2, ENABLE);   //for test use.
    TIM_OC2Init(TIM2, & TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	#if 0
    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	#endif
    //TIM_Cmd(TIM2, ENABLE);  //使能TIMx                     
}

void app_c_adc_dma_start(void)
{
	adc1_ok = 0;
	/* Start ADC1 Software Conversion */ 
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE); //连续转换开始，ADC通过DMA方式不断的更新RAM区。
	#if 0
	ADC1->CR2 |= CR2_EXTTRIG_Set;
	DMA1_Channel1->CCR |= DMA_CCR1_EN;
	ADC1->CR2 |= CR2_DMA_Set;
	TIM2->CR1 |= TIM_CR1_CEN;
	#else
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);  //使能外部定时器触发 
	DMA_Cmd(DMA1_Channel1,ENABLE);
	ADC_DMACmd(ADC1, ENABLE); //开启ADC1 DMA采集
	TIM_Cmd(TIM2, ENABLE);  //使能TIMx 
	#endif
}

void app_c_adc_dma_init(void)
{
	DMA_InitTypeDef dma;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel1);
	dma.DMA_PeripheralBaseAddr  = (u32)&(ADC1->DR);					//DMA对应的外设基地址
	dma.DMA_MemoryBaseAddr      = (u32)FFT_data;					//内存存储基地址
	dma.DMA_DIR                 = DMA_DIR_PeripheralSRC;			//DMA的转换模式为SRC模式，由外设搬移到内存
	dma.DMA_M2M                 = DMA_M2M_Disable;					//M2M模式禁用
	dma.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_HalfWord;	//定义外设数据宽度为16位
	dma.DMA_MemoryDataSize      = DMA_MemoryDataSize_HalfWord;		//DMA搬移数据尺寸，HalfWord就是为16位    
	dma.DMA_BufferSize          = MIC_ADC_SAMPLE_CNT;								//DMA缓存大小CH_NUM个
	dma.DMA_MemoryInc           = DMA_MemoryInc_Enable;				//接收一次数据后，目标内存地址后移
	dma.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;		//接收一次数据后，设备地址禁止后移
	//转换模式：常用循环缓存模式。如果M2M开启了，则这个模式失效
    //另一种是Normal模式：不循环，仅一次DMA
	dma.DMA_Mode                = DMA_Mode_Circular;//DMA_Mode_Normal;    //DMA_Mode_Circular;				//转换模式，循环缓存模式。
	dma.DMA_Priority            = DMA_Priority_High;				//DMA优先级高
	DMA_Init(DMA1_Channel1,&dma); 
	DMA_ClearITPendingBit(DMA1_IT_TC1);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);                //开启DMA1CH1中断
	//DMA_Cmd(DMA1_Channel1, ENABLE);
}

void app_c_adc_dma_stop(void)
{
	#if 0
	TIM2->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
	DMA1_Channel1->CCR &= (uint16_t)(~DMA_CCR1_EN);
	ADC1->CR2 &= CR2_DMA_Reset;
	ADC1->CR2 &= CR2_EXTTRIG_Reset;
	#else
	TIM_Cmd(TIM2, DISABLE);  //使能TIMx 
	DMA_Cmd(DMA1_Channel1,DISABLE);
	ADC_DMACmd(ADC1,DISABLE);
	ADC_ExternalTrigConvCmd(ADC1, DISABLE);  //使能外部定时器触发 
	#endif
}

void TIM3_4_ADC_Init(u16 SampleRate) 
{ 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	//NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = SystemCoreClock/SampleRate;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update); //选择TRGO触发源为计时器更新时间 
#if 0
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
#endif
	TIM_Cmd(TIM3,ENABLE);
}




