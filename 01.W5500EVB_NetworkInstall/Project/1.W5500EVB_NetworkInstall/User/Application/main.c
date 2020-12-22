#include "include.h"

int main(void)
{
	/***** MCUʱ�ӳ�ʼ�� *****/							  
	Systick_Init(72);				
	
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
		
	printf("W5500EVB Init Complete!\r\n");
  printf("Start Ping Test!\r\n");

	while(1)
	{}
}

