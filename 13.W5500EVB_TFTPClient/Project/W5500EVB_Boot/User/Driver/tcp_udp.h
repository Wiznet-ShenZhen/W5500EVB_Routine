#ifndef __TCP_UDP_H
#define __TCP_UDP_H
#include "stm32f10x.h"
#include "Types.h"
extern uint8 demo_num;

void do_tcp_server(void);//TCP Server回环演示函数
void do_tcp_client(void);//TCP Clinet回环演示函数
void do_udp(void);
#endif 
