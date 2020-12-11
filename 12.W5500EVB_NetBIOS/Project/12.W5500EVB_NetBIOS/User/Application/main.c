/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.0
* @date    			2015-08-04

******************************************************************************
*/ 
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio.h"
#include "bsp_led.h"
#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"
/*app����ͷ�ļ�*/
#include "netbios.h"
#include "http_server.h"
#include "httputil.h"
#include "dhcp.h"
uint8 reboot_flag = 0;

int main(void)
{	  
	systick_init(72);				            /*��ʼ��Systick����ʱ��*/
	USART1_Config(); 				            /*��ʼ������ͨ��:115200@8-n-1*/
	i2c_CfgGpio();				    	        /*��ʼ��eeprom*/
	
  printf(" NetBIOS Name Server Demo V1.0 \r\n");
	
	gpio_for_w5500_config();	         	/*��ʼ��MCU�������*/
	reset_w5500();					            /*Ӳ��λW5500*/
	set_w5500_mac();                    /*����MAC��ַ*/
  socket_buf_init(txsize, rxsize);    /*��ʼ��8��Socket�ķ��ͽ��ջ����С*/
	
	while(1)														/*ѭ��ִ�еĺ���*/ 
	{
		do_dhcp();                        /*��DHCP��������ȡIP��ַ*/
		if(dhcp_ok==1)										/*��ȡIP��ַ֮��*/
		{
			do_https();                     /*��HTTP Server*/
			do_netbios();                   /*NetBios���Գ���*/
		} 
	}
} 



