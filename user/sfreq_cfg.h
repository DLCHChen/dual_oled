/*
CFP
2022 - 
*/


#ifndef _SFREQ_CFG_H_
#define _SFREQ_CFG_H_
#include "board_cfg.h"
#include "disp_show.h"

#ifdef FFT_SAMPLE_FREG_2P56KHZ
#if (G_NUM_LEDS == 512)
#define BASE_FREQ	5
#define LDOT_POS	3
#define FFT_DIV_NOISE_TH	8
#define FFT_DB_NOISE_TH		2
#endif

#if (G_NUM_LEDS == 256)
#define BASE_FREQ	1
#define LDOT_POS	2
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 128)
#define BASE_FREQ	2
#define LDOT_POS	2
#define FFT_DIV_NOISE_TH	6
#define FFT_DB_NOISE_TH		24

#endif

#if (G_NUM_LEDS == 64)
#define BASE_FREQ	4
#define LDOT_POS	2
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		24

#endif

#if (G_NUM_LEDS == 32)
#define BASE_FREQ	8
#define LDOT_POS	2
#define FFT_DIV_NOISE_TH	0
#define FFT_DB_NOISE_TH		18

#endif

#endif

#ifdef FFT_SAMPLE_FREG_25P6KHZ
#if (G_NUM_LEDS == 512)
#define BASE_FREQ	5
#define LDOT_POS	2
#define FFT_DIV_NOISE_TH	8
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 256)
#define BASE_FREQ	1
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 128)
#define BASE_FREQ	2
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	6
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 64)
#define BASE_FREQ	4
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		3

#endif

#if (G_NUM_LEDS == 32)
#define BASE_FREQ	8
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	0
#define FFT_DB_NOISE_TH		14

#endif

#endif

#ifdef FFT_SAMPLE_FREG_6P4KHZ
#if (G_NUM_LEDS == 512)
#define BASE_FREQ	125
#define LDOT_POS	4
#define FFT_DIV_NOISE_TH	8
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 256)
#define BASE_FREQ	25
#define LDOT_POS	3
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 128)
#define BASE_FREQ	5
#define LDOT_POS	2
#define FFT_DIV_NOISE_TH	6
#define FFT_DB_NOISE_TH		24

#endif

#if (G_NUM_LEDS == 64)
#define BASE_FREQ	1
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		24

#endif

#if (G_NUM_LEDS == 32)
#define BASE_FREQ	2
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	0
#define FFT_DB_NOISE_TH		18

#endif

#endif

#ifdef FFT_SAMPLE_FREG_12P8KHZ
#if (G_NUM_LEDS == 512)
#define BASE_FREQ	25
#define LDOT_POS	3
#define FFT_DIV_NOISE_TH	8
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 256)
#define BASE_FREQ	5
#define LDOT_POS	2
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 128)
#define BASE_FREQ	1
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	6
#define FFT_DB_NOISE_TH		3

#endif

#if (G_NUM_LEDS == 64)
#define BASE_FREQ	2
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		4

#endif

#if (G_NUM_LEDS == 32)
#define BASE_FREQ	4
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	0
#define FFT_DB_NOISE_TH		18

#endif

#endif

#ifdef FFT_SAMPLE_FREG_19P2KHZ
#if (G_NUM_LEDS == 512)
#define BASE_FREQ	375
#define LDOT_POS	4
#define FFT_DIV_NOISE_TH	8
#define FFT_DB_NOISE_TH		4

#endif

#if (G_NUM_LEDS == 256)
#define BASE_FREQ	75
#define LDOT_POS	3
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		4

#endif

#if (G_NUM_LEDS == 128)
#define BASE_FREQ	15
#define LDOT_POS	2
#define FFT_DIV_NOISE_TH	6
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 64)
#define BASE_FREQ	3
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		5

#endif

#if (G_NUM_LEDS == 32)
#define BASE_FREQ	6
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	0
#define FFT_DB_NOISE_TH		18

#endif

#endif

#ifdef FFT_SAMPLE_FREG_38P4KHZ
#if (G_NUM_LEDS == 512)
#define BASE_FREQ	75
#define LDOT_POS	3
#define FFT_DIV_NOISE_TH	8
#define FFT_DB_NOISE_TH		4//10

#endif

#if (G_NUM_LEDS == 256)
#define BASE_FREQ	15
#define LDOT_POS	2
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		4//10

#endif

#if (G_NUM_LEDS == 128)
#define BASE_FREQ	3
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	6
#define FFT_DB_NOISE_TH		6

#endif

#if (G_NUM_LEDS == 64)
#define BASE_FREQ	6
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		8

#endif

#if (G_NUM_LEDS == 32)
#define BASE_FREQ	120
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	0
#define FFT_DB_NOISE_TH		18

#endif

#endif

#ifdef FFT_SAMPLE_FREG_16KHZ
#if (G_NUM_LEDS == 512)
#define BASE_FREQ	3125
#define LDOT_POS	5
#define FFT_DIV_NOISE_TH	8
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 256)
#define BASE_FREQ	625
#define LDOT_POS	4
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 128)
#define BASE_FREQ	125
#define LDOT_POS	3
#define FFT_DIV_NOISE_TH	6
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 64)
#define BASE_FREQ	25
#define LDOT_POS	2
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		2

#endif

#if (G_NUM_LEDS == 32)
#define BASE_FREQ	5
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	8
#define FFT_DB_NOISE_TH		2

#endif
#endif


#ifdef FFT_SAMPLE_FREG_32KHZ
#if (G_NUM_LEDS == 512)
#define BASE_FREQ	625
#define LDOT_POS	4
#define FFT_DIV_NOISE_TH	8
#define FFT_DB_NOISE_TH		6

#endif

#if (G_NUM_LEDS == 256)
#define BASE_FREQ	125
#define LDOT_POS	3
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		6

#endif

#if (G_NUM_LEDS == 128)
#define BASE_FREQ	25
#define LDOT_POS	2
#define FFT_DIV_NOISE_TH	6
#define FFT_DB_NOISE_TH		6

#endif

#if (G_NUM_LEDS == 64)
#define BASE_FREQ	5
#define LDOT_POS	1
#define FFT_DIV_NOISE_TH	9
#define FFT_DB_NOISE_TH		6

#endif

#if (G_NUM_LEDS == 32)
#define BASE_FREQ	1
#define LDOT_POS	0
#define FFT_DIV_NOISE_TH	0
#define FFT_DB_NOISE_TH		8

#endif
#endif


#endif

