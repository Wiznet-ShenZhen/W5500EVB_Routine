#include "include.h"

extern uint8 txsize[];
extern uint8 rxsize[];
//extern const	uint8 str1; 
uint8 buffer[2048];/*����һ��2KB�Ļ���*/  
int main()
{
  Systick_Init(72);/* ��ʼ��Systick����ʱ��*/ 
  GPIO_Configuration();/* ����GPIO*/
  USART1_Init(); /*��ʼ������ͨ��:115200@8-n-1*/
  printf("W5500 EVB initialization over.\r\n");
 
  Reset_W5500();/*Ӳ����W5500*/
  WIZ_SPI_Init();/*��ʼ��SPI�ӿ�*/
  printf("W5500 initialized!\r\n"); 
	set_default(); 	
	set_network(); 
  ntpclient_init(); 
  while(1)
  {
    do_ntp_client();
  }
}

