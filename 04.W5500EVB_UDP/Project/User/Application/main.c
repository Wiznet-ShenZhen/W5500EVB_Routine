#include "include.h"

extern uint16 local_port;				// ���ض˿�

int main()
{
  /***** MCUʱ�ӳ�ʼ�� *****/				  
	Systick_Init(72);	

	/***** �жϿ��������� *****/
	NVIC_Configuration();
	
	/***** GPIO��ʼ�� *****/
	GPIO_Configuration();	
	
  /***** SPI��ʼ�� *****/
	WIZ_SPI_Init();
	
	/***** ���ڳ�ʼ�� *****/
	USART1_Init(); 			
	
	/***** Ӳ����W5500 *****/
	Reset_W5500();
	
	/***** W5500��IP��Ϣ��ʼ�� *****/	
	set_default(); 
	set_network();														
	
	printf("UDP Local Port: %d \r\n",local_port); 
	printf("W5500EVB Init Complete!\r\n");
  printf("Start UDP Test!\r\n"); 	
	
	while(1)																												
	{
			do_udp();
	}
}
