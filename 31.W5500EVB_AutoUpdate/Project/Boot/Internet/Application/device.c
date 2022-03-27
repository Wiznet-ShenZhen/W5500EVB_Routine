#include "device.h"
#include "config.h"
#include "mcu_init.h"
#include "usart.h"
#include "socket.h"
#include "util.h"
#include "w5500.h"
#include "24c16.h"
//#include "crc.h"
#include "iap.h"
#include <stdio.h> 
#include <string.h>

uint8 DEFAULT_MAC[6] = { 0x00, 0x08, 0xdc, 0x11, 0x12, 0x13 };

uint8 DEFAULT_LIP[4] = { 192, 168, 1, 77 };
uint8 DEFAULT_SN[4] = { 255, 255, 255, 0 };
uint8 DEFAULT_GW[4] = { 192, 168, 1, 1 };
uint8 DEFAULT_DNS[4] = { 114,114,114,114 };
uint8 reboot_flag;

CONFIG_MSG  ConfigMsg, RecvMsg;

uint16 any_port = 3000;

uint8 txsize[MAX_SOCK_NUM] = { 2,2,2,2,2,2,2,2 };
uint8 rxsize[MAX_SOCK_NUM] = { 2,2,2,2,2,2,2,2 };
//public buffer for DHCP, DNS, HTTP
uint8 BUFPUB[1024];

CONFIG_MSG  ConfigMsg, RecvMsg;
WORK_STATE  WorkState;
extern uint8 reboot_flag;

void reset_w5500(void)
{
	GPIO_ResetBits(GPIOB, WIZ_RESET);
	Delay_us(2);
	GPIO_SetBits(GPIOB, WIZ_RESET);
	Delay_ms(50);
}

/* Private functions */
void reboot_app(void)
{
	uint16 i;
	pFunction Jump_To_Application;
	uint32 JumpAddress;
	for (i = 0; i < 1000; i++);//wait for a while
	JumpAddress = *(vu32*)(ApplicationAddress + 4);

	Jump_To_Application = (pFunction)JumpAddress;
	//initialize user application's stack pointer
	//__set_MSP(*(__IO uint32_t *)ApplicationAddress);
	  //RCC_DeInit();
	__disable_irq();
	__set_MSP(*(__IO uint32_t*)ApplicationAddress);
	Jump_To_Application();
}
void reboot_back(void)
{

	uint16 i;
	pFunction Jump_To_Application;
	uint32 JumpAddress;
	for (i = 0; i < 1000; i++);//wait for a while
	JumpAddress = *(vu32*)(AppBackupAddress + 4);

	Jump_To_Application = (pFunction)JumpAddress;
	printf("启动backup\r\n");
	//RCC_DeInit();
  //NVIC_DeInit();
  //initialize user application's stack pointer
	//__set_PSP(*(__IO uint32_t *)AppBackupAddress);
	//__set_CONTROL(0);
	__set_MSP(*(__IO uint32_t*)AppBackupAddress);

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
	uint8_t i;
	for (i = 0; i < (uint8)CONFIG_MSG_LEN_ALL - 27; i++)
	{
		*(ConfigMsg.sw_ver + i) = 0;//从版本号之后的开始赋值 0
	}
	ConfigMsg.state = FW_APP_STATE;//boot有待考量
	memcpy(ConfigMsg.net.ip, DEFAULT_LIP, 4);
	memcpy(ConfigMsg.net.sub, DEFAULT_SN, 4);
	memcpy(ConfigMsg.net.gw, DEFAULT_GW, 4);
	memcpy(ConfigMsg.net.dns, DEFAULT_DNS, 4);

	ConfigMsg.sw_ver[1] = FW_VER_LOW;
	ConfigMsg.sw_ver[0] = FW_VER_HIGH;
	//调试中使用
	ConfigMsg.debug_flag = 1;
	ConfigMsg.echo_flag = 1;

}

int8 atonum(int8 ch)
{
	ch -= '0';
	if (ch > 9) ch -= 7;
	if (ch > 15) ch -= 0x20;
	return(ch);
}
uint8 get_hex(uint8 b0, uint8 b1)
{
	uint8 buf[2];
	buf[0] = b0;
	buf[1] = b1;
	buf[0] = atonum(buf[0]);
	buf[0] <<= 4;
	buf[0] += atonum(buf[1]);
	return(buf[0]);
}

void write_config_to_eeprom(void)
{
	uint8 i, data;
	//calculate CRC, the last 8 bytes not included
	for (i = 0; i < (uint8)CONFIG_MSG_LEN; i++)
	{
		data = *(uint8*)(ConfigMsg.mac + i);
		at24c16_write(i, data);
		IWDG_ReloadCounter();//feed iwdg
	}
}

void set_device_state(uint8 state)
{
	at24c16_write(10, state);
}


void get_config(void)
{
	uint16 i;
	at24c16_read(0);
	for (i = 0; i < (uint8)CONFIG_MSG_LEN; i++)
	{
		*(ConfigMsg.mac + i) = at24c16_read(i);
	}
}

void set_network(void)
{
	uint8 ip[4];
	setSHAR(ConfigMsg.mac);
	setSUBR(ConfigMsg.net.sub);
	setGAR(ConfigMsg.net.gw);
	setSIPR(ConfigMsg.net.ip);
	//Init. TX & RX Memory size of w5500
	sysinit(txsize, rxsize);
	setRTR(2000);//200ms
	setRCR(3);
	getSIPR(ip);
}
