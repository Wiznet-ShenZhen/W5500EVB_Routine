/*
	boot �޸���ʷ��
	�汾�ţ� �޸����ڣ�    �޸����ݣ�
	V1.0     2016.06.20    ȷ����һ�������汾
	V1.1     2016.07.25    ��λ�󣬶��������ĵ�һ���ֽڡ���>�����Ƶ����λʱ��EEPROM����������

*/
/* Includes ------------------------------------------------------------------*/
#include "mcu_init.h"
#include "spi.h"
#include "24c16.h"
#include "usart.h"
#include "config.h"
#include "device.h"
#include "util.h"
//#include "crc.h"
#include "iap.h"
#include <stdio.h>
#include <string.h>
#define MSG_ADDR		0x08000000+0x800
#define XOR_VAL			0x12345678
#define XOR_VAL_3   0x20160531 
static uint8 is_app_inside(void)
{
	u32 addr, app_first_4_bytes;
	addr = ApplicationAddress;
	app_first_4_bytes = *(vu32*)addr;
	if ((app_first_4_bytes & 0xffffffff) == 0xffffffff)
		return 0;
	else
		return 1;
}

static uint8 is_first_run(void)
{
	uint32 msg = *(vu32*)(MSG_ADDR);
	if (msg == 0x4E5A4957)//the first time
	{
		return (1);
	}
	else
	{
		return (0);
	}

}

static void len_zero(void)
{
	uint16 i = 0;

	for (i = 210; i <= 213; i++)
		at24c16_write(i, 0x00);
}

// ע��boot�в��ܿ��� �����ж�
int main(void)
{
	uint16 i;
	uint32 len;
	Systick_Init(72);/* ��ʼ��Systick����ʱ��*/
//	Delay_ms(200);
	GPIO_Configuration();
	NVIC_Configuration(); // NVIC Configuration
	USART1_115200();
	WIZ_SPI_Init();
	at24c16_init();
	reset_w5500();
	get_config();//��FLASH ConfigAddrλ�ö�ȡ�豸������Ϣ

	len = at24c16_read(210);
	len = (len << 8) + at24c16_read(211);
	len = (len << 8) + at24c16_read(212);
	len = (len << 8) + at24c16_read(213);

	if (len != 0)
	{
		ConfigMsg.state = FW_RXD_SERVER;
	}
	else
	{
		ConfigMsg.state = FW_APP_STATE;
	}
	if (is_first_run() == 1)
	{
		printf("First download\r\n");
		memcpy(ConfigMsg.mac, DEFAULT_MAC, 6);// ����Ĭ�ϵ�MAC��ַ
		sprintf((char*)ConfigMsg.device_id, "20181208-%02x%02x%02x", ConfigMsg.mac[3], ConfigMsg.mac[4], ConfigMsg.mac[5]);
		set_default();            //����Ĭ��ֵ
		write_config_to_eeprom(); //д��Ĭ��ֵ
		//write cpuID
	}
	switch (ConfigMsg.state)
	{
	case FW_APP_STATE:
#if 1
		if (is_app_inside() == 1)
		{
			printf("App Normal\r\n");
			for (i = 0; i < 10000; i++) {};
			reboot_app();        //jump to app directly
		}
		else
		{
			printf("No App found\r\n");
			//			printf("Please download the APP.\r\n");
		}
#endif

		break;
	case FW_RXD_HTTP:
	case FW_RXD_SERVER:
		ConfigMsg.fw_len = len;
		//	  printf("len:%ld\r\n",ConfigMsg.fw_len);
#if 1
		if (copy_app(ConfigMsg.fw_len, ConfigMsg.fw_checksum) == TRUE)
		{
			//set to normal mode

			printf("Copy App Normal\r\n");

			ConfigMsg.fw_checksum = 0;
			ConfigMsg.fw_len = 0;
			ConfigMsg.state = FW_APP_STATE;//�̼�����
			len = 0;
			write_config_to_eeprom();     //д�����
			at24c16_write(210, 0);
			at24c16_write(211, 0);
			at24c16_write(212, 0);
			at24c16_write(213, 0);
			//			printf("update ok **************\r\n");
			reboot_app();
		}
		else
		{
			printf("Copy fw error\r\n");
		}
#endif
		//reboot_back();
		//reboot();
		break;
	case EEPROM_CRC_ERROR:
		printf("CRC error!\r\n");
		break;
	default:
		printf("Unknown State:%02x\r\n", ConfigMsg.state);
		printf("Please download the APP.\r\n");
		break;
	}

	set_network();
	/* Infinite loop */
	while (1)
	{
		IWDG_ReloadCounter();       //feed iwdg
		if (reboot_flag == 1)reboot(); // reboot
	}
}
