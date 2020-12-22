#include "include.h"

extern uint8 txsize[];
extern uint8 rxsize[];
//extern const	uint8 str1; 
uint8 buffer[2048];/*����һ��2KB�Ļ���*/  
int main()
{
	/***** MCUʱ�ӳ�ʼ�� *****/
  Systick_Init(72);
	
	/***** GPIO��ʼ�� *****/
  GPIO_Configuration();
	
	/***** ���ڳ�ʼ�� *****/
  USART1_Init(); 
  printf("W5500 EVB initialization over.\r\n");
 
	/***** Ӳ����W5500 *****/
  Reset_W5500();
	
	/***** SPI��ʼ�� *****/
  WIZ_SPI_Init();
  printf("W5500 initialized!\r\n"); 
	
	/***** W5500��IP��Ϣ��ʼ�� *****/	
	set_default(); 	
	set_network(); 
	
	/***** NTP��ʼ�� *****/
  ntpclient_init(); 
  while(1)
  {
    do_ntp_client();
  }
}

