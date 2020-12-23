#include "include.h"

int main()
{	
  /***** MCUʱ�ӳ�ʼ�� *****/				  
	Systick_Init(72);	

	/***** ����Ƕ���ж����� *****/
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
	
	printf("W5500EVB Init Complete!\r\n");
  printf("Start DNS Test!\r\n"); 	
	printf("Input the domain name(eg>>www.baidu.com):\r\n");	
	
	while(1)																												
	{
		if(check_usart_buf())  //�жϴ��ڽ��յ�����
		{ 
		   do_dns();  // DNS����
		}										
		
	}
}
