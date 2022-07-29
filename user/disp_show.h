/*
CFP
2022 - 
*/

#ifndef _DISP_SHOW_H_

#define _DISP_SHOW_H_

#include "board_cfg.h"

#define G_NUM_LEDS		256		//32	//128

#define FFT_COUNT_USE_32BIT            1
#define FFT_IN_DATA_CUT_HALF			1

#define MIC_ADC_SAMPLE_OFT	16
#define MIC_ADC_SAMPLE_CNT	(G_NUM_LEDS + MIC_ADC_SAMPLE_OFT)

#define MIC_ADC_OFFSET	2048   ////2032//2048

#define FFT_USED_DATA_START	0

#if (G_NUM_LEDS == 512)
#define FFT_USED_DATA_ST_END	127
#define FFT_USED_DATA_TT_END	128
#else
#define FFT_USED_DATA_ST_END	((G_NUM_LEDS/2) - 1)
#define FFT_USED_DATA_TT_END	(G_NUM_LEDS/2)    //((G_NUM_LEDS/2) - 1)
#endif

#define FFT_OVER_MIN_CNT_TH	4
#define FFT_OVER_EMN_CNT_TH	4

#define FFT_ROTATION_180D 	1

#ifdef DISP_SLEF_DEF
volatile s16 fft_sample_cnt;
s16 FFT_data[MIC_ADC_SAMPLE_CNT+4];
s16 FFT_out[G_NUM_LEDS/2+4];
s16 sFFT_out[G_NUM_LEDS/2+4];
s16 xFFT_out[G_NUM_LEDS/2+4];
s16 yFFT_out[G_NUM_LEDS/2+4];

#ifdef FFT_COUNT_USE_32BIT
s32 tFFT_data[G_NUM_LEDS+4];
#endif

volatile u8 mic_adc_start;
volatile u8 is_disp_show_period;

#else
extern volatile s16 fft_sample_cnt;
extern s16 FFT_data[MIC_ADC_SAMPLE_CNT+4];
extern s16 FFT_out[G_NUM_LEDS/2+4];
extern s16 sFFT_out[G_NUM_LEDS/2+4];
extern s16 xFFT_out[G_NUM_LEDS/2+4];
extern s16 yFFT_out[G_NUM_LEDS/2+4];

#ifdef FFT_COUNT_USE_32BIT
extern s32 tFFT_data[G_NUM_LEDS+4];
#endif

extern volatile u8 mic_adc_start;
extern volatile u8 is_disp_show_period;
#endif

typedef enum {
	DISP_RUN_ST_NONE,
	DISP_RUN_ADC_ST,
	DISP_RUN_FFT_ST,
	DISP_RUN_SHOW_ST,
} DISP_RUN_LOOP_ST_TF;

void disp_show_init(void);
void Set_disp_run_st(DISP_RUN_LOOP_ST_TF new);
DISP_RUN_LOOP_ST_TF Get_disp_run_st(void);
void mic_data_apply(void);
void disp_shwo_loop_st_machine(void);

void Set_Mode_Down(void);
void Set_Mode_Up(void);
void Set_Mode_roll(void);

void Display_peak_freq_onoff(u8 sel);

void mic_apply_demo_stripeZ_ff_dot(void);
void mic_apply_demo_only_ff_dot(void);
void mic_apply_demo_fft_wav(void);

void Set_FFT_Proc_Mode_Up(void);
void Set_FFT_Proc_Mode_Down(void);
void fft_display_mode_original(void);
void fft_display_mode_peak(void);
void fft_display_mode_zip(void);
void fft_display_mode_peak_Filter(void);
void fft_display_mode_PFM(void);

void wav_display_mode_fixed_zip(void);
void wav_display_mode_zip(void);
void mic_apply_draw_wav(void);

#endif

