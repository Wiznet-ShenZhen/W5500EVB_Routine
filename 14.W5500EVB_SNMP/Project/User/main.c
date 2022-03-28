#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stdio.h"
#include "usart.h"
#include "config.h"
#include "ult.h"
#include "w5500.h"
#include "socket.h"
#include "spi2.h"
#include "snmpLib.h"
#include "snmpDemo.h"
#include "device.h"
#include "24c16.h"
#include <stdio.h>
#include <string.h>
//#include <time.h>

CONFIG_MSG Config_Msg;
CHCONFIG_TYPE_DEF Chconfig_Type_Def;

uint8 ch_status[MAX_SOCK_NUM] = { 0, };	/** 0:close, 1:ready, 2:connected */

uint8 TX_BUF[TX_RX_MAX_BUF_SIZE]; // TX Buffer for applications
uint8 RX_BUF[TX_RX_MAX_BUF_SIZE]; // RX Buffer for applications

void Led(void)
{

}

int main(void)
{
	uint8 m = 0;
	Systick_Init(72);/* 初始化Systick工作时钟*/
	GPIO_Configuration();/* 配置GPIO*/

  //  Timer_Configuration();

	USART1_Init(); /*初始化串口通信:115200@8-n-1*/
	at24c16_init();/*初始化eeprom*/
	printf("W5500 EVB initialization over.\r\n");

	Reset_W5500();/*硬重启W5500*/
	WIZ_SPI_Init();/*初始化SPI接口*/
	printf("W5500 initialized!\r\n");
	set_default();
	set_network();
	while (1)
	{
		for (m = 0; m < 10; m++)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_0);
			Delay_ms(100);
			GPIO_ResetBits(GPIOA, GPIO_Pin_0);
			Delay_ms(100);
		}

		UserSnmpDemo();
	}
}
