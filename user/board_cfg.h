/*
CFP
2022 - 
*/


#ifndef _BOARD_CFG_H_
#define _BOARD_CFG_H_

#include "stm32f10x.h"

/*
LED --- PB0,PC13
*/
//PA2
#define MIC_GPIO_PORT  GPIOA
#define MIC_GPIO_PIN  GPIO_Pin_2
//PB10
#define I2C_SCL_GPIO_PORT  GPIOB
#define I2C_SCL_GPIO_PIN  GPIO_Pin_10
//PB11
#define I2C_SDA_GPIO_PORT  GPIOB
#define I2C_SDA_GPIO_PIN  GPIO_Pin_11

//PA4
#define SPI_CS_GPIO_PORT  GPIOA
#define SPI_CS_GPIO_PIN   GPIO_Pin_4
//PA3  - RS or DC
#define SPI_DC_GPIO_PORT  GPIOA
#define SPI_DC_GPIO_PIN   GPIO_Pin_3
//PA7
#define SPI_DATA_GPIO_PORT  GPIOA
#define SPI_DATA_GPIO_PIN   GPIO_Pin_7
//PA5
#define SPI_CLK_GPIO_PORT  GPIOA
#define SPI_CLK_GPIO_PIN   GPIO_Pin_5
//PB1
#define SPI_RES_GPIO_PORT  GPIOB
#define SPI_RES_GPIO_PIN   GPIO_Pin_1

//PB0
#define SPI_BLK_GPIO_PORT  GPIOB
#define SPI_BLK_GPIO_PIN   GPIO_Pin_0

//PA11
#define KEY_GPIO_PORT  GPIOA
#define KEY_GPIO_PIN  GPIO_Pin_11

//PB8
#define IIC_RES_GPIO_PORT  GPIOB
#define IIC_RES_GPIO_PIN   GPIO_Pin_8

//#define ADC_NORMAL_MODE		1
#define ADC_TIM2_DMA_MODE   1

#ifdef ADC_NORMAL_MODE
#define USING_CONTINUE_MODE    1
#endif

//#define CPU_FREQ_24MHZ		1
//#define CPU_FREQ_32MHZ		1
//#define CPU_FREQ_36MHZ		1
//#define CPU_FREQ_48MHZ		1
//#define CPU_FREQ_56MHZ		1
#define CPU_FREQ_72MHZ		1

//#define PCLK1_FRQR_16MHZ		1
//#define PCLK1_FRQR_24MHZ     1
//#define PCLK1_FRQR_32MHZ		1
//#define PCLK1_FRQR_48MHZ		1
//#define PCLK1_FRQR_56MHZ		1
#define PCLK1_FRQR_72MHZ		1

//#define FFT_SAMPLE_FREG_2P56KHZ	1
//#define FFT_SAMPLE_FREG_6P4KHZ	1
//#define FFT_SAMPLE_FREG_12P8KHZ	1
//#define FFT_SAMPLE_FREG_16KHZ	    1
//#define FFT_SAMPLE_FREG_19P2KHZ	    1
#define FFT_SAMPLE_FREG_25P6KHZ   1
//#define FFT_SAMPLE_FREG_38P4KHZ	    1

//#define FFT_SAMPLE_FREG_20KHZ	    1
//#define FFT_SAMPLE_FREG_32KHZ  	    1
//#define FFT_SAMPLE_FREG_64KHZ	    1
//#define FFT_SAMPLE_FREG_128KHZ	1
//#define FFT_SAMPLE_FREG_256KHZ	1


//#define DISP_FRAME_RATIO_40HZ	1
//#define DISP_FRAME_RATIO_30HZ	1
//#define DISP_FRAME_RATIO_25HZ	1
#define DISP_FRAME_RATIO_20HZ	1
//#define DISP_FRAME_RATIO_16HZ	1
//#define DISP_FRAME_RATIO_15HZ	1
//#define DISP_FRAME_RATIO_10HZ	1
//#define DISP_FRAME_RATIO_5HZ	1

#ifdef DISP_FRAME_RATIO_40HZ
#define ONE_SECOND_CNT_NUM     40
#endif

#ifdef DISP_FRAME_RATIO_30HZ
#define ONE_SECOND_CNT_NUM     30
#endif

#ifdef DISP_FRAME_RATIO_25HZ
#define ONE_SECOND_CNT_NUM     25
#endif

#ifdef DISP_FRAME_RATIO_20HZ
#define ONE_SECOND_CNT_NUM     20
#endif

#ifdef DISP_FRAME_RATIO_16HZ
#define ONE_SECOND_CNT_NUM     16
#endif

#ifdef DISP_FRAME_RATIO_15HZ
#define ONE_SECOND_CNT_NUM     15
#endif

#ifdef DISP_FRAME_RATIO_10HZ
#define ONE_SECOND_CNT_NUM     10
#endif

#ifdef DISP_FRAME_RATIO_5HZ
#define ONE_SECOND_CNT_NUM     5
#endif

#define OLED_USE_IIC		    1

#define OLED_USE_SPI		    1

#define OLED_SPI_CS_SOFT		1
//#define OLED_SPI_ONLY_TX		1
//#define NEW_SSD1351_INIT        1
#define OLED_SPI_DATA_NO_CHK	1

#ifdef OLED_USE_SPI
#define OLED_USE_HW_SPI		    1
#define SPI_OLED_0P96_BW_PANEL		1
//#define OLED_1P3_COLOR_PANEL	1		//Display frame ratio 36MHz - NG; 18MHz - 9MHz - OK
//#define OLED_0P96_COLOR_PANEL	1		//Display frame ratio ??? ST7735s color 0.96 80x160 RGB565
#endif

#ifdef OLED_USE_IIC
//#define OLED_USE_HW_IIC		1		//max. iic speed is 400KHz, so a frame reflash need 100ms!!!
#define OLED_0P96_BW_PANEL		1
#endif

#define ENABLE_KEYIN     1


#endif


