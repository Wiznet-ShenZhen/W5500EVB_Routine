#ifndef _TIMER_H_
#define _TIMER_H_
#include "stm32f10x.h"
#include "Types.h"

extern uint32 inact;//uint: s
extern uint16 recon;//uint: ms
extern uint16 plus_wait_count;
extern uint16 nagle_time_count;
extern uint8 set_default_count;
extern uint32 dns_wait_time;

void Timer_Configuration(void);
void Timer2_ISR(void);
#endif 

