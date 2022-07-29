/*
CFP
2022 - 
*/

#include "stm32f10x.h"
#include "board_cfg.h"
#define DISP_SLEF_DEF
#include "disp_show.h"
#include "sfreq_cfg.h"
#include "fft.h"
#include "iic_oled.h"
#include "spi_oled.h"
#include "user_adc.h"

#include "ex_key_proc.h"
#include "debug.h"
#include "led.h"
#include "tim.h"

u16 print_o_c;

//extern u8 OLED_GRAM[132][8];
extern u8 OLED_GRAM[128*8];

DISP_RUN_LOOP_ST_TF drs_current_st;
u16 b_loop_cnt;

u16 WheelPos;
//s16 fft_math;
s32 fft_a,fft_b,fft_sum;

s16 fft_max_val;
s16 fft_min_val;
s16 fft_avg_val;
u16 fft_over_min_cnt;
s16 fft_update_on;

u8 curr_mode;
u8 fft_proc_mode;

u16 dummy_cnt;

#define ZERO_DOT_USE_SOURCE   1

#define TOTAL_MODE_NUM		4
#define WAV_PROC_MODE_START		3

//volatile
void (*update_showing_mode_data[TOTAL_MODE_NUM]) (void)=
{
	mic_apply_demo_stripeZ_ff_dot,
	mic_apply_demo_only_ff_dot,
	mic_apply_demo_fft_wav,
	mic_apply_draw_wav,
};

#define FFT_DOUBLE_SHOW    1

#if (G_NUM_LEDS == 256)||(G_NUM_LEDS == 512)

#define TOTAL_FFT_PROC_MODE_NUM		5

void (*afft_data_proc_mode[TOTAL_FFT_PROC_MODE_NUM]) (void)=
{
	fft_display_mode_original,
	fft_display_mode_peak,
	fft_display_mode_peak_Filter,
	fft_display_mode_PFM,
	fft_display_mode_zip,
};

const char afft_idx[] =
{
'o',
'p',
'f',
'm',
'z',
};

const char mode_data_related[TOTAL_MODE_NUM] = 
{
3,
2,
2,
0,
};
#else
#define TOTAL_FFT_PROC_MODE_NUM		4

void (*afft_data_proc_mode[TOTAL_FFT_PROC_MODE_NUM]) (void)=
{
	fft_display_mode_original,
	fft_display_mode_peak,
	fft_display_mode_peak_Filter,
	fft_display_mode_zip,
};

const char afft_idx[] =
{
'o',
'p',
'f',
'z',
};

const char mode_data_related[TOTAL_MODE_NUM] = 
{
2,
2,
2,
0,
};

#endif

//#define NO_DB_TRANS	1

#define TOP2BOT		1
#ifdef TOP2BOT
const u8 offset_bar[] =
{
	0x0,0x1,0x3,0x7,0xf,0x1f,0x3f,0x7f,0xff
};

const u8 activing_dot[] =
{
	0x0,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80
};

#else
const u8 offset_bar[] =
{
	0x0,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff
};

const u8 activing_dot[] =
{
	0x0,0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01
};

#endif

//u8 falling_x_pos[(G_NUM_LEDS/2)];
u8 falling_y_pos[(G_NUM_LEDS/2)];

//u8 flying_x_pos[(G_NUM_LEDS/2)];
u8 flying_y_pos[(G_NUM_LEDS/2)];

//u8 flying_actives[(G_NUM_LEDS/2)];

//u8 ff_actives[(G_NUM_LEDS/2)];

u8 freq_max_pos;
u8 has_max_freq_pos;

#define FLY_LEVEL_VAL	6
#define FLY_LEVEL_POS	(FLY_LEVEL_VAL*8)

#define FALL_LEVEL_VAL	4
#define FALL_LEVEL_POS	(FALL_LEVEL_VAL*8)

#define FALL_GONE_LEVEL_VAL	1
#define FALL_GONE_LEVEL_POS	(FALL_LEVEL_VAL*8)

#define SHOW_POS_LIMIT		64

#ifdef OLED_1P3_COLOR_PANEL
#define A_SHOW_POS_LIMIT		96
#endif

#ifdef OLED_0P96_COLOR_PANEL
#define B_SHOW_POS_LIMIT		80
#endif

#ifdef OLED_0P96_BW_PANEL
#define C_SHOW_POS_LIMIT		64
#endif

//#else
//#define FLY_LEVEL_VAL	3//5
//#endif

/*temp move to .h*/

#define FFT_DIV_MIN_LVL_TH	(FFT_DIV_NOISE_TH - 1)//20
#define FFT_DIV_EMN_LVL_TH	(FFT_DIV_NOISE_TH - 3)//18

#define FFT_DB_MIN_LVL_TH	(FFT_DB_NOISE_TH - 1)//20
#define FFT_DB_EMN_LVL_TH	(FFT_DB_NOISE_TH - 3)//18

u8 using_which_data;
s16 *p2iFFT_data;
//s16 *p2oFFT_data;
s16 fft_cut_threshold;
s16 fft_cut_magin;

s8 showing_peak_freg_info;

void disp_show_init(void)
{
	drs_current_st = DISP_RUN_ST_NONE;
	mic_adc_start = 0;
	is_disp_show_period = 0;
	curr_mode = 0;
	fft_sample_cnt = 0;
	print_o_c = 0;
	//fft_max_val = FFT_NOISE_TH;//0;
	//has_max_freq_pos = 0;
	//p2oFFT_data = xFFT_out;
	p2iFFT_data = FFT_out;
	using_which_data = 0;		//using dB transfered data
	fft_cut_magin = 0;
	showing_peak_freg_info = 1;		//display peak freq info
	//rand16seed = (uint16_t)i_rand_x();
	fft_proc_mode = mode_data_related[curr_mode];		// stripe bar
}

void Set_disp_run_st(DISP_RUN_LOOP_ST_TF new)
{
	drs_current_st = new;
}

DISP_RUN_LOOP_ST_TF Get_disp_run_st(void)
{
	return drs_current_st;
}

void Set_FFT_Proc_Mode_Up(void)
{
	if(fft_proc_mode < (TOTAL_FFT_PROC_MODE_NUM - 1))
	{
		fft_proc_mode++;
	}
	else
	{
		dbg_uart_print_str("F-t\r\n");
	}
}

void Set_FFT_Proc_Mode_Down(void)
{
	if(fft_proc_mode)
	{
		fft_proc_mode--;
	}
	else
	{
		dbg_uart_print_str("F-b\r\n");
	}
}

void Set_Mode_roll(void)
{
	if(curr_mode < (TOTAL_MODE_NUM - 1))
	{
		curr_mode++;
	}
	else
	{
		curr_mode = 0;
		dbg_uart_print_str("mrb\r\n");
	}
	
#ifdef OLED_1P3_COLOR_PANEL
	SSD1351_FillRectangle(0,0,128,96,RGB565_Black);
#endif

#ifdef OLED_0P96_COLOR_PANEL
	ST7735_FillRectangle(0,0,160,80,RGB565_Black);
#endif

#ifdef OLED_0P96_BW_PANEL
	OLED_Clear_Buff();
#endif
	fft_cut_magin = 0;
	fft_proc_mode = mode_data_related[curr_mode];
}

void Set_Mode_Up(void)
{
	if(curr_mode < (TOTAL_MODE_NUM - 1))
	{
		//print_o_c = 0;
		#ifdef OLED_1P3_COLOR_PANEL
		SSD1351_FillRectangle(0,0,128,96,RGB565_Black);
		#endif
		#ifdef OLED_0P96_COLOR_PANEL
		ST7735_FillRectangle(0,0,160,80,RGB565_Black);
		#endif
		#ifdef OLED_0P96_BW_PANEL
		OLED_Clear_Buff();
		#endif
		curr_mode++;
		fft_cut_magin = 0;
	}
	else
	{
		dbg_uart_print_str("Top\r\n");
	}
	fft_proc_mode = mode_data_related[curr_mode];
}

void Set_Mode_Down(void)
{
	if(curr_mode)
	{
		//print_o_c = 0;
		#ifdef OLED_1P3_COLOR_PANEL
		SSD1351_FillRectangle(0,0,128,96,RGB565_Black);
		#endif
		#ifdef OLED_0P96_COLOR_PANEL
		ST7735_FillRectangle(0,0,160,80,RGB565_Black);
		#endif
		#ifdef OLED_0P96_BW_PANEL
		OLED_Clear_Buff();
		#endif
		curr_mode--;
		fft_cut_magin = 0;
	}
	else
	{
		dbg_uart_print_str("Bot\r\n");
	}
	fft_proc_mode = mode_data_related[curr_mode];
}

void Display_peak_freq_onoff(u8 sel)
{
	showing_peak_freg_info = sel;
	if(sel == 0)
	{
		#ifdef OLED_1P3_COLOR_PANEL
		SSD1351_FillRectangle(0,0,128,96,RGB565_Black);
		#endif
		#ifdef OLED_0P96_COLOR_PANEL
		ST7735_FillRectangle(0,0,160,80,RGB565_Black);
		#endif
		#ifdef OLED_0P96_BW_PANEL
		OLED_Clear_Buff();
		#endif
	}
}

s32 insqrt(s32 a)
{
	s32 i,c;
	s32 b = 0;
	for(i = 0x40000000; i != 0; i >>= 2)
	{
		c = i + b;
		b >>= 1;
		if(c <= a)
		{
			a -= c;
			b += i;
		}
	}
	return b;
}

u8 tab_ch_lg[] = {
	0,		//0
	0,		//0
	3,		//0.3
	5,		//0.48
	6,		//0.6
	7,		//0.7
	8,		//0.78
	8,		//0.85
	9,		//0.9
	9,		//0.95
};

/*
ln是log以e为底的对数
lg是log以10为底的对数
*/
int choose_lg(int val)
{
	int i;
	int temp_val;

	if(val < 10)
	{
		return (int)(tab_ch_lg[(u8)val]);
	}
	temp_val = val;
	for(i = 0; i < 30; i++)
	{
		if(temp_val >= 10)
		{
			temp_val = temp_val/10;
		}
		else
		{
			break;
		}
	}

	return (i*10+(int)(tab_ch_lg[(u8)temp_val]));
}

#ifdef FFT_COUNT_USE_32BIT
void mic_data_apply(void)
{
	u16 ipos, ires;

	if(curr_mode >= WAV_PROC_MODE_START)
	{
		//skip fft proc
		ires = 0;
		for (ipos = MIC_ADC_SAMPLE_OFT; ipos < MIC_ADC_SAMPLE_CNT; ipos++) {
			FFT_out[ires] = FFT_data[ipos];
			ires++;
		}
		return;
	}
	else
	{
		//pre-fft proc
		ires = 0;
		for (ipos = MIC_ADC_SAMPLE_OFT; ipos < MIC_ADC_SAMPLE_CNT; ipos++) {
			#ifdef FFT_IN_DATA_CUT_HALF
			tFFT_data[ires] = (FFT_data[ipos] - MIC_ADC_OFFSET)>>1;
			#else
			tFFT_data[ires] = (FFT_data[ipos] - MIC_ADC_OFFSET);
			#endif
			ires++;
		}
	}

	//compute FFT
	sFast_FFT(&(tFFT_data[0]),G_NUM_LEDS);

	//count the DB
	tFFT_data[G_NUM_LEDS] = 0;
	fft_avg_val = choose_lg(G_NUM_LEDS);
	ires = G_NUM_LEDS;
	for (b_loop_cnt = 0; b_loop_cnt < (G_NUM_LEDS/2); b_loop_cnt++) {
		fft_a = tFFT_data[b_loop_cnt];
		fft_b = tFFT_data[ires];
		fft_sum = fft_a*fft_a + fft_b*fft_b;
		fft_a = insqrt(fft_sum);
		fft_max_val = choose_lg(fft_a);
		if(fft_max_val < fft_avg_val)
    		FFT_out[b_loop_cnt] = 0;
		else
    		FFT_out[b_loop_cnt] = ((fft_max_val - fft_avg_val)*2);
		ires--;
	}
}

#else
void mic_data_apply(void)
{
	u16 ipos, ires;

	if(curr_mode >= WAV_PROC_MODE_START)
	{
		//skip fft proc
		ires = 0;
		for (ipos = MIC_ADC_SAMPLE_OFT; ipos < MIC_ADC_SAMPLE_CNT; ipos++) {
			FFT_out[ires] = FFT_data[ipos];
			ires++;
		}
		return;
	}
	else
	{
		//pre-fft proc
		for (ipos = MIC_ADC_SAMPLE_OFT; ipos < MIC_ADC_SAMPLE_CNT; ipos++) {
			#ifdef FFT_IN_DATA_CUT_HALF
			FFT_data[ipos] = (FFT_data[ipos] - MIC_ADC_OFFSET)>>1;
			#else
			FFT_data[ipos] = (FFT_data[ipos] - MIC_ADC_OFFSET);
			#endif
		}
	}

	//compute FFT
	xFast_FFT(&(FFT_data[MIC_ADC_SAMPLE_OFT]),G_NUM_LEDS);

	//count the DB
	FFT_data[G_NUM_LEDS + MIC_ADC_SAMPLE_OFT] = 0;
	fft_avg_val = choose_lg(G_NUM_LEDS);
	ires = G_NUM_LEDS + MIC_ADC_SAMPLE_OFT;
	ipos = 0;
	for (b_loop_cnt = MIC_ADC_SAMPLE_OFT; b_loop_cnt < (MIC_ADC_SAMPLE_OFT + G_NUM_LEDS/2); b_loop_cnt++) {
		fft_a = (s32)FFT_data[b_loop_cnt];
		fft_b = (s32)FFT_data[ires];
		fft_sum = fft_a*fft_a + fft_b*fft_b;
		fft_a = insqrt(fft_sum);
		fft_max_val = choose_lg(fft_a);
		if(fft_max_val < fft_avg_val)
    		FFT_out[ipos] = 0;
		else
    		FFT_out[ipos] = ((fft_max_val - fft_avg_val)*2);
		ires--;
		ipos++;
	}
}
#endif

void oled_showing_update(void)
{
	afft_data_proc_mode[fft_proc_mode]();
	update_showing_mode_data[curr_mode]();
}

u8 fft_filter_noise(void)
{
	u16 i;
	if(fft_over_min_cnt <= FFT_OVER_MIN_CNT_TH)
	{
		for(i = FFT_USED_DATA_START; i < FFT_USED_DATA_TT_END; i++)
		{
			xFFT_out[i] = 0;
		}
		has_max_freq_pos = 0;
		return 0;
	}
	return 1;
}

void fft_display_mode_original(void)
{
	int i;
	s16 i_data;
	fft_avg_val = 0;
	fft_over_min_cnt = 0;
	freq_max_pos = 0;
	has_max_freq_pos = 0;

	//count the base info
	p2iFFT_data = FFT_out;
	fft_cut_threshold = fft_cut_magin+FFT_DB_NOISE_TH;
	fft_max_val = fft_cut_magin+FFT_DB_NOISE_TH;//0;
	
	for(i = FFT_USED_DATA_START; i < FFT_USED_DATA_TT_END; i++)
	{
		i_data = p2iFFT_data[i];
		fft_avg_val = fft_avg_val + i_data;
		//if((i> 0) && (i_data > fft_max_val))
		if((i_data > fft_max_val))
		{
			fft_max_val = i_data;
			freq_max_pos = i;
			has_max_freq_pos = 1;
		}
		#ifdef FFT_DOUBLE_SHOW
		xFFT_out[i] = i_data*2;
		#else
		xFFT_out[i] = i_data;
		#endif
	}
	fft_avg_val = fft_avg_val/(FFT_USED_DATA_TT_END);
	#ifdef ZERO_DOT_USE_SOURCE
	#ifdef FFT_DOUBLE_SHOW
	xFFT_out[0] = xFFT_out[0]/2;
	#endif
	#else
	xFFT_out[0] = fft_avg_val/2;
	#endif
}

void fft_display_mode_peak(void)
{
	int i;
	s16 i_data;
	fft_avg_val = 0;
	fft_over_min_cnt = 0;
	freq_max_pos = 0;
	has_max_freq_pos = 0;

	//count the base info
	p2iFFT_data = FFT_out;
	fft_cut_threshold = fft_cut_magin+FFT_DB_NOISE_TH;
	fft_max_val = fft_cut_magin+FFT_DB_NOISE_TH;//0;
	
	for(i = FFT_USED_DATA_START; i < FFT_USED_DATA_TT_END; i++)
	{
		i_data = p2iFFT_data[i];
		fft_avg_val = fft_avg_val + i_data;
		if((i> 0) && (i_data > fft_max_val))
		{
			fft_max_val = i_data;
			freq_max_pos = i;
			has_max_freq_pos = 1;
		}
		//xFFT_out[i] = i_data;
	}
	
	if(fft_max_val > SHOW_POS_LIMIT)
	{
		fft_min_val = fft_max_val - SHOW_POS_LIMIT;
	}
	else
	{
		fft_min_val = 0;
	}
	
	for(i = (FFT_USED_DATA_START + 1); i < FFT_USED_DATA_TT_END; i++)
	{
		i_data = p2iFFT_data[i];
		if(fft_min_val < i_data)
		{
			#ifdef FFT_DOUBLE_SHOW
			xFFT_out[i] = (i_data - fft_min_val)*2;
			#else
			xFFT_out[i] = i_data - fft_min_val;
			#endif
		}
		else
			xFFT_out[i] = 0;//i_data/2;
	}
	fft_avg_val = fft_avg_val/(FFT_USED_DATA_TT_END);
	#ifdef ZERO_DOT_USE_SOURCE
	xFFT_out[0] = p2iFFT_data[0];///2;
	#else
	xFFT_out[0] = fft_avg_val/2;
	#endif
}

#if (G_NUM_LEDS == 512)
void fft_display_mode_PFM(void)
{
	int i;
	s16 i_data;

	fft_avg_val = 0;
	fft_over_min_cnt = 0;
	freq_max_pos = 0;
	has_max_freq_pos = 0;

	p2iFFT_data = FFT_out;
	fft_cut_threshold = fft_cut_magin+FFT_DB_NOISE_TH;
	fft_max_val = fft_cut_magin+FFT_DB_NOISE_TH;//0;

	for(i = 0; i < 32; i++)
	{
		i_data = p2iFFT_data[8*i];
		i_data += p2iFFT_data[8*i+1];
		i_data += p2iFFT_data[8*i+2];
		i_data += p2iFFT_data[8*i+3];
		//i_data /= 4;
		i_data += p2iFFT_data[8*i+4];
		i_data += p2iFFT_data[8*i+5];
		i_data += p2iFFT_data[8*i+6];
		i_data += p2iFFT_data[8*i+7];
		i_data /= 2;
		fft_avg_val = fft_avg_val + i_data;
		if(i_data >= fft_cut_threshold)
		{
			fft_over_min_cnt++;
		}
		#ifdef FFT_DOUBLE_SHOW
		//i_data *= 2;
		#endif
		xFFT_out[4*i] = i_data;
		xFFT_out[4*i+1] = i_data;
		xFFT_out[4*i+2] = i_data;
		xFFT_out[4*i+3] = 0;
		if((i> 0) && (i_data > fft_max_val))
		{
//skip zero point.
			fft_max_val = i_data;
			freq_max_pos = i;
			has_max_freq_pos = 1;
		}
	}
	fft_avg_val = fft_avg_val/32;
	#ifdef ZERO_DOT_USE_SOURCE
	i_data = xFFT_out[0];///2;
	xFFT_out[0] = i_data;
	xFFT_out[1] = i_data;
	xFFT_out[2] = i_data;
	xFFT_out[3] = 0;
	#else
	xFFT_out[0] = fft_avg_val*2;
	xFFT_out[1] = xFFT_out[0];//fft_avg_val*2;
	xFFT_out[2] = xFFT_out[0];//fft_avg_val*2;
	xFFT_out[3] = 0;//fft_avg_val*2;
	#endif
	fft_filter_noise();
}
#endif

//only for 256 samples case
#if (G_NUM_LEDS == 256)
void fft_display_mode_PFM(void)
{
	int i;
	s16 i_data;

	fft_avg_val = 0;
	fft_over_min_cnt = 0;
	freq_max_pos = 0;
	has_max_freq_pos = 0;

	p2iFFT_data = FFT_out;
	fft_cut_threshold = fft_cut_magin+FFT_DB_NOISE_TH;
	fft_max_val = fft_cut_magin+FFT_DB_NOISE_TH;//0;

	for(i = 0; i < 32; i++)
	{
		i_data = p2iFFT_data[4*i];
		i_data += p2iFFT_data[4*i+1];
		i_data += p2iFFT_data[4*i+2];
		i_data += p2iFFT_data[4*i+3];
		i_data /= 2;
		fft_avg_val = fft_avg_val + i_data;
		if(i_data >= fft_cut_threshold)
		{
			fft_over_min_cnt++;
		}
		#ifdef FFT_DOUBLE_SHOW
		//i_data *= 2;
		#endif
		xFFT_out[4*i] = i_data;
		xFFT_out[4*i+1] = i_data;
		xFFT_out[4*i+2] = i_data;
		xFFT_out[4*i+3] = 0;
		if((i> 0) && (i_data > fft_max_val))
		{
//skip zero point.
			fft_max_val = i_data;
			freq_max_pos = i;
			has_max_freq_pos = 1;
		}
	}
	fft_avg_val = fft_avg_val/32;
	#ifdef ZERO_DOT_USE_SOURCE
	i_data = xFFT_out[0];///2;
	xFFT_out[0] = i_data;
	xFFT_out[1] = i_data;
	xFFT_out[2] = i_data;
	xFFT_out[3] = 0;
	#else
	xFFT_out[0] = fft_avg_val*2;
	xFFT_out[1] = xFFT_out[0];//fft_avg_val*2;
	xFFT_out[2] = xFFT_out[0];//fft_avg_val*2;
	xFFT_out[3] = 0;//fft_avg_val*2;
	#endif
	fft_filter_noise();
}
#endif

void fft_display_mode_peak_Filter(void)
{
	int i;
	s16 i_data;

	fft_avg_val = 0;
	fft_over_min_cnt = 0;
	freq_max_pos = 0;
	has_max_freq_pos = 0;

	p2iFFT_data = FFT_out;
	fft_cut_threshold = fft_cut_magin+FFT_DB_NOISE_TH;
	fft_max_val = fft_cut_magin+FFT_DB_NOISE_TH;//0;
	
	for(i = FFT_USED_DATA_START; i < FFT_USED_DATA_TT_END; i++)
	{
		i_data = p2iFFT_data[i];
		fft_avg_val = fft_avg_val + i_data;
		if(i_data >= fft_cut_threshold)
		{
			fft_over_min_cnt++;
		}
		#ifdef FFT_DOUBLE_SHOW
		xFFT_out[i] = i_data*2;
		#else
		xFFT_out[i] = i_data;
		#endif
		
		if((i> 0) && (i_data > fft_max_val))
		{
//skip zero point.
			fft_max_val = i_data;
			freq_max_pos = i;
			has_max_freq_pos = 1;
		}
	}
	fft_avg_val = fft_avg_val/(FFT_USED_DATA_TT_END);
	#ifdef ZERO_DOT_USE_SOURCE
	xFFT_out[0] = xFFT_out[0]>>1;
	#else
	xFFT_out[0] = fft_avg_val>>1;
	#endif
	#if 1
	fft_filter_noise();
	#else
	if(fft_over_min_cnt < FFT_OVER_MIN_CNT_TH)
	{
		for(i = FFT_USED_DATA_START; i < FFT_USED_DATA_TT_END; i++)
		{
			xFFT_out[i] = 0;
		}
	}
	#endif
}




void fft_display_mode_zip(void)
{
	int i;
	s16 i_data;
	fft_avg_val = 0;
	fft_over_min_cnt = 0;
	freq_max_pos = 0;
	has_max_freq_pos = 0;

	//count the base info
	p2iFFT_data = FFT_out;
	fft_cut_threshold = fft_cut_magin+FFT_DB_NOISE_TH;
	fft_max_val = fft_cut_magin+FFT_DB_NOISE_TH;//0;
	
	for(i = FFT_USED_DATA_START; i < FFT_USED_DATA_TT_END; i++)
	{
		i_data = p2iFFT_data[i];
		fft_avg_val = fft_avg_val + i_data;
		if((i> 0) && (i_data > fft_max_val))
		{
			fft_max_val = i_data;
			freq_max_pos = i;
			has_max_freq_pos = 1;
		}
		xFFT_out[i] = i_data;
	}
	if(fft_max_val)
	{
		fft_min_val = 1 + (fft_max_val/SHOW_POS_LIMIT);
		for(i = (FFT_USED_DATA_START+1); i < FFT_USED_DATA_TT_END; i++)
		{
			i_data = p2iFFT_data[i];
			xFFT_out[i] = i_data/fft_min_val;
		}
	}
	
	fft_avg_val = fft_avg_val/(FFT_USED_DATA_TT_END);
	#ifdef ZERO_DOT_USE_SOURCE
	;//xFFT_out[0] = xFFT_out[0]/2;
	#else
	xFFT_out[0] = fft_avg_val/2;
	#endif
	//xFFT_out[0] = p2iFFT_data[0]/2;
}

void wav_display_mode_fixed_zip(void)
{
	u16 i;
	s16 i_data;
	fft_avg_val = 0;

	for(i = 0; i < G_NUM_LEDS; i++)
	{
		i_data = (FFT_out[i] - MIC_ADC_OFFSET)>>2;
		//i_data = FFT_out[i] - MIC_ADC_OFFSET;
		//i_data = FFT_out[i];
		fft_avg_val = i_data + (SHOW_POS_LIMIT/2);
		if(fft_avg_val > SHOW_POS_LIMIT)
		{
			i_data = SHOW_POS_LIMIT;
		}
		if(fft_avg_val < 0)
		{
			fft_avg_val = 0;
		}
		xFFT_out[i] = fft_avg_val;
	}
}

void wav_display_mode_zip(void)
{
	u16 i;
	s16 i_data;
	s32 sum_data;
	sum_data = 0;
	//fft_over_min_cnt = 0;
	fft_max_val = 0;
	fft_min_val = 0x7fff;
	//freq_max_pos = 0;
	//has_max_freq_pos = 0;

	for(i = 0; i < G_NUM_LEDS; i++)
	{
		i_data = FFT_out[i];
		sum_data = sum_data + i_data;
		if(i_data > fft_max_val)
		{
			fft_max_val = i_data;
			//freq_max_pos = i;
			//has_max_freq_pos = 1;
		}
		if(i_data < fft_min_val)
		{
			fft_min_val = i_data;
		}
	}
	fft_avg_val = sum_data/G_NUM_LEDS;
	i_data = (fft_max_val - fft_min_val);
	if(i_data)
	{
		fft_max_val = 1 + (i_data/SHOW_POS_LIMIT);
	}
	for(i = 0; i < G_NUM_LEDS; i++)
	{
		i_data = (FFT_out[i] - fft_avg_val)/fft_max_val;
		xFFT_out[i] = i_data + 32;
	}
}


/*
FFT不转为dB（直接除以100 - 采样256点时；如果是64点时，除以50），动态范围更广。（适合小动态的输入的场合）
加上x_count_fft_state()效果很好（类似降噪效果）
不加上x_count_fft_state()效果也不错

FFT转为dB，动态范围被压缩，不容易看到变化，适合动态输入大的场合。
需要加x_count_fft_state()，突出变化（否则几乎看不到变化）
在转为dB前先除以10，能减少噪音。不过动态范围进一步缩小，建议不要加x_count_fft_state()

2022-06
以上信息忽略。
*/

void OLED_Draw_Vertical_Column(u8 x,u8 y,u8 length)
{
	u8 y0;
#if 1
	u8 cPattern;
	u8 s_shift;
	u8 draw_len = length;
	u16 offset = (u16)x;
	u8 *pBuf;
	pBuf = OLED_GRAM;
	y0 = y>>3;
	s_shift = 0;
	while(s_shift < y0)
	{
		offset += 128;
		s_shift++;
	}
	s_shift = y & 0x07;
	if(s_shift)
	{
		if(draw_len > (8-s_shift))
		{
			draw_len -= (8-s_shift);
			cPattern = (~offset_bar[s_shift]);
		}
		else
		{
			cPattern = offset_bar[draw_len] << s_shift;
			draw_len = 0;
		}
		pBuf[offset] = cPattern;
		offset += 128;
		y0++;
	}
	while(y0 < 8)
	{
		if(draw_len > 8)
		{
			pBuf[offset] = 0xff;
			draw_len -= 8;
		}
		else
		{
			pBuf[offset] = offset_bar[draw_len];
			draw_len = 0;
		}
		y0++;
		offset += 128;
	}
#else
	for(y0 = 0; y0 < 64; y0++)
	{
		if(y0 >= length)
		{
			OLED_ClearPoint(x,(y+y0));
		}
		else
		{
			OLED_DrawPoint(x,(y+y0));
		}
	}
#endif
}

void OLED_DrawY_dot(u8 x,u8 y,u8 fall_dot)
{
	u8 y0;
	u8 dPattern;
	u8 s_shift;
	u8 draw_fall_dot = fall_dot;
	u16 offset = (u16)x;
	u8 *pBuf;

	pBuf = OLED_GRAM;

	y0 = y>>3;
	s_shift = 0;
	while(s_shift < y0)
	{
		offset += 128;
		s_shift++;
	}

	s_shift = y & 0x07;
	if(s_shift)
	{
		if(draw_fall_dot > (8-s_shift))
		{
			draw_fall_dot -= (8-s_shift);
			dPattern = 0;
		}
		else
		{
			dPattern = activing_dot[draw_fall_dot] << s_shift;
			draw_fall_dot = 0;
		}
		pBuf[offset] |= dPattern;
		offset += 128;
		y0++;
	}
	while(y0 < 8)
	{
		if(draw_fall_dot > 8)
		{
			draw_fall_dot -= 8;
			dPattern = 0;
		}
		else
		{
			dPattern = activing_dot[draw_fall_dot];
			draw_fall_dot = 0;
		}
		pBuf[offset] |= dPattern;
		y0++;
		offset += 128;
	}
}

void OLED_DrawX_dot(u8 x,u8 y,u8 fall_dot)
{
	u8 y0;
	u8 dPattern;
	u8 s_shift;
	u8 draw_fall_dot = fall_dot;
	u16 offset = (u16)x;
	u8 *pBuf;

	pBuf = OLED_GRAM;

	y0 = y>>3;
	s_shift = 0;
	while(s_shift < y0)
	{
		offset += 128;
		s_shift++;
	}

	s_shift = y & 0x07;
	if(s_shift)
	{
		if(draw_fall_dot > (8-s_shift))
		{
			draw_fall_dot -= (8-s_shift);
			dPattern = 0;
		}
		else
		{
			dPattern = activing_dot[draw_fall_dot] << s_shift;
			draw_fall_dot = 0;
		}
		pBuf[offset] = dPattern;
		offset += 128;
		y0++;
	}
	while(y0 < 8)
	{
		if(draw_fall_dot > 8)
		{
			draw_fall_dot -= 8;
			dPattern = 0;
		}
		else
		{
			dPattern = activing_dot[draw_fall_dot];
			draw_fall_dot = 0;
		}
		pBuf[offset] = dPattern;
		y0++;
		offset += 128;
	}
}

void OLED_DrawX_Vertical_Column(u8 x,u8 y,u8 length,u8 fall_dot)
{
	u8 y0;
#if 1
	u8 cPattern;
	u8 dPattern;
	u8 s_shift;
	u8 draw_fall_dot = fall_dot;
	u8 draw_len = length;
	u16 offset = (u16)x;
	u8 *pBuf;
	pBuf = OLED_GRAM;
	y0 = y>>3;
	s_shift = 0;
	while(s_shift < y0)
	{
		offset += 128;
		s_shift++;
	}
	s_shift = y & 0x07;
	if(s_shift)
	{
		if(draw_len > (8-s_shift))
		{
			draw_len -= (8-s_shift);
			cPattern = (~offset_bar[s_shift]);
		}
		else
		{
			cPattern = offset_bar[draw_len] << s_shift;
			draw_len = 0;
		}
		if(draw_fall_dot > (8-s_shift))
		{
			draw_fall_dot -= (8-s_shift);
			dPattern = 0;
		}
		else
		{
			dPattern = activing_dot[draw_fall_dot] << s_shift;
			draw_fall_dot = 0;
		}
		pBuf[offset] = cPattern|dPattern;
		offset += 128;
		y0++;
	}
	while(y0 < 8)
	{
		if(draw_len > 8)
		{
			cPattern = 0xff;
			draw_len -= 8;
		}
		else
		{
			cPattern = offset_bar[draw_len];
			draw_len = 0;
		}
		if(draw_fall_dot > 8)
		{
			draw_fall_dot -= 8;
			dPattern = 0;
		}
		else
		{
			dPattern = activing_dot[draw_fall_dot];
			draw_fall_dot = 0;
		}
		pBuf[offset] = cPattern|dPattern;
		y0++;
		offset += 128;
	}
#else
	for(y0 = 0; y0 < 64; y0++)
	{
		if(y0 >= length)
		{
			OLED_ClearPoint(x,(y+y0));
		}
		else
		{
			OLED_DrawPoint(x,(y+y0));
		}
	}
#endif
}

#ifdef OLED_1P3_COLOR_PANEL
void draw_freq_info(void)
{
	u8 i;
	u16 freq_val;
	u16 show_val;
	u8 x_pos = 16;

	if(showing_peak_freg_info == 0)
	{
		return;
	}

	SSD1351_WriteChar_16x16(112,80,6,RGB565_Red,RGB565_Black);
	SSD1351_WriteChar_16x16(96,80,0,RGB565_Green,RGB565_Black);
	SSD1351_WriteChar_16x16(80,80,3,RGB565_Blue,RGB565_Black);
	SSD1351_WriteChar_16x16(64,80,1,RGB565_Yellow,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'K',RGB565_White,RGB565_Black);
#ifdef DISP_FRAME_RATIO_30HZ
	SSD1351_WriteChar_8x16(8,64,'3',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,64,'0',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_25HZ
	SSD1351_WriteChar_8x16(8,64,'2',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,64,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_20HZ
	SSD1351_WriteChar_8x16(8,64,'2',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,64,'0',RGB565_Lgray,RGB565_Black);
#endif

#ifdef DISP_FRAME_RATIO_16HZ
	SSD1351_WriteChar_8x16(8,64,'1',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,64,'6',RGB565_Lgray,RGB565_Black);
#endif

#ifdef DISP_FRAME_RATIO_15HZ
	SSD1351_WriteChar_8x16(8,64,'1',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,64,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_5HZ
	SSD1351_WriteChar_8x16(8,64,'0',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,64,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_40HZ
	SSD1351_WriteChar_8x16(8,64,'4',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,64,'0',RGB565_Lgray,RGB565_Black);
#endif
	SSD1351_WriteChar_8x16(16,64,afft_idx[fft_proc_mode],RGB565_Lgray,RGB565_Black);

#if 1
	if(has_max_freq_pos == 0)
	{
		for(i = 0; i < 6 ; i ++)
		{
			SSD1351_WriteChar_8x16(x_pos,80,'-',RGB565_White,RGB565_Black);
			x_pos += 8;
		}
		return;
	}
#endif
	//freq_val = ((u16)freq_max_pos+1) * BASE_FREQ;
	freq_val = ((u16)freq_max_pos) * BASE_FREQ;
	if(LDOT_POS == 0)
	{
		SSD1351_WriteChar_8x16(x_pos,80,'0',RGB565_White,RGB565_Black);
		x_pos += 8;
	}
	for(i = 0; i < 6 ; i ++)
	{
		if(freq_val)
		{
			show_val = freq_val%10;
			freq_val = freq_val/10;
			if(i == LDOT_POS)
			{
				SSD1351_WriteChar_8x16(x_pos,80,'.',RGB565_White,RGB565_Black);
				x_pos += 8;
			}
			SSD1351_WriteChar_8x16(x_pos,80,('0'+(u8)show_val),RGB565_White,RGB565_Black);
			x_pos += 8;
		}
		else
		{
			break;
		}
	}
	while(i < LDOT_POS)
	{
		SSD1351_WriteChar_8x16(x_pos,80,'0',RGB565_White,RGB565_Black);
		x_pos += 8;
		i += 1;
	}
	if(i == LDOT_POS)
	{
		SSD1351_WriteChar_8x16(x_pos,80,'.',RGB565_White,RGB565_Black);
		x_pos += 8;
		SSD1351_WriteChar_8x16(x_pos,80,'0',RGB565_White,RGB565_Black);
		x_pos += 8;
		i += 1;
	}
	while(i < 5)
	{
		i++;
		SSD1351_WriteChar_8x16(x_pos,80,' ',RGB565_White,RGB565_Black);
		x_pos += 8;
	}
}

#endif

#ifdef OLED_0P96_COLOR_PANEL
void draw_freq_info_y(void)
{
	u8 i;
	u16 freq_val;
	u16 show_val;
	u8 x_pos = 16;

	if(showing_peak_freg_info == 0)
	{
		return;
	}

	ST7735_WriteChar_16x16(144,64,6,RGB565_Red,RGB565_Black);
	ST7735_WriteChar_16x16(128,64,0,RGB565_Green,RGB565_Black);
	ST7735_WriteChar_16x16(112,64,3,RGB565_Blue,RGB565_Black);
	ST7735_WriteChar_16x16(96,64,1,RGB565_Yellow,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'K',RGB565_White,RGB565_Black);
#ifdef DISP_FRAME_RATIO_30HZ
	ST7735_WriteChar_8x16(8,48,'3',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,48,'0',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_25HZ
	ST7735_WriteChar_8x16(8,48,'2',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,48,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_20HZ
	ST7735_WriteChar_8x16(8,48,'2',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,48,'0',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_16HZ
	ST7735_WriteChar_8x16(8,48,'1',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,48,'6',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_15HZ
	ST7735_WriteChar_8x16(8,48,'1',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,48,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_5HZ
	ST7735_WriteChar_8x16(8,48,'0',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,48,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_40HZ
	ST7735_WriteChar_8x16(8,48,'4',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,48,'0',RGB565_Lgray,RGB565_Black);
#endif
	ST7735_WriteChar_8x16(16,48,afft_idx[fft_proc_mode],RGB565_Lgray,RGB565_Black);


#if 1
	if(has_max_freq_pos == 0)
	{
		for(i = 0; i < 6 ; i ++)
		{
			ST7735_WriteChar_8x16(x_pos,64,'-',RGB565_White,RGB565_Black);
			x_pos += 8;
		}
		return;
	}
#endif
	//freq_val = ((u16)freq_max_pos+1) * BASE_FREQ;
	freq_val = ((u16)freq_max_pos) * BASE_FREQ;
	if(LDOT_POS == 0)
	{
		ST7735_WriteChar_8x16(x_pos,64,'0',RGB565_White,RGB565_Black);
		x_pos += 8;
	}
	for(i = 0; i < 6 ; i ++)
	{
		if(freq_val)
		{
			show_val = freq_val%10;
			freq_val = freq_val/10;
			if(i == LDOT_POS)
			{
				ST7735_WriteChar_8x16(x_pos,64,'.',RGB565_White,RGB565_Black);
				x_pos += 8;
			}
			ST7735_WriteChar_8x16(x_pos,64,('0'+(u8)show_val),RGB565_White,RGB565_Black);
			x_pos += 8;
		}
		else
		{
			break;
		}
	}
	while(i < LDOT_POS)
	{
		ST7735_WriteChar_8x16(x_pos,64,'0',RGB565_White,RGB565_Black);
		x_pos += 8;
		i += 1;
	}
	if(i == LDOT_POS)
	{
		ST7735_WriteChar_8x16(x_pos,64,'.',RGB565_White,RGB565_Black);
		x_pos += 8;
		ST7735_WriteChar_8x16(x_pos,64,'0',RGB565_White,RGB565_Black);
		x_pos += 8;
		i += 1;
	}
	while(i < 5)
	{
		i++;
		ST7735_WriteChar_8x16(x_pos,64,' ',RGB565_White,RGB565_Black);
		x_pos += 8;
	}
}

#endif

#ifdef OLED_0P96_BW_PANEL
#define SHOW_FREQ_LINE_POS    48
#define SHOW_FREQ_START_POS   16

void draw_freq_info_x(void)
{
	u8 i;
	u16 freq_val;
	u16 show_val;
	u8 x_pos = SHOW_FREQ_START_POS;
#ifdef DISP_FRAME_RATIO_30HZ
	OLED_ShowCharR_8x16(16,32,'3');
	OLED_ShowCharR_8x16(8,32,'0');
#endif
#ifdef DISP_FRAME_RATIO_25HZ
	OLED_ShowCharR_8x16(16,32,'2');
	OLED_ShowCharR_8x16(8,32,'5');
#endif
#ifdef DISP_FRAME_RATIO_20HZ
	OLED_ShowCharR_8x16(16,32,'2');
	OLED_ShowCharR_8x16(8,32,'0');
#endif

#ifdef DISP_FRAME_RATIO_16HZ
	OLED_ShowCharR_8x16(16,32,'1');
	OLED_ShowCharR_8x16(8,32,'6');
#endif

#ifdef DISP_FRAME_RATIO_15HZ
	OLED_ShowCharR_8x16(16,32,'1');
	OLED_ShowCharR_8x16(8,32,'5');
#endif
#ifdef DISP_FRAME_RATIO_5HZ
	OLED_ShowCharR_8x16(16,32,'0');
	OLED_ShowCharR_8x16(8,32,'5');
#endif
#ifdef DISP_FRAME_RATIO_40HZ
	OLED_ShowCharR_8x16(16,32,'4');
	OLED_ShowCharR_8x16(8,32,'0');
#endif

	OLED_ShowCharR_8x16(0,32,afft_idx[fft_proc_mode]);

	if(showing_peak_freg_info == 0)
	{
		return;
	}

	OLED_ShowPF_16x16(112,SHOW_FREQ_LINE_POS,5);
	OLED_ShowPF_16x16(96,SHOW_FREQ_LINE_POS,2);
	OLED_ShowPF_16x16(80,SHOW_FREQ_LINE_POS,0);
	OLED_ShowPF_16x16(64,SHOW_FREQ_LINE_POS,3);
	OLED_ShowCharR_8x16(0,SHOW_FREQ_LINE_POS,'K');

#if 1
	if(has_max_freq_pos == 0)
	{
		for(i = 0; i < 6 ; i ++)
		{
			OLED_ShowCharR_8x16(x_pos,SHOW_FREQ_LINE_POS,'-');
			x_pos += 8;
		}
		return;
	}
#endif
	//freq_val = ((u16)freq_max_pos+1) * BASE_FREQ;
	freq_val = ((u16)freq_max_pos) * BASE_FREQ;
	if(LDOT_POS == 0)
	{
		OLED_ShowCharR_8x16(x_pos,SHOW_FREQ_LINE_POS,'0');
		x_pos += 8;
	}
	for(i = 0; i < 6 ; i ++)
	{
		if(freq_val)
		{
			show_val = freq_val%10;
			freq_val = freq_val/10;
			if(i == LDOT_POS)
			{
				OLED_ShowCharR_8x16(x_pos,SHOW_FREQ_LINE_POS,'.');
				x_pos += 8;
			}
			OLED_ShowCharR_8x16(x_pos,SHOW_FREQ_LINE_POS,('0'+(u8)show_val));
			x_pos += 8;
		}
		else
		{
			break;
		}
	}

	while(i < LDOT_POS)
	{
		OLED_ShowCharR_8x16(x_pos,SHOW_FREQ_LINE_POS,'0');
		x_pos += 8;
		i += 1;
	}

	if(i == LDOT_POS)
	{
		OLED_ShowCharR_8x16(x_pos,SHOW_FREQ_LINE_POS,'.');
		x_pos += 8;
		OLED_ShowCharR_8x16(x_pos,SHOW_FREQ_LINE_POS,'0');
		x_pos += 8;
		i += 1;
	}
	while(i < 5)
	{
		i++;
		OLED_ShowCharR_8x16(x_pos,SHOW_FREQ_LINE_POS,' ');
		x_pos += 8;
	}
}
#endif

void mic_apply_draw_wav(void)
{
	u16 i;
	u16 line_length;

	u16 offset;
	u16 i_offset,i_sx;
	u16 cur_fft_val;
	u16 sx,sy;

	if(curr_mode == WAV_PROC_MODE_START)
	{
		wav_display_mode_fixed_zip();
	}
	if(curr_mode == (WAV_PROC_MODE_START+1))
	{
		wav_display_mode_zip();
	}
	#ifdef OLED_0P96_BW_PANEL
	OLED_Clear_Buff();
	#endif
	#ifdef OLED_1P3_COLOR_PANEL
	SSD1351_FillRectangle(0,0,128,80,RGB565_Black);
	#endif
	#ifdef OLED_0P96_COLOR_PANEL
	
	ST7735_FillRectangle((160-128),0,160,80,RGB565_Black);
	#endif
#if (G_NUM_LEDS == 256)||(G_NUM_LEDS == 512)
    line_length = 1;
	
	sy = (u16)xFFT_out[0];
	if(sy >= SHOW_POS_LIMIT)
	{
		sy = SHOW_POS_LIMIT;
	}
	#ifdef OLED_0P96_BW_PANEL
	sx = 127;
	//draw first dot
	OLED_DrawPoint(sx,sy);
	#endif
	#ifdef OLED_1P3_COLOR_PANEL
	sx = 127;
	//draw first dot
	SSD1351_DrawPixel(sx,sy,RGB565_Blue);
	#endif
	#ifdef OLED_0P96_COLOR_PANEL
	i_sx = 159;
	ST7735_DrawPoint(i_sx,sy,RGB565_Blue);
	#endif
	for(i = 1; i < FFT_USED_DATA_TT_END; i++)
	{
		cur_fft_val = (u16)xFFT_out[i];
		if(cur_fft_val >= SHOW_POS_LIMIT)
		{
			cur_fft_val = SHOW_POS_LIMIT;
		}
		{
			//draw line
			#ifdef OLED_1P3_COLOR_PANEL
			offset = 128 - i*line_length;
			SSD1351_DrawLine(sx,sy,offset,cur_fft_val,RGB565_Blue);
			#endif
			#ifdef OLED_0P96_BW_PANEL
			offset = 128 - i*line_length;
			OLED_DrawLine(sx,sy,offset,cur_fft_val);
			#endif
			#ifdef OLED_0P96_COLOR_PANEL
			i_offset = 160 - i*line_length;
			ST7735_DrawLine(i_sx,sy,i_offset,cur_fft_val,RGB565_Blue);
			#endif
			sx = offset;
			i_sx = i_offset;
			sy = cur_fft_val;
		}
	}
#else
	line_length = 128 / G_NUM_LEDS;
	
	sy = (u16)xFFT_out[0];
	if(sy >= SHOW_POS_LIMIT)
	{
		sy = SHOW_POS_LIMIT;
	}
	//offset = 128 - line_length;
	#ifdef OLED_0P96_BW_PANEL
	sx = 127;
	//draw first dot
	OLED_DrawPoint(sx,sy);
	#endif
	#ifdef OLED_1P3_COLOR_PANEL
	sx = 127;
	//draw first dot
	SSD1351_DrawPixel(sx,sy,RGB565_Blue);
	#endif
	#ifdef OLED_0P96_COLOR_PANEL
	i_sx = 159;
	ST7735_DrawPoint(i_sx,sy,RGB565_Blue);
	#endif
	for(i = 1; i < G_NUM_LEDS; i++)
	{
		cur_fft_val = (u16)xFFT_out[i];
		if(cur_fft_val >= SHOW_POS_LIMIT)
		{
			cur_fft_val = SHOW_POS_LIMIT;
		}
		#if 0
		offset = 127 - i*line_length;
		//OLED_DrawPoint(offset,cur_fft_val);
		OLED_DrawLine(offset,32,offset,cur_fft_val);
		#else
		/*
		if(line_length == 1)
		{
			//draw a dot
			OLED_DrawPoint((127-i),cur_fft_val);
		}
		else
		*/
		{
			//draw line
			offset = 128 - i*line_length;
			#ifdef OLED_1P3_COLOR_PANEL
			//offset = 128 - i*line_length;
			SSD1351_DrawLine(sx,sy,offset,cur_fft_val,RGB565_Blue);
			#endif
			#ifdef OLED_0P96_BW_PANEL
			//offset = 128 - i*line_length;
			OLED_DrawLine(sx,sy,offset,cur_fft_val);
			#endif
			#ifdef OLED_0P96_COLOR_PANEL
			i_offset = 160 - i*line_length;
			ST7735_DrawLine(i_sx,sy,i_offset,cur_fft_val,RGB565_Blue);
			#endif
			sx = offset;
			i_sx = i_offset;
			sy = cur_fft_val;
		}
		#endif
	}
#endif

#ifdef DISP_MODE_INFO
#ifdef OLED_1P3_COLOR_PANEL
	SSD1351_WriteChar_8x16(0,48,'a',RGB565_Yellow,RGB565_Black);
	SSD1351_WriteChar_8x16(8,48,'w',RGB565_Yellow,RGB565_Black);
	if(curr_mode == WAV_PROC_MODE_START)
	{
		SSD1351_WriteChar_8x16(16,48,'f',RGB565_Yellow,RGB565_Black);
	}
	if(curr_mode == (WAV_PROC_MODE_START+1))
	{
		SSD1351_WriteChar_8x16(16,48,'z',RGB565_Yellow,RGB565_Black);
	}
#endif
#ifdef OLED_0P96_COLOR_PANEL
	ST7735_WriteChar_8x16(0,48,'a',RGB565_Yellow,RGB565_Black);
	ST7735_WriteChar_8x16(8,48,'w',RGB565_Yellow,RGB565_Black);
	if(curr_mode == WAV_PROC_MODE_START)
	{
		ST7735_WriteChar_8x16(16,48,'f',RGB565_Yellow,RGB565_Black);
	}
	if(curr_mode == (WAV_PROC_MODE_START+1))
	{
		ST7735_WriteChar_8x16(16,48,'z',RGB565_Yellow,RGB565_Black);
	}
#endif

#ifdef OLED_0P96_BW_PANEL
	OLED_ShowCharR_8x16(0,48,'a');
	OLED_ShowCharR_8x16(8,48,'w');
	if(curr_mode == WAV_PROC_MODE_START)
	{
		OLED_ShowCharR_8x16(16,48,'f');
	}
	if(curr_mode == (WAV_PROC_MODE_START+1))
	{
		OLED_ShowCharR_8x16(16,48,'z');
	}
#endif
#endif

#ifdef OLED_0P96_COLOR_PANEL
	ST7735_WriteChar_16x16(144,64,4,RGB565_Cyan,RGB565_Black);
	ST7735_WriteChar_16x16(128,64,3,RGB565_Cyan,RGB565_Black);
	ST7735_WriteChar_16x16(112,64,9,RGB565_Cyan,RGB565_Black);
	ST7735_WriteChar_16x16(96,64,10,RGB565_Cyan,RGB565_Black);
#ifdef DISP_FRAME_RATIO_30HZ
	ST7735_WriteChar_8x16(8,64,'3',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'0',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_25HZ
	ST7735_WriteChar_8x16(8,64,'2',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_20HZ
	ST7735_WriteChar_8x16(8,64,'2',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'0',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_16HZ
	ST7735_WriteChar_8x16(8,64,'1',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'6',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_15HZ
	ST7735_WriteChar_8x16(8,64,'1',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_5HZ
	ST7735_WriteChar_8x16(8,64,'0',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_40HZ
	ST7735_WriteChar_8x16(8,64,'4',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'0',RGB565_Lgray,RGB565_Black);
#endif
	ST7735_WriteChar_8x16(16,64,'w',RGB565_Lgray,RGB565_Black);
#endif


#ifdef OLED_1P3_COLOR_PANEL
	SSD1351_WriteChar_16x16(112,80,4,RGB565_Blue,RGB565_Black);
	SSD1351_WriteChar_16x16(96,80,3,RGB565_Blue,RGB565_Black);
	SSD1351_WriteChar_16x16(80,80,9,RGB565_Blue,RGB565_Black);
	SSD1351_WriteChar_16x16(64,80,10,RGB565_Blue,RGB565_Black);
#ifdef DISP_FRAME_RATIO_30HZ
	SSD1351_WriteChar_8x16(8,80,'3',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'0',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_25HZ
	SSD1351_WriteChar_8x16(8,80,'2',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_20HZ
	SSD1351_WriteChar_8x16(8,80,'2',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'0',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_16HZ
	SSD1351_WriteChar_8x16(8,80,'1',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'6',RGB565_Lgray,RGB565_Black);
#endif

#ifdef DISP_FRAME_RATIO_15HZ
	SSD1351_WriteChar_8x16(8,80,'1',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_5HZ
	SSD1351_WriteChar_8x16(8,80,'0',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_40HZ
	SSD1351_WriteChar_8x16(8,80,'4',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'0',RGB565_Lgray,RGB565_Black);
#endif
	SSD1351_WriteChar_8x16(120,64,'w',RGB565_Lgray,RGB565_Black);
#endif

	#ifdef OLED_0P96_BW_PANEL
#ifdef DISP_FRAME_RATIO_30HZ
	OLED_ShowCharR_8x16(120,48,'3');
	OLED_ShowCharR_8x16(112,48,'0');
#endif
#ifdef DISP_FRAME_RATIO_25HZ
	OLED_ShowCharR_8x16(120,48,'2');
	OLED_ShowCharR_8x16(112,48,'5');
#endif
#ifdef DISP_FRAME_RATIO_20HZ
	OLED_ShowCharR_8x16(120,48,'2');
	OLED_ShowCharR_8x16(112,48,'0');
#endif
#ifdef DISP_FRAME_RATIO_16HZ
	OLED_ShowCharR_8x16(120,48,'1');
	OLED_ShowCharR_8x16(112,48,'6');
#endif
#ifdef DISP_FRAME_RATIO_15HZ
	OLED_ShowCharR_8x16(120,48,'1');
	OLED_ShowCharR_8x16(112,48,'5');
#endif
#ifdef DISP_FRAME_RATIO_5HZ
	OLED_ShowCharR_8x16(120,48,'0');
	OLED_ShowCharR_8x16(112,48,'5');
#endif
#ifdef DISP_FRAME_RATIO_40HZ
	OLED_ShowCharR_8x16(120,48,'4');
	OLED_ShowCharR_8x16(112,48,'0');
#endif
	//OLED_ShowCharR_8x16(104,48,afft_idx[fft_proc_mode]);
	OLED_ShowCharR_8x16(104,48,'w');

	OLED_ShowPF_16x16(48,48,7);
	OLED_ShowPF_16x16(32,48,0);
	OLED_ShowPF_16x16(16,48,9);
	OLED_ShowPF_16x16(0,48,8);
	OLED_Refresh();
	#endif
}

void mic_apply_demo_stripeZ_ff_dot(void)
{
	u16 i;
	//u8 cPattern;
	//u8 x,y;
	//u8 f_len,i_len;
	u8 cur_fft_val;
	u8 e_cur_fft_val;
	//s16 cur_diff;
	u8 bar_width;
	u8 bar_w_cnt;
	//u8 draw_data;
	u16 offset;
	u16 i_offset;

#if (G_NUM_LEDS == 512)
	bar_width = 1;
#else
	bar_width = 128 / (G_NUM_LEDS/2);
#endif
	if(bar_width <= 0)
		return;
	//x = 0;
	for(i = FFT_USED_DATA_START; i < FFT_USED_DATA_TT_END; i++)
	{
		cur_fft_val = (u8)xFFT_out[i];
		e_cur_fft_val = cur_fft_val;
		#ifdef OLED_0P96_BW_PANEL
		if(cur_fft_val >= C_SHOW_POS_LIMIT)
		{
			cur_fft_val = C_SHOW_POS_LIMIT;
		}
		#endif
		#ifdef OLED_1P3_COLOR_PANEL
		if(e_cur_fft_val >= A_SHOW_POS_LIMIT)
		{
			e_cur_fft_val = A_SHOW_POS_LIMIT;
		}
		#endif
		#ifdef OLED_0P96_COLOR_PANEL
		if(e_cur_fft_val >= B_SHOW_POS_LIMIT)
		{
			e_cur_fft_val = B_SHOW_POS_LIMIT;
		}
		#endif
		#ifdef OLED_0P96_COLOR_PANEL
		i_offset = 160 - i*bar_width;
		#endif
		#ifdef OLED_0P96_BW_PANEL
		offset = 127 - i*bar_width;
		#endif
		#ifdef OLED_1P3_COLOR_PANEL
		i_offset = 127 - i*bar_width;
		#endif
		bar_w_cnt = 0;
		do
		{
			#ifdef OLED_1P3_COLOR_PANEL
			SSD1351_DrawVertical_Column((i_offset-bar_w_cnt),0,(u8)e_cur_fft_val,RGB565_Green);
			SSD1351_DrawPixel((i_offset-bar_w_cnt),(u8)falling_y_pos[i],RGB565_Maroon);
			#endif
			#ifdef OLED_0P96_COLOR_PANEL
			ST7735_DrawVertical_Column((i_offset-bar_w_cnt),0,(u8)e_cur_fft_val,RGB565_PeacockGreen);
			ST7735_DrawPoint((i_offset-bar_w_cnt),(u8)falling_y_pos[i],RGB565_Red);
			#endif
			#ifdef OLED_0P96_BW_PANEL
			//OLED_DrawX_Vertical_Column((offset-bar_w_cnt),0,(u8)cur_fft_val,(u8)ff_actives[i]);
			OLED_DrawX_Vertical_Column((offset-bar_w_cnt),0,(u8)cur_fft_val,(u8)falling_y_pos[i]);
			//OLED_DrawY_dot((offset-bar_w_cnt),0,(u8)flying_y_pos[i]);
			#endif
			bar_w_cnt++;
		}while(bar_w_cnt < (bar_width - 1));

		if(cur_fft_val > falling_y_pos[i])
			falling_y_pos[i] = cur_fft_val;
		if(falling_y_pos[i] > 8)
			falling_y_pos[i] -= 2;
		else
			falling_y_pos[i] = 0;

		//ff_actives[i] = falling_y_pos[i];

		if(cur_fft_val > FLY_LEVEL_POS)
			flying_y_pos[i] = cur_fft_val;
		if(flying_y_pos[i] <= SHOW_POS_LIMIT)
		{
			flying_y_pos[i] += 3;
			//ff_actives[i] = flying_y_pos[i];
			falling_y_pos[i] = flying_y_pos[i];
		}
		//x+=1;
	}

	#ifdef OLED_0P96_COLOR_PANEL
	ST7735_WriteChar_16x16(144,64,12,RGB565_Golden,RGB565_Black);
	ST7735_WriteChar_16x16(128,64,4,RGB565_Golden,RGB565_Black);
	ST7735_WriteChar_16x16(112,64,5,RGB565_Golden,RGB565_Black);
	ST7735_WriteChar_16x16(96,64,8,RGB565_Golden,RGB565_Black);
	#ifdef DISP_FRAME_RATIO_30HZ
	ST7735_WriteChar_8x16(8,64,'3',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'0',RGB565_Lgray,RGB565_Black);
	#endif
	#ifdef DISP_FRAME_RATIO_25HZ
	ST7735_WriteChar_8x16(8,64,'2',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'5',RGB565_Lgray,RGB565_Black);
	#endif
	#ifdef DISP_FRAME_RATIO_20HZ
	ST7735_WriteChar_8x16(8,64,'2',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'0',RGB565_Lgray,RGB565_Black);
	#endif
	#ifdef DISP_FRAME_RATIO_16HZ
	ST7735_WriteChar_8x16(8,64,'1',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'6',RGB565_Lgray,RGB565_Black);
	#endif
	#ifdef DISP_FRAME_RATIO_15HZ
	ST7735_WriteChar_8x16(8,64,'1',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'5',RGB565_Lgray,RGB565_Black);
	#endif
	#ifdef DISP_FRAME_RATIO_5HZ
	ST7735_WriteChar_8x16(8,64,'0',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'5',RGB565_Lgray,RGB565_Black);
	#endif
	#ifdef DISP_FRAME_RATIO_40HZ
	ST7735_WriteChar_8x16(8,64,'4',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'0',RGB565_Lgray,RGB565_Black);
	#endif
	ST7735_WriteChar_8x16(16,64,afft_idx[fft_proc_mode],RGB565_Lgray,RGB565_Black);
	#endif
	
	#ifdef OLED_1P3_COLOR_PANEL
	SSD1351_WriteChar_16x16(112,80,12,RGB565_Blue,RGB565_Black);
	SSD1351_WriteChar_16x16(96,80,4,RGB565_Blue,RGB565_Black);
	SSD1351_WriteChar_16x16(80,80,5,RGB565_Blue,RGB565_Black);
	SSD1351_WriteChar_16x16(64,80,8,RGB565_Blue,RGB565_Black);
	#ifdef DISP_FRAME_RATIO_30HZ
	SSD1351_WriteChar_8x16(8,80,'3',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'0',RGB565_Lgray,RGB565_Black);
	#endif
	#ifdef DISP_FRAME_RATIO_25HZ
	SSD1351_WriteChar_8x16(8,80,'2',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'5',RGB565_Lgray,RGB565_Black);
	#endif
	#ifdef DISP_FRAME_RATIO_20HZ
	SSD1351_WriteChar_8x16(8,80,'2',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'0',RGB565_Lgray,RGB565_Black);
	#endif
	#ifdef DISP_FRAME_RATIO_16HZ
	SSD1351_WriteChar_8x16(8,80,'1',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'6',RGB565_Lgray,RGB565_Black);
	#endif
	#ifdef DISP_FRAME_RATIO_15HZ
	SSD1351_WriteChar_8x16(8,80,'1',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'5',RGB565_Lgray,RGB565_Black);
	#endif
	#ifdef DISP_FRAME_RATIO_5HZ
	SSD1351_WriteChar_8x16(8,80,'0',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'5',RGB565_Lgray,RGB565_Black);
	#endif
	#ifdef DISP_FRAME_RATIO_40HZ
	SSD1351_WriteChar_8x16(8,80,'4',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'0',RGB565_Lgray,RGB565_Black);
	#endif
	SSD1351_WriteChar_8x16(120,64,afft_idx[fft_proc_mode],RGB565_Lgray,RGB565_Black);
	#endif

	#ifdef OLED_0P96_BW_PANEL
	#ifdef DISP_FRAME_RATIO_30HZ
	OLED_ShowCharR_8x16(120,48,'3');
	OLED_ShowCharR_8x16(112,48,'0');
	#endif
	#ifdef DISP_FRAME_RATIO_25HZ
	OLED_ShowCharR_8x16(120,48,'2');
	OLED_ShowCharR_8x16(112,48,'5');
	#endif
	#ifdef DISP_FRAME_RATIO_20HZ
	OLED_ShowCharR_8x16(120,48,'2');
	OLED_ShowCharR_8x16(112,48,'0');
	#endif
	#ifdef DISP_FRAME_RATIO_16HZ
	OLED_ShowCharR_8x16(120,48,'1');
	OLED_ShowCharR_8x16(112,48,'6');
	#endif
	#ifdef DISP_FRAME_RATIO_15HZ
	OLED_ShowCharR_8x16(120,48,'1');
	OLED_ShowCharR_8x16(112,48,'5');
	#endif
	#ifdef DISP_FRAME_RATIO_5HZ
	OLED_ShowCharR_8x16(120,48,'0');
	OLED_ShowCharR_8x16(112,48,'5');
	#endif
	#ifdef DISP_FRAME_RATIO_40HZ
	OLED_ShowCharR_8x16(120,48,'4');
	OLED_ShowCharR_8x16(112,48,'0');
	#endif

	OLED_ShowPF_16x16(48,48,12);
	OLED_ShowPF_16x16(32,48,7);
	OLED_ShowPF_16x16(16,48,6);
	OLED_ShowPF_16x16(0,48,10);
	OLED_ShowCharR_8x16(104,48,afft_idx[fft_proc_mode]);
	#endif

	#ifdef OLED_0P96_BW_PANEL
	OLED_Refresh();
	#endif
}

void mic_apply_demo_only_ff_dot(void)
{
	u16 i;
	u8 cur_fft_val;
	u8 e_cur_fft_val;
	u8 bar_width;
	u8 bar_w_cnt;
	u16 offset;
	u16 i_offset;
	#ifdef OLED_0P96_BW_PANEL
	//OLED_Clear_Buff();
	#endif
	#ifdef OLED_0P96_COLOR_PANEL
	ST7735_FillRectangle((160-128),0,160,80,RGB565_Black);
	#endif
	#ifdef OLED_1P3_COLOR_PANEL
	SSD1351_FillRectangle(0,0,128,80,RGB565_Black);
	#endif
#if (G_NUM_LEDS == 512)
	bar_width = 1;
#else
	bar_width = 128 / (G_NUM_LEDS/2);
#endif
	if(bar_width <= 0)
		return;
	//x = 0;
	for(i = FFT_USED_DATA_START; i < FFT_USED_DATA_TT_END; i++)
	{
		cur_fft_val = (u8)xFFT_out[i];
		e_cur_fft_val = cur_fft_val;
		#ifdef OLED_0P96_BW_PANEL
		if(cur_fft_val >= C_SHOW_POS_LIMIT)
		{
			cur_fft_val = C_SHOW_POS_LIMIT;
		}
		#endif
		#ifdef OLED_1P3_COLOR_PANEL
		if(e_cur_fft_val >= A_SHOW_POS_LIMIT)
		{
			e_cur_fft_val = A_SHOW_POS_LIMIT;
		}
		#endif
		#ifdef OLED_0P96_COLOR_PANEL
		if(e_cur_fft_val >= B_SHOW_POS_LIMIT)
		{
			e_cur_fft_val = B_SHOW_POS_LIMIT;
		}
		#endif
		#ifdef OLED_0P96_BW_PANEL
		offset = 127 - i*bar_width;
		#endif
		#ifdef OLED_0P96_COLOR_PANEL
		i_offset = 159 - i*bar_width;
		#endif
		#ifdef OLED_1P3_COLOR_PANEL
		i_offset = 127 - i*bar_width;
		#endif
		bar_w_cnt = 0;
		do
		{
			#ifdef OLED_1P3_COLOR_PANEL
			//SSD1351_DrawVertical_Column((i_offset-bar_w_cnt),0,e_cur_fft_val,RGB565_Black);
			SSD1351_DrawPixel((i_offset-bar_w_cnt),(u8)falling_y_pos[i],RGB565_Blue);
			#endif
			#ifdef OLED_0P96_COLOR_PANEL
			//ST7735_DrawVertical_Column((i_offset-bar_w_cnt),0,0,RGB565_Black);
			ST7735_DrawPoint((i_offset-bar_w_cnt),(u8)falling_y_pos[i],RGB565_Blue);
			#endif
			#ifdef OLED_0P96_BW_PANEL
			OLED_DrawX_dot((offset-bar_w_cnt),0,(u8)falling_y_pos[i]);
			//OLED_DrawY_dot((offset-bar_w_cnt),0,(u8)flying_y_pos[i]);
			#endif
			bar_w_cnt++;
		}while(bar_w_cnt < (bar_width - 1));

		if(cur_fft_val > falling_y_pos[i])
			falling_y_pos[i] = cur_fft_val;
		if(falling_y_pos[i] > 8)
			falling_y_pos[i] -= 2;
		else
			falling_y_pos[i] = 0;

		//ff_actives[i] = falling_y_pos[i];

		if(cur_fft_val > FLY_LEVEL_POS)
			flying_y_pos[i] = cur_fft_val;
		if(flying_y_pos[i] <= SHOW_POS_LIMIT)
		{
			flying_y_pos[i] += 3;
			//ff_actives[i] = flying_y_pos[i];
			falling_y_pos[i] = flying_y_pos[i];
		}
		//x+=1;
	}

#ifdef OLED_0P96_COLOR_PANEL
	ST7735_WriteChar_16x16(144,64,12,RGB565_Golden,RGB565_Black);
	ST7735_WriteChar_16x16(128,64,4,RGB565_Golden,RGB565_Black);
	ST7735_WriteChar_16x16(112,64,7,RGB565_Golden,RGB565_Black);
	ST7735_WriteChar_16x16(96,64,13,RGB565_Golden,RGB565_Black);
#ifdef DISP_FRAME_RATIO_30HZ
	ST7735_WriteChar_8x16(8,64,'3',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'0',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_25HZ
	ST7735_WriteChar_8x16(8,64,'2',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_20HZ
	ST7735_WriteChar_8x16(8,64,'2',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'0',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_16HZ
	ST7735_WriteChar_8x16(8,64,'1',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'6',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_15HZ
	ST7735_WriteChar_8x16(8,64,'1',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_5HZ
	ST7735_WriteChar_8x16(8,64,'0',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_40HZ
	ST7735_WriteChar_8x16(8,64,'4',RGB565_Lgray,RGB565_Black);
	ST7735_WriteChar_8x16(0,64,'0',RGB565_Lgray,RGB565_Black);
#endif
	ST7735_WriteChar_8x16(16,64,afft_idx[fft_proc_mode],RGB565_Lgray,RGB565_Black);
#endif


#ifdef OLED_1P3_COLOR_PANEL
	SSD1351_WriteChar_16x16(112,80,12,RGB565_Cyan,RGB565_Black);
	SSD1351_WriteChar_16x16(96,80,4,RGB565_Cyan,RGB565_Black);
	SSD1351_WriteChar_16x16(80,80,7,RGB565_Cyan,RGB565_Black);
	SSD1351_WriteChar_16x16(64,80,13,RGB565_Cyan,RGB565_Black);
#ifdef DISP_FRAME_RATIO_30HZ
	SSD1351_WriteChar_8x16(8,80,'3',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'0',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_25HZ
	SSD1351_WriteChar_8x16(8,80,'2',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_20HZ
	SSD1351_WriteChar_8x16(8,80,'2',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'0',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_16HZ
	SSD1351_WriteChar_8x16(8,80,'1',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'6',RGB565_Lgray,RGB565_Black);
#endif

#ifdef DISP_FRAME_RATIO_15HZ
	SSD1351_WriteChar_8x16(8,80,'1',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_5HZ
	SSD1351_WriteChar_8x16(8,80,'0',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'5',RGB565_Lgray,RGB565_Black);
#endif
#ifdef DISP_FRAME_RATIO_40HZ
	SSD1351_WriteChar_8x16(8,80,'4',RGB565_Lgray,RGB565_Black);
	SSD1351_WriteChar_8x16(0,80,'0',RGB565_Lgray,RGB565_Black);
#endif
	SSD1351_WriteChar_8x16(120,64,afft_idx[fft_proc_mode],RGB565_Lgray,RGB565_Black);
#endif

	#ifdef OLED_0P96_BW_PANEL
	#ifdef DISP_FRAME_RATIO_30HZ
	OLED_ShowCharR_8x16(120,48,'3');
	OLED_ShowCharR_8x16(112,48,'0');
	#endif
	#ifdef DISP_FRAME_RATIO_25HZ
	OLED_ShowCharR_8x16(120,48,'2');
	OLED_ShowCharR_8x16(112,48,'5');
	#endif
	#ifdef DISP_FRAME_RATIO_20HZ
	OLED_ShowCharR_8x16(120,48,'2');
	OLED_ShowCharR_8x16(112,48,'0');
	#endif
	#ifdef DISP_FRAME_RATIO_16HZ
	OLED_ShowCharR_8x16(120,48,'1');
	OLED_ShowCharR_8x16(112,48,'6');
	#endif
	#ifdef DISP_FRAME_RATIO_15HZ
	OLED_ShowCharR_8x16(120,48,'1');
	OLED_ShowCharR_8x16(112,48,'5');
	#endif
	#ifdef DISP_FRAME_RATIO_5HZ
	OLED_ShowCharR_8x16(120,48,'0');
	OLED_ShowCharR_8x16(112,48,'5');
	#endif
	#ifdef DISP_FRAME_RATIO_40HZ
	OLED_ShowCharR_8x16(120,48,'4');
	OLED_ShowCharR_8x16(112,48,'0');
	#endif

	OLED_ShowPF_16x16(48,48,12);
	OLED_ShowPF_16x16(32,48,7);
	OLED_ShowPF_16x16(16,48,4);
	OLED_ShowPF_16x16(0,48,13);
	OLED_ShowCharR_8x16(104,48,afft_idx[fft_proc_mode]);
	#endif
	#ifdef OLED_0P96_BW_PANEL
	OLED_Refresh();
	#endif
}

void mic_apply_demo_fft_wav(void)
{
	u16 i;
	u8 line_length;
	u8 bar_w_cnt;
	u16 offset;
	u16 i_offset, i_sx;
	u16 cur_fft_val;

	u16 sx,sy;

	#ifdef OLED_0P96_BW_PANEL
	OLED_Clear_Buff();
	#endif
	#ifdef OLED_0P96_COLOR_PANEL
	ST7735_FillRectangle((160-128),0,160,64,RGB565_Black);
	#endif

	#ifdef OLED_1P3_COLOR_PANEL
	SSD1351_FillRectangle(0,0,128,80,RGB565_Black);
	#endif

#if (G_NUM_LEDS == 256)||(G_NUM_LEDS == 512)
	line_length = 1;
	sy = (u16)xFFT_out[0];
	if(sy >= SHOW_POS_LIMIT)
	{
		sy = SHOW_POS_LIMIT;
	}
	#ifdef OLED_0P96_BW_PANEL
	sx = 127;
	//draw first dot
	OLED_DrawPoint(sx,sy);
	#endif
	#ifdef OLED_1P3_COLOR_PANEL
	sx = 127;
	//draw first dot
	SSD1351_DrawPixel(sx,sy,RGB565_Olive);
	#endif
	i_sx = 159;
	#ifdef OLED_0P96_COLOR_PANEL
	ST7735_DrawPoint(i_sx,sy,RGB565_Olive);
	#endif
	for(i = 1; i < FFT_USED_DATA_TT_END; i++)
	{
		cur_fft_val = (u16)xFFT_out[i];
		if(!cur_fft_val)
		{
			continue;
		}
		if(cur_fft_val >= SHOW_POS_LIMIT)
		{
			cur_fft_val = SHOW_POS_LIMIT;
		}
		{
			//draw line
			offset = 128 - i*line_length;
			#ifdef OLED_0P96_BW_PANEL
			//offset = 128 - i*line_length;
			OLED_DrawLine(sx,sy,offset,cur_fft_val);
			#endif
			#ifdef OLED_1P3_COLOR_PANEL
			//offset = 128 - i*line_length;
			SSD1351_DrawLine(sx,sy,offset,cur_fft_val,RGB565_Olive);
			#endif
			#ifdef OLED_0P96_COLOR_PANEL
			i_offset = 160 - i*line_length;
			ST7735_DrawLine(i_sx,sy,i_offset,cur_fft_val,RGB565_Olive);
			#endif
			sx = offset;
			i_sx = i_offset;
			sy = cur_fft_val;
		}
	}
#else
	line_length = 128 / (G_NUM_LEDS/2);
	
	sy = (u16)xFFT_out[0];
	if(sy >= SHOW_POS_LIMIT)
	{
		sy = SHOW_POS_LIMIT;
	}
	//offset = 128 - line_length;
	//draw first dot
	#ifdef OLED_0P96_BW_PANEL
	sx = 127;
	OLED_DrawPoint(sx,sy);
	#endif
	#ifdef OLED_1P3_COLOR_PANEL
	sx = 127;
	//draw first dot
	SSD1351_DrawPixel(sx,sy,RGB565_Olive);
	#endif
	#ifdef OLED_0P96_COLOR_PANEL
	i_sx = 159;
	ST7735_DrawPoint(i_sx,sy,RGB565_Olive);
	#endif
	for(i = 1; i < (G_NUM_LEDS/2); i++)
	{
		cur_fft_val = (u16)xFFT_out[i];
		if(cur_fft_val >= SHOW_POS_LIMIT)
		{
			cur_fft_val = SHOW_POS_LIMIT;
		}
	#if 0
		offset = 127 - i*line_length;
		//OLED_DrawPoint(offset,cur_fft_val);
		OLED_DrawLine(offset,32,offset,cur_fft_val);
	#else
		/*
		if(line_length == 1)
		{
			//draw a dot
			OLED_DrawPoint((127-i),cur_fft_val);
		}
		else
		*/
		{
			//draw line
			offset = 128 - i*line_length;
			#ifdef OLED_0P96_BW_PANEL
			//offset = 128 - i*line_length;
			OLED_DrawLine(sx,sy,offset,cur_fft_val);
			#endif
			#ifdef OLED_1P3_COLOR_PANEL
			//offset = 128 - i*line_length;
			SSD1351_DrawLine(sx,sy,offset,cur_fft_val,RGB565_Olive);
			#endif
			#ifdef OLED_0P96_COLOR_PANEL
			i_offset = 160 - i*line_length;
			ST7735_DrawLine(i_sx,sy,i_offset,cur_fft_val,RGB565_Olive);
			#endif
			sx = offset;
			i_sx = i_offset;
			sy = cur_fft_val;
		}
	#endif
	}
#endif

	#ifdef OLED_1P3_COLOR_PANEL
	draw_freq_info();
	#endif
	#ifdef OLED_0P96_COLOR_PANEL
	draw_freq_info_y();
	#endif
	#ifdef OLED_0P96_BW_PANEL
	draw_freq_info_x();
	#endif

#ifdef OLED_0P96_BW_PANEL
	OLED_Refresh();
#endif
}

void disp_shwo_loop_st_machine(void)
{
	//dbg_uart_print_str("dlp\r\n");
	switch(drs_current_st)
	{
		case DISP_RUN_ST_NONE:
			read_key();
			if(dummy_cnt >= ONE_SECOND_CNT_NUM)
			{
				dummy_cnt = 0;
				LED2_Toggle();
			}
			else
			{
				dummy_cnt++;
			}
			//start ADC sample
			mic_adc_start = 1;
			fft_sample_cnt = 0;
			#ifdef ADC_NORMAL_MODE
			//ADC_Cmd(ADC1,ENABLE);
			TIM_Cmd(TIM2, ENABLE);  //使能TIMx
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			#endif
			#ifdef ADC_TIM2_DMA_MODE
			app_c_adc_dma_start();
			#endif
			drs_current_st = DISP_RUN_ADC_ST;
			break;
		case DISP_RUN_ADC_ST:
			if(mic_adc_start)
			{
				;//do nothing
			}
			else
			{
				drs_current_st = DISP_RUN_FFT_ST;
			}
			break;
		case DISP_RUN_FFT_ST:
			mic_data_apply();
			print_o_c++;
			//drs_current_st = DISP_RUN_ST_NONE;
			drs_current_st = DISP_RUN_SHOW_ST;
			break;
		case DISP_RUN_SHOW_ST:
			oled_showing_update();
			is_disp_show_period = 0;
			drs_current_st = DISP_RUN_ST_NONE;
			break;
		default:
			break;
	}
}


