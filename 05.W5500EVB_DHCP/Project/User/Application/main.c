#include "include.h"

int main()
{	
	/***** MCUʱ�ӳ�ʼ�� *****/				  
	Systick_Init(72);
  RCC_Configuration(); 	
	
	/***** ����Ƕ���ж����� *****/
	NVIC_Configuration();

	/***** ��ʱ����ʼ�� *****/
	Timer_Configuration();
	
	/***** GPIO��ʼ�� *****/ 
  GPIO_Configuration();
	
	/***** ��ʼ��SPI�ӿ� *****/  
	WIZ_SPI_Init();
	
	/***** ���ڳ�ʼ�� *****/
  USART1_Init(); 
  
	/***** Ӳ����W5500 *****/
  Reset_W5500();
  
	/***** DHCP��ʼ�� *****/
  set_default(); 	
  init_dhcp_client();

	printf("W5500EVB Init Complete!\r\n");
  printf("Start DHCP Test!\r\n"); 	
  while(1)
  {
		DHCP_run();
	}
}
