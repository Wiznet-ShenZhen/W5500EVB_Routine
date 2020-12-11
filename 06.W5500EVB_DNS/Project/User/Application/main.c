#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "dns.h"
#include <string.h>

extern uint8 buffer[2048];												// 定义一个2KB的数组，用来存放Socket的通信数据
int main()
{	
  /***** MCU时钟初始化 *****/				  
	Systick_Init(72);	

	/***** 中断配置 *****/
	NVIC_Configuration();
	
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
	
	printf("W5500EVB Init Complete!\r\n");
  printf("Start DNS Test!\r\n"); 	
	printf("Input the domain name(eg>>www.baidu.com):\r\n");	
	
	while(1)																												
	{
		if(check_usart_buf())  //判断串口接收的数据
		{ 
		   do_dns(buffer);  // DNS过程
		}										
		
	}
}
