/*
CFP
2022 - 
*/


#ifndef __USER_ADC_H__
#define __USER_ADC_H__

void user_adc_Init(void);

void TIM2_4_ADC_Init(void);

void app_c_adc_dma_start(void);
void app_c_adc_dma_init(void);
void app_c_adc_dma_stop(void);

#endif

