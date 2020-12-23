#include "include.h"

int main()
{	
	/***** MCU时钟初始化 *****/				  
	Systick_Init(72);
  RCC_Configuration(); 	
	
	/***** 配置嵌套中断向量 *****/
	NVIC_Configuration();

	/***** 定时器初始化 *****/
	Timer_Configuration();
	
	/***** GPIO初始化 *****/ 
  GPIO_Configuration();
	
	/***** 初始化SPI接口 *****/  
	WIZ_SPI_Init();
	
	/***** 串口初始化 *****/
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
