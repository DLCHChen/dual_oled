/*
CFP owner
2022
*/

#include "board_cfg.h"
#include "disp_show.h"
#include "sfreq_cfg.h"
#include "debug.h"
#include "fft.h"
#include "iic_oled.h"
#include "spi_oled.h"

#include "tim.h"
#include "ex_key_proc.h"


uint8_t key_buf_q[KEY_BUF_Q_MAX+1];
uint8_t key_in_cnt;
uint8_t key_in_pos;
uint8_t key_out_pos;

void key_in_init(void)
{
	key_in_cnt = 0;
	key_in_pos = 0;
	key_out_pos = 0;
}

void key_input(uint8_t key_val)
{
	if(key_in_cnt < KEY_BUF_Q_MAX)
	{
		key_in_cnt++;
		key_buf_q[key_in_pos] = key_val;
		key_in_pos++;
		if(key_in_pos >= KEY_BUF_Q_MAX)
		{
			key_in_pos = 0;
		}
	}
}

uint8_t key_output(void)
{
	uint8_t key_val = 0xff;
	if(key_in_cnt)
	{
		key_in_cnt--;
		key_val = key_buf_q[key_out_pos];
		key_buf_q[key_out_pos] =  0xff;
		key_out_pos++;
		if(key_out_pos >= KEY_BUF_Q_MAX)
		{
			key_out_pos = 0;
		}
	}
	//else
	//{
	//	return 0xff;
	//}
	return key_val;
}

void key_in_proc(void)
{
	uint8_t key_val;
	key_val = key_output();
	if(key_val == 0xff)
	{
		return;
	}

	switch(key_val)
	{
		case KEY_USER:
			dbg_uart_print_str("uki\r\n");
			Set_Mode_roll();
			break;
		default:
			break;
	}
}

u16 rKey_st;
u16 rKey_repeat_cnt;

void read_key(void)
{
	if(GPIO_ReadInputDataBit(KEY_GPIO_PORT , KEY_GPIO_PIN))
	{
		//release
		rKey_st = 1;
		if(rKey_repeat_cnt >= 1)
		{
			key_input(KEY_USER);
		}
		rKey_repeat_cnt = 0;
	}
	else
	{
		//press
		if(!rKey_st)
		{
			rKey_repeat_cnt++;
		}
		rKey_st = 0;
	}
}


