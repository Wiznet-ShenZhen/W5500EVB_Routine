#include "include.h"

extern uint8 txsize[];										// �����ⲿ����������Socket���ͻ����С
extern uint8 rxsize[];										// �����ⲿ����������Socket���ջ����С


int main(void)
{
  uint8 server_ip[4]={192,168,1,102};			// ����Զ�̷�����IP��ַ
	uint16 server_port=5000;								// ����Զ�̷������˿�
	
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
	
	printf("TCP Server IP: %d.%d.%d.%d \r\n",server_ip[0],server_ip[1],server_ip[2],server_ip[3]);
  printf("TCP Server Port: %d \r\n",server_port);	
	printf("W5500 Init Complete!\r\n");
  printf("Start TCP Client Test!\r\n"); 

	while(1)																				
	{	
		// Socket״̬��
		tcpc(server_ip);
	}
}


