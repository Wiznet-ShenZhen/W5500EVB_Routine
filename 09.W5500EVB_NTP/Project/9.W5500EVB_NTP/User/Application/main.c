#include "include.h"

extern uint8 txsize[];
extern uint8 rxsize[];
//extern const	uint8 str1; 
uint8 buffer[2048];/*定义一个2KB的缓存*/  
int main()
{
  Systick_Init(72);/* 初始化Systick工作时钟*/ 
  GPIO_Configuration();/* 配置GPIO*/
  USART1_Init(); /*初始化串口通信:115200@8-n-1*/
  printf("W5500 EVB initialization over.\r\n");
 
  Reset_W5500();/*硬重启W5500*/
  WIZ_SPI_Init();/*初始化SPI接口*/
  printf("W5500 initialized!\r\n"); 
	set_default(); 	
	set_network(); 
  ntpclient_init(); 
  while(1)
  {
    do_ntp_client();
  }
}

