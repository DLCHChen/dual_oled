/*
CFP
2022 - 
*/

#ifndef _TIM_H_
#define _TIM_H_

#include "stm32f10x.h"


void TIM2_fixed_freq_Init(void);

void TIM4_Init(void);

void TIM3_Set(u8 Sta);
void TIM3_fixed_freq_Init(void);

#endif

