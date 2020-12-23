#include "include.h"

uint16 local_port=5000;										// ����һ�����ض˿ڲ���ʼ�� 	

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
	set_default(); 														// ����Ĭ��MAC��IP��GW��SUB��DNS
	set_network();														// ���ó�ʼ��IP��Ϣ����ӡ����ʼ��8��Socket
	
	/***** ��W5500��Keepalive���� *****/
	setkeepalive(0);
	
	printf("TCP Server Port: %d\r\n",local_port);
	printf("W5500 Init Complete!\r\n");
  printf("Start Modbus TCP Server Test!\r\n"); 

	while(1)																	
	{
		do_Modbus();
	}
}

