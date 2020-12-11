#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "udp.h"
#include <string.h>

extern uint8  remote_ip[4];			// 远程IP地址
extern uint16 remote_port;			// 远程端口
extern uint16 local_port;				// 本地端口

int main()
{
  /***** MCU时钟初始化 *****/				  
	Systick_Init(72);	
	
	/***** GPIO初始化 *****/
	GPIO_Configuration();	
	
  /***** SPI初始化 *****/
	WIZ_SPI_Init();
	
	/***** 串口初始化 *****/
	USART1_Init(); 			
	
	/***** 硬重启W5500 *****/
	Reset_W5500();
	
	/***** W5500的IP信息初始化 *****/	
	set_default(); 
	set_network();														
	
	printf("UDP Local Port: %d \r\n",local_port); 
	printf("W5500EVB Init Complete!\r\n");
  printf("Start UDP Test!\r\n"); 	
	
	while(1)																												
	{
			do_udp();
	}
}
