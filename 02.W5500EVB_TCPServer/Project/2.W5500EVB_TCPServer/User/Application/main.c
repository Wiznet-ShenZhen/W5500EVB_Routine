#include "include.h"

extern uint8 txsize[];										// �����ⲿ����������Socket���ͻ����С
extern uint8 rxsize[];										// �����ⲿ����������Socket���ջ����С

uint8 buffer[2048];												// ����һ��2KB�����飬�������Socket��ͨ������

int main(void)
{
  uint16 local_port=5000;												// ����һ�����ض˿ڲ���ʼ��													

	/***** MCUʱ�ӳ�ʼ�� *****/							  
	Systick_Init(72);	
	
	/***** �жϿ��������� *****/
	NVIC_Configuration();
	
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
	printf("W5500EVB Init Complete!\r\n");
  printf("Start TCP Server Test!\r\n"); 


	while(1)																	
	{
   /***** socket״̬�� *****/
		tcps();
	}
}

