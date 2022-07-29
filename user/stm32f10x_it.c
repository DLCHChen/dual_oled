/**
  ******************************************************************************
  * @file    GPIO/JTAG_Remap/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

#include "board_cfg.h"
#include "debug.h"
#include "led.h"
#include "tim.h"
#include "disp_show.h"
#include "user_adc.h"


extern __IO uint16_t TIM2_CCR1_Val;
extern __IO uint16_t TIM2_CCR2_Val;
extern __IO uint16_t TIM2_CCR3_Val;
extern __IO uint16_t TIM2_CCR4_Val;

extern uint16_t tim2_cnt1;
extern uint16_t tim2_cnt2;
extern uint16_t tim2_cnt3;
extern uint16_t tim2_cnt4;

extern uint16_t tim3_ud_cnt;
extern u16 adc1_ok;
u16 u16AdcResult;

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_JTAG_Remap
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t input_char;
uint16_t capture;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		//USART_ClearITPendingBit(USART1,USART_IT_RXNE); // 清中断标志
		/* Read one byte from the receive data register */
		input_char = (uint8_t)(USART_ReceiveData(USART1)&0xFF);
		debug_data_in(input_char);
	}
#if 0
	if(USART_GetITStatus(USARTy, USART_IT_TXE) != RESET)
	{
	}
#endif
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		tim2_cnt2++;
		// adc sample
		#ifdef ADC_NORMAL_MODE
		if(fft_sample_cnt < MIC_ADC_SAMPLE_CNT)
		{
			//if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
			{
				u16AdcResult = ADC_GetConversionValue(ADC1);
			}
			FFT_data[fft_sample_cnt] = u16AdcResult;
			#ifndef USING_CONTINUE_MODE
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			#endif
			fft_sample_cnt++;
		}
		else
		{
			mic_adc_start = 0;
			//stop Timer2
			TIM_Cmd(TIM2, DISABLE);  //使能TIMx
			ADC_SoftwareStartConvCmd(ADC1, DISABLE);
			//ADC_Cmd(ADC1,DISABLE);
			fft_sample_cnt = 0;
		}
		#endif
	}
	else if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		/* Clear TIM2 Capture Compare1 interrupt pending bit*/
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		tim2_cnt1++;
		capture = TIM_GetCapture1(TIM2);
		TIM_SetCompare1(TIM2, capture + TIM2_CCR1_Val );
	}
	else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
	{
		/* Clear TIM2 Capture Compare2 interrupt pending bit*/
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
		#ifndef ADC_TIM2_DMA_MODE
		tim2_cnt2++;
		capture = TIM_GetCapture2(TIM2);
		TIM_SetCompare2(TIM2, capture + TIM2_CCR2_Val );
		#endif
	}
	else if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
	{
		/* Clear TIM2 Capture Compare3 interrupt pending bit*/
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
		tim2_cnt3++;
		capture = TIM_GetCapture3(TIM2);
		TIM_SetCompare3(TIM2, capture + TIM2_CCR3_Val );
	}
	else if (TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET)
	{
		/* Clear TIM2 Capture Compare4 interrupt pending bit*/
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
		tim2_cnt4++;
		capture = TIM_GetCapture4(TIM2);
		TIM_SetCompare4(TIM2, capture + TIM2_CCR4_Val );
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//是更新中断
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志 
		tim3_ud_cnt++;
	}
}

void DMA1_Channel1_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
    {
        adc1_ok = 1;
		mic_adc_start = 0;
		app_c_adc_dma_stop();
        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }
}

