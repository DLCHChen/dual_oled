/*
CFP
2022 - 
*/


#ifndef _DEBUG_H_
#define _DEBUG_H_

#define DEBUG_BUF_LEN		16

void debug_data_in(uint8_t data);
void dbg_uart_print_str(char *pd);
void dbgu_send_char_uart(uint8_t data);
void dbgu_PrintHex(uint8_t x);
void dbgu_PrintLongInt(int x);
void dbgu_PrintLongIntHex(uint32_t x);

void delay_ms(u32 Nms);
void delay_us(u32 Nus);
void delay_init(u32 SYSCLK);


#endif

