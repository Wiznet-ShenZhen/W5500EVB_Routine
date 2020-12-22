/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.0
* @date    			2015-08-04

******************************************************************************
*/ 
#include "include.h"
uint8 reboot_flag = 0;

int main(void)
{	
	/***** MCU时钟初始化 *****/		
	systick_init(72);

	/***** 串口初始化 *****/
	USART1_Config();

	/***** 初始化eeprom *****/
	i2c_CfgGpio();				    	        
	
  printf(" NetBIOS Name Server Demo V1.0 \r\n");
	/***** 初始化MCU相关引脚 *****/
	gpio_for_w5500_config();

	/***** 复位W5500 *****/
	reset_w5500();	
	
	/***** 配置MAC地址 *****/
	set_w5500_mac(); 

	/***** 初始化8个Socket的发送接收缓存大小 *****/
  socket_buf_init(txsize, rxsize);    
	
	while(1)														
	{
		 /***** 从DHCP服务器获取IP地址 *****/
		do_dhcp();                       
		if(dhcp_ok==1)										/*获取IP地址之后*/
		{
			/***** 打开HTTP Server *****/
			do_https();

			/***** NetBios测试程序 *****/
			do_netbios();                   
		} 
	}
} 



