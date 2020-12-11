#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "dns.h"
#include <string.h>

extern uint8 buffer[2048];												// ����һ��2KB�����飬�������Socket��ͨ������
int main()
{	
  /***** MCUʱ�ӳ�ʼ�� *****/				  
	Systick_Init(72);	

	/***** �ж����� *****/
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
		   do_dns(buffer);  // DNS����
		}										
		
	}
}
