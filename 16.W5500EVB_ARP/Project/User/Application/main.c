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
	/***** MCUʱ�ӳ�ʼ�� *****/		
  RCC_Configuration();	
	Systick_Init(72);	
	
	/***** GPIO��ʼ�� *****/
	GPIO_Configuration();	
	
	/***** NVIC��ʼ�� *****/	
	NVIC_Configuration();
	
	/***** TIMER2��ʼ�� *****/
	Timer_Configuration();
	
  /***** SPI��ʼ�� *****/
	WIZ_SPI_Init();
	
	/***** ���ڳ�ʼ�� *****/
	USART1_Init(); 			
	
	/***** Ӳ����W5500 *****/
	Reset_W5500();
	
	/***** W5500��IP��Ϣ��ʼ�� *****/	
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
