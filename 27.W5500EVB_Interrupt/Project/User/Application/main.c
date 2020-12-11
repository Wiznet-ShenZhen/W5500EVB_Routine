#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "loopback.h"
#include <string.h>

extern uint8 dest_ip[4];
extern uint16 dest_port;

int main()
{
	uint16 local = 6000;
  /***** MCU时钟初始化 *****/				  
	Systick_Init(72);	
	
	/***** GPIO初始化 *****/
	GPIO_Configuration();	
	RCC_Configuration();
	/***** 配置嵌套中断向量 *****/
	NVIC_Configuration();
	
  /***** SPI初始化 *****/
	WIZ_SPI_Init();
	
	/***** 串口初始化 *****/
	USART1_Init(); 			
	
	/***** 硬重启W5500 *****/
	Reset_W5500();
	
	/***** W5500的IP信息初始化 *****/	
	set_default(); 
	set_network();	
	
	IINCHIP_WRITE(Sn_MR(7), 0x20);//TCP模式下开启无延时ACK
  IINCHIP_WRITE(Sn_IMR(7), 0x0F);
  IINCHIP_WRITE(IMR, 0xF0);
  IINCHIP_WRITE(SIMR, 0xFE);

  printf("Remote sever %d.%d.%d.%d:%d\r\n",dest_ip[0],dest_ip[1],dest_ip[2],dest_ip[3],dest_port);
	printf("Local sever port:%d\r\n",local);
	printf("W5500EVB Init Complete!\r\n");
  printf("Start Interrupt Test!\r\n"); 	
	
	while(1)																												
	{		
		Delay_ms(500);
		loopback_tcps(7, 6000);		
		Delay_ms(500);
		loopback_tcpc(6, 5000);
	}
}
