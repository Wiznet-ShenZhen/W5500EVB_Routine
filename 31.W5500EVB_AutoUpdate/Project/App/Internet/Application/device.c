#include "config.h"
#include "mcu_init.h"
#include "device.h"
#include "socket.h"
#include "util.h"
#include "w5500.h"
#include "md5.h"

#include "tsensor.h"
#include "24c16.h"
#include "adc.h"
#include "usart.h"
#include "iap.h"

#include <stdio.h> 
#include <string.h>
//#define DEBUG_UPDATE
uint8 reboot_flag;
uint8 dhcp_fail_count;
uint8 ip_ok_flag = 1;
uint16 dhcp_retry_time = 100;
uint8  dhcp_retry_flag = 1;

uint8 DEFAULT_MAC[6] = { 0x00, 0x08, 0xdc, 0x11, 0x12, 0x13 };

uint8 DEFAULT_LIP[4] = { 192, 168, 1, 77 };
uint8 DEFAULT_MARK[4] = { 255, 255, 255, 0 };
uint8 DEFAULT_GW[4] = { 192, 168,1, 1 };
uint8 DEFAULT_DNS[4] = { 114,114,114,114 };
uint8 DEFAULT_RIP[4] = { 192,168,1,99 };

CONFIG_MSG  ConfigMsg, RecvMsg;

uint16 any_port = 3000;
uint8 txsize[MAX_SOCK_NUM] = { 2,2,2,2,2,2,2,2 };
uint8 rxsize[MAX_SOCK_NUM] = { 2,2,2,2,2,2,2,2 };

extern uint8 reboot_flag;

void reset_w5500(void)
{
	GPIO_ResetBits(GPIOB, WIZ_RESET);
	Delay_us(2);
	GPIO_SetBits(GPIOB, WIZ_RESET);
	Delay_ms(50);
}

//reboot app
void reboot_app(void)
{
	//if using this function, the wwdg should be disabled
	pFunction Jump_To_Application;
	uint32 JumpAddress;
	JumpAddress = *(vu32*)(ApplicationAddress + 0x00000004);
	Jump_To_Application = (pFunction)JumpAddress;
	//initialize user application's stack pointer
	__set_MSP(*(__IO uint32_t*)ApplicationAddress);
	Jump_To_Application();
}

//reboot 
void reboot(void)
{
	pFunction Jump_To_Application;
	uint32 JumpAddress;
	JumpAddress = *(vu32*)(0x00000004);
	Jump_To_Application = (pFunction)JumpAddress;
	Jump_To_Application();
}

void set_default(void)
{
	uint16 i;
	for (i = 0; i < (uint8)CONFIG_MSG_LEN_ALL - 27; i++)
	{
		*(ConfigMsg.sw_ver + i) = 0;//从版本号之后的开始清0
	}

	ConfigMsg.state = FW_APP_NORMAL;

	memcpy(ConfigMsg.net.ip, DEFAULT_LIP, 4);
	memcpy(ConfigMsg.net.sub, DEFAULT_MARK, 4);
	memcpy(ConfigMsg.net.gw, DEFAULT_GW, 4);
	memcpy(ConfigMsg.net.dns, DEFAULT_DNS, 4);
	memcpy(ConfigMsg.mac, DEFAULT_MAC, 6);

	ConfigMsg.debug_flag = 1;
	ConfigMsg.echo_flag = 1;
	//	ConfigMsg.sw_ver[2]= FW_VER_LOW;
	//  ConfigMsg.sw_ver[0]= FW_VER_HIGH;  
}


void write_config_to_eeprom(void)
{
	uint8 i, data;
	for (i = 0; i < (uint8)CONFIG_MSG_LEN; i++)
	{
		data = *(uint8*)(ConfigMsg.mac + i);
		at24c16_write(i, data);
	}
}
void get_config(void)
{
	uint16 i;
	for (i = 0; i < CONFIG_MSG_LEN; i++)
	{
		*(ConfigMsg.mac + i) = at24c16_read(i);
	}
}
void set_network(void)
{
	uint8 ip[4];
	uint8 mac[6];

	setSHAR(ConfigMsg.mac);
	setSIPR(ConfigMsg.net.ip);
	setSUBR(ConfigMsg.net.sub);
	setGAR(ConfigMsg.net.gw);

	//Init. TX & RX Memory size of w5500
	sysinit(txsize, rxsize);
	setRTR(2000);//200ms
	setRCR(3);

	getSIPR(ip);
	printf("IP:%d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);

	getSUBR(ip);
	printf("SN:%d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);

	getGAR(ip);
	printf("GW:%d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);

	getSHAR(mac);
	printf("MAC:%02x:%02x:%02x:%02x:%02x:%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}




