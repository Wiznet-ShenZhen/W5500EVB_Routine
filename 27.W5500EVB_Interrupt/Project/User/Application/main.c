#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "loopback.h"
#include <string.h>

extern uint8 dest_ip[4];
extern uint16 dest_port;

int main()
{
	uint16 local = 6000;
  /***** MCUʱ�ӳ�ʼ�� *****/				  
	Systick_Init(72);	
	
	/***** GPIO��ʼ�� *****/
	GPIO_Configuration();	
	RCC_Configuration();
	/***** ����Ƕ���ж����� *****/
	NVIC_Configuration();
	
  /***** SPI��ʼ�� *****/
	WIZ_SPI_Init();
	
	/***** ���ڳ�ʼ�� *****/
	USART1_Init(); 			
	
	/***** Ӳ����W5500 *****/
	Reset_W5500();
	
	/***** W5500��IP��Ϣ��ʼ�� *****/	
	set_default(); 
	set_network();	
	
	IINCHIP_WRITE(Sn_MR(7), 0x20);//TCPģʽ�¿�������ʱACK
  IINCHIP_WRITE(Sn_IMR(7), 0x0F);
  IINCHIP_WRITE(IMR, 0xF0);
  IINCHIP_WRITE(SIMR, 0xFE);

  printf("Remote sever %d.%d.%d.%d:%d\r\n",dest_ip[0],dest_ip[1],dest_ip[2],dest_ip[3],dest_port);
	printf("Local sever port:%d\r\n",local);
	printf("W5500EVB Init Complete!\r\n");
  printf("Start Interrupt Test!\r\n"); 	
	
	while(1)																												
	{		
		Delay_ms(500);
		loopback_tcps(7, 6000);		
		Delay_ms(500);
		loopback_tcpc(6, 5000);
	}
}
