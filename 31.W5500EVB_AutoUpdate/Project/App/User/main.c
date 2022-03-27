/*
	app 修改历史：
	版本号： 修改日期：    修改内容：
	V1.0     2018.11.27    确定第一个发布版本

*/

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "mcu_init.h"
#include "usart.h"
#include "device.h"
#include "spi.h"
#include "util.h"
#include "tsensor.h"
#include "httpult.h"
#include "base64.h"
#include "timer.h"
#include "24c16.h"
#include "socket.h"
#include "w5500.h"
#include <stdio.h>
#include <string.h>
#include "iap.h"
#include "version.h"

#define MSG_ADDR		0x08000000+0x800
#define XOR_VAL			0x12345678
#define XOR_VAL_3		0x20160531

uint8 flg = 0;
uint8 update_flag = 0;			//检查有无新版本固件的标志位，0：初始化值；1：有新版版，尚未下载；2：云平台该产品无对应固件/无新版本/有新版本，但已经下载到Backup区
uint8 download_flag = 0;		//固件是否下载标志位，0：初始化值；1：已下载固件

extern uint8 version[10];

static uint8 is_first_run(void)
{
	uint32 msg = *(vu32*)(MSG_ADDR);
	if (msg == 0x4E5A4957)
	{
		return (1);
	}
	else
	{
		return (0);
	}
}

void write_cpuID(void)
{
	static u32 cpuID[3];
	static u32 tmp[0x200];
	uint16 i = 0;
	vu32 dest_addr = MSG_ADDR;
	cpuID[0] = *(vu32*)(0x1ffff7e8);
	cpuID[1] = *(vu32*)(0x1ffff7ec);
	cpuID[2] = *(vu32*)(0x1ffff7f0);

	//read one page flash from MSG_ADDR ; one page size is 2K Bytes(0x800)
	for (i = 0; i < 512; i++)
	{
		tmp[i] = *(vu32*)(MSG_ADDR + i * 4);
	}
	tmp[0] = cpuID[0] ^ XOR_VAL;
	tmp[1] = cpuID[1] ^ XOR_VAL;
	tmp[2] = cpuID[2] ^ XOR_VAL;
	tmp[3] = ((cpuID[0] >> 1) + (cpuID[1] >> 2) + (cpuID[2] >> 3)) ^ XOR_VAL_3;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(MSG_ADDR);
	for (i = 0; i < 512; i++)
	{
		FLASH_ProgramWord(dest_addr, tmp[i]);
		dest_addr += 4;
		IWDG_ReloadCounter();
	}
	FLASH_Lock();
}

int main(void)
{
	Systick_Init(72);
	GPIO_Configuration();//时钟输出
	NVIC_Configuration();//中断控制器
	at24c16_init();//初始化eeprom
	WIZ_SPI_Init();//初始化SPI
	USART1_115200();
	reset_w5500();//
	set_default();//
	set_network();
	write_config_to_eeprom();

	if (is_first_run() == 1)
	{
		write_cpuID();
	}

	printf("The fw version is V%d.%d\r\n", FW_VER_HIGH, FW_VER_LOW);

	while (1)
	{
		if (update_flag == 0)
		{
			w5500_version();
		}
		if (update_flag == 1)
		{
			w5500_update();
		}
		if (download_flag == 1)
		{
			reboot();
		}
	}
}


