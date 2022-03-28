#include "stm32f10x.h"
#include "config.h"
#include "w5500.h"
#include "socket.h"
#include "spi2.h"
#include "lockback.h"
#include "dhcp.h"
#include "device.h"
#include "24c16.h"
#include "ult.h"
#include "usart.h"
#include "UPnP.h"
#include "hyperterminal.h"
#include "stdio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern uint8 txsize[];
extern uint8 rxsize[];	
uint8 buffer[2048];/*定义一个2KB的缓存*/
CONFIG_MSG Config_Msg;

uint8 TX_BUF[TX_RX_MAX_BUF_SIZE]; // TX Buffer for applications
uint8 RX_BUF[TX_RX_MAX_BUF_SIZE]; // RX Buffer for applications
uint8 C_Flag = 0;
//uint8 CONFLICT_Flag = 0;
uint8 buffer[2048];/*定义一个2KB的缓存*/
uint8 I_STATUS[MAX_SOCK_NUM];
uint8 Enable_DHCP =ON;
int main()
{
  uint8 dhcpret=0;
  GPIO_Configuration();/* 配置GPIO*/
  NVIC_Configuration();/* 配置嵌套中断向量*/
  
  Systick_Init(72);/* 初始化Systick工作时钟*/
  USART1_Init(); /*初始化串口通信:115200@8-n-1*/
  at24c16_init();/*初始化eeprom*/
  printf("W5500 EVB initialization over.\r\n");
  
  Reset_W5500();/*硬重启W5500*/
  WIZ_SPI_Init();/*初始化SPI接口*/
	set_w5500_mac();
  printf("W5500 initialized!\r\n"); 
  init_dhcp_client();
	while(1)
	{
		dhcpret = check_DHCP_state(SOCK_DHCP);
		switch(dhcpret)
		{
			case DHCP_RET_NONE:
			break;

			case DHCP_RET_TIMEOUT:
			break;

			case DHCP_RET_UPDATE:
					set_default();
					set_network();
					C_Flag = 1;
			break;

			case DHCP_RET_CONFLICT:
					while(1);
			default:
			break;
		}
		if(C_Flag == 1)
		{
	//	Timer_Configuration();
			do
			{
				printf("Send SSDP..\r\n");
			}
			while(SSDPProcess(SOCK_SSDP)!=0);
			// Try to Get Description of IGD
			if(GetDescriptionProcess(SOCK_UPNP)==0) 
				printf("GetDescription Success!!\r\n");
			else printf("GetDescription Fail!!\r\n");
			// Try to Subscribe Eventing
			if(SetEventing(SOCK_UPNP)==0) 
				printf("SetEventing Success!!\r\n");
			else 
				printf("SetEventing Fail!!\r\n");
			
			Main_Menu();
			
			while(1) {} 
		}
	}
}

