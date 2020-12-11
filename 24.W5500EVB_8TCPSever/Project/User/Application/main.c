#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "tcp.h"
#include <string.h>

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
	
	printf("W5500EVB Init Complete!\r\n");
  printf("Start 8TCPSever Test!\r\n"); 	
	
	while(1)																												
	{
			do_8tcps();
	}
}