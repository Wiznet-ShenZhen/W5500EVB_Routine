#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"

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
		
	printf("W5500EVB Init Complete!\r\n");
  printf("Start Ping Test!\r\n");

	while(1)
	{}
}

