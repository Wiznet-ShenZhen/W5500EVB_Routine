#include <stm32f10x.h>
#include "mcu_init.h"
#include "config.h"
#include "device.h"
#include "spi2.h"
#include "socket.h"
#include "w5500.h"
#include "util.h"
#include "dhcp.h"
#include "string.h"
#include <stdio.h>

int main()
{
	/***** 配置单片机系统时钟 *****/
  RCC_Configuration(); 
	
	/***** 配置嵌套中断向量 *****/
	NVIC_Configuration();
	
	/***** 初始化Systick工作时钟 *****/
	Systick_Init(72);
	
	/***** 配置GPIO *****/ 
  GPIO_Configuration();
	
	/***** 初始化SPI接口 *****/  
	WIZ_SPI_Init();
	
	/***** 定时器初始化 *****/
	Timer_Configuration();
	
	/***** 初始化串口通信:115200@8-n-1 *****/
  USART1_Init(); 
  
	/***** 硬重启W5500 *****/
  Reset_W5500();
  
	/***** DHCP初始化 *****/
  set_default(); 	
  init_dhcp_client();

	printf("W5500EVB Init Complete!\r\n");
  printf("Start DHCP Test!\r\n"); 	
  while(1)
  {
		DHCP_run();
	}
}
