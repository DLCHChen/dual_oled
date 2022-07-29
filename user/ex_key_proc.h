/*
CFP
2022
*/

#ifndef __EX_KEY_PROC_H__
#define __EX_KEY_PROC_H__

typedef enum {
	KEY_NULL,
	KEY_USER,
	KEY_433_REMOTE,
	KEY_MAX
} KEY_DEF_TF;

#define KEY_BUF_Q_MAX   5

void key_in_init(void);
void key_input(uint8_t key_val);
uint8_t key_output(void);
void key_in_proc(void);
void read_key(void);

#endif

