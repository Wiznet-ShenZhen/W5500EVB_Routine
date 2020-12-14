#include <stm32f10x.h>
#include "mcu_init.h"
#include "config.h"
#include "device.h"
#include "spi2.h"
#include "socket.h"
#include "w5500.h"
#include "util.h"
#include "dhcp.h"
#include "string.h"
#include <stdio.h>

int main()
{
	/***** ���õ�Ƭ��ϵͳʱ�� *****/
  RCC_Configuration(); 
	
	/***** ����Ƕ���ж����� *****/
	NVIC_Configuration();
	
	/***** ��ʼ��Systick����ʱ�� *****/
	Systick_Init(72);
	
	/***** ����GPIO *****/ 
  GPIO_Configuration();
	
	/***** ��ʼ��SPI�ӿ� *****/  
	WIZ_SPI_Init();
	
	/***** ��ʱ����ʼ�� *****/
	Timer_Configuration();
	
	/***** ��ʼ������ͨ��:115200@8-n-1 *****/
  USART1_Init(); 
  
	/***** Ӳ����W5500 *****/
  Reset_W5500();
  
	/***** DHCP��ʼ�� *****/
  set_default(); 	
  init_dhcp_client();

	printf("W5500EVB Init Complete!\r\n");
  printf("Start DHCP Test!\r\n"); 	
  while(1)
  {
		DHCP_run();
	}
}
