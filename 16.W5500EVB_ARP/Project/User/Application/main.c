#include "include.h"

int main()
{
	/***** MCUʱ�ӳ�ʼ�� *****/	
	Systick_Init(72);	
	RCC_Configuration();
	
	/***** GPIO��ʼ�� *****/
	GPIO_Configuration();	
	
	/***** ����Ƕ���ж����� *****/
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
		
	while(1)																												
	{
		 do_arp();
	}
}
