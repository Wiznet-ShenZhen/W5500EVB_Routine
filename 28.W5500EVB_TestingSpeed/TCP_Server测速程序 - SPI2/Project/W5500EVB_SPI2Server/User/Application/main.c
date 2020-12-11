#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "string.h"
#include "tcp.h"

int main(void)
{	
  /***** MCUʱ�ӳ�ʼ�� *****/							  
	Systick_Init(72);	
		
	/***** GPIO��SPI��ʼ�� *****/
	GPIO_Configuration();			
	WIZ_SPI_Init();
	
	/***** ���ڳ�ʼ�� *****/
	USART1_Init(); 		
	
	/***** Ӳ����W5500 *****/
	Reset_W5500();
	
	/***** W5500��IP��Ϣ��ʼ�� *****/
	set_default(); 														
	set_network();														
	
	/***** ��Keepalive���� *****/
	setkeepalive(0);													
	
	printf("W5500EVB Init Complete!\r\n");
  printf("Start Testing Speed Test!\r\n"); 
  
	while(1)																			
	{	
		tcps();
	}
}


