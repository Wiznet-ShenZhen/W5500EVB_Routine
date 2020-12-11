#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "string.h"
#include "tcp.h"

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
	set_default(); 														
	set_network();														
	
	/***** 打开Keepalive功能 *****/
	setkeepalive(0);													
	
	printf("W5500EVB Init Complete!\r\n");
  printf("Start Testing Speed Test!\r\n"); 
  
	while(1)																			
	{	
		tcps();
	}
}


