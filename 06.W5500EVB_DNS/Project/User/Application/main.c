#include "include.h"

int main()
{	
  /***** MCU时钟初始化 *****/				  
	Systick_Init(72);	

	/***** 配置嵌套中断向量 *****/
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
		   do_dns();  // DNS过程
		}										
		
	}
}
