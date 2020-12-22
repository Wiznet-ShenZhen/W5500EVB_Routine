#include "include.h"

extern uint8 txsize[];
extern uint8 rxsize[];
//extern const	uint8 str1; 
uint8 buffer[2048];/*定义一个2KB的缓存*/  
int main()
{
	/***** MCU时钟初始化 *****/
  Systick_Init(72);
	
	/***** GPIO初始化 *****/
  GPIO_Configuration();
	
	/***** 串口初始化 *****/
  USART1_Init(); 
  printf("W5500 EVB initialization over.\r\n");
 
	/***** 硬重启W5500 *****/
  Reset_W5500();
	
	/***** SPI初始化 *****/
  WIZ_SPI_Init();
  printf("W5500 initialized!\r\n"); 
	
	/***** W5500的IP信息初始化 *****/	
	set_default(); 	
	set_network(); 
	
	/***** NTP初始化 *****/
  ntpclient_init(); 
  while(1)
  {
    do_ntp_client();
  }
}

