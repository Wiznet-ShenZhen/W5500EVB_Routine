#ifndef _ULT_H
#define _ULT_H
#include"stdio.h"
#include "types.h"
#include "stm32f10x.h"


void Systick_Init (uint8 SYSCLK);
void Delay_s( uint32 time_s );
void Delay_us(uint32 time_us);
void Delay_ms(uint32 time_ms);


uint16 ATOI(char* str,uint16 base); 			/* Convert a string to integer number */
uint32 ATOI32(char* str,uint16 base); 			/* Convert a string to integer number */
void itoa(uint16 n,uint8* str, uint8 len);
int ValidATOI(char* str, int base, int* ret); 		/* Verify character string and Convert it to (hexa-)decimal. */
char C2D(u_char c); 					/* Convert a character to HEX */

uint16 swaps(uint16 i);
uint32 swapl(uint32 l);

void replacetochar(char * str, char oldchar, char newchar);

void mid(int8* src, int8* s1, int8* s2, int8* sub);
void inet_addr_(unsigned char* addr,unsigned char *ip);
#endif
