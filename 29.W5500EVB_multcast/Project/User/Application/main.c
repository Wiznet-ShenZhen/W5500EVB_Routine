#include "include.h"

int main()
{
  /***** MCU时钟初始化 *****/				  
	Systick_Init(72);	

	/***** 中断控制器配置 *****/
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
  printf("Start Multcast Test!\r\n"); 	
	
	while(1)																												
	{
			do_multcast();
	}
}
