#ifndef _USART_H_
#define _USART_H_
#include "stm32f10x.h"
#include "Types.h"
#include <stdio.h>

void USART1_115200(void);
int fputc(int ch, FILE *f);
int get_char(void);
#endif 

