#ifndef _USART_H_
#define _USART_H_
#include "stm32f10x.h"
#include <stdio.h>

void USART1_Init(void);
int fputc(int ch, FILE *f);
void USART1_Putc(unsigned char c);
void USART1_Puts(char * str);
void USART1_IRQHandler(void);
#endif 

