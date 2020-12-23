#include "include.h"

uint16 local_port=5000;										// 定义一个本地端口并初始化 	

int main(void)
{  												
	/***** MCU时钟初始化 *****/							  
	Systick_Init(72);		
	
	/***** GPIO、SPI初始化 *****/
	GPIO_Configuration();			
	WIZ_SPI_Init();
	
	/***** 串口初始化 *****/
	USART1_Init(); 			
	
	/***** 硬重启W5500 *****/
	Reset_W5500();
	
	/***** W5500的IP信息初始化 *****/
	set_default(); 														// 设置默认MAC、IP、GW、SUB、DNS
	set_network();														// 配置初始化IP信息并打印，初始化8个Socket
	
	/***** 打开W5500的Keepalive功能 *****/
	setkeepalive(0);
	
	printf("TCP Server Port: %d\r\n",local_port);
	printf("W5500 Init Complete!\r\n");
  printf("Start Modbus TCP Server Test!\r\n"); 

	while(1)																	
	{
		do_Modbus();
	}
}

