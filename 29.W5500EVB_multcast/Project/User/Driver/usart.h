#ifndef _USART_H_
#define _USART_H_
#include "stm32f10x.h"
#include <stdio.h>

#define USART_REC_LEN  		200  							// 定义串口接收缓存大小
#define EN_USART1_RX 			1									// 使能串口1接收，0为禁止

extern u8  USART_RX_BUF[USART_REC_LEN]; 		// 声明串口接收缓存 
extern u16 USART_RX_STA;         						// 声明串口接收状态标志位

void USART1_Init(void);
int fputc(int ch, FILE *f);
void USART1_Putc(unsigned char c);
void USART1_Puts(char * str);
void USART1_IRQHandler(void);
#endif 

