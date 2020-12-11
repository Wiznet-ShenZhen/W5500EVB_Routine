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
/*app函数头文件*/
#include "netbios.h"
#include "http_server.h"
#include "httputil.h"
#include "dhcp.h"
uint8 reboot_flag = 0;

int main(void)
{	  
	systick_init(72);				            /*初始化Systick工作时钟*/
	USART1_Config(); 				            /*初始化串口通信:115200@8-n-1*/
	i2c_CfgGpio();				    	        /*初始化eeprom*/
	
  printf(" NetBIOS Name Server Demo V1.0 \r\n");
	
	gpio_for_w5500_config();	         	/*初始化MCU相关引脚*/
	reset_w5500();					            /*硬复位W5500*/
	set_w5500_mac();                    /*配置MAC地址*/
  socket_buf_init(txsize, rxsize);    /*初始化8个Socket的发送接收缓存大小*/
	
	while(1)														/*循环执行的函数*/ 
	{
		do_dhcp();                        /*从DHCP服务器获取IP地址*/
		if(dhcp_ok==1)										/*获取IP地址之后*/
		{
			do_https();                     /*打开HTTP Server*/
			do_netbios();                   /*NetBios测试程序*/
		} 
	}
} 



