#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "string.h"
#include "arp.h"
#include "mcu_init.h"

int main()
{
	/***** MCU时钟初始化 *****/		
  RCC_Configuration();	
	Systick_Init(72);	
	
	/***** GPIO初始化 *****/
	GPIO_Configuration();	
	
	/***** NVIC初始化 *****/	
	NVIC_Configuration();
	
	/***** TIMER2初始化 *****/
	Timer_Configuration();
	
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
  printf("Start ARP Test!\r\n"); 	
	
	Delay_ms(1000);
	while(1)																												
	{
		 do_arp();
	}
}
