/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.0
* @date    			2015-12-12
* @brief   			用3.5.0版本库建的工程模板
* @attention  	
*               内网测试，请保证W5500的IP与测试PC机在同一网段内，且不冲突
******************************************************************************
*/ 
#include "include.h"
int main(void)
{	 
	/***** MCU时钟初始化 *****/
	systick_init(72);

	/***** 串口初始化 *****/
	USART1_Config();

	/*初始化eeprom*/
	i2c_CfgGpio();				    	       
	
  printf(" W5500EVB Init Complete!\r\n");
	
	/***** GPIO初始化 *****/
	gpio_for_w5500_config();

	/***** 硬重启W5500 *****/
	reset_w5500();					            
	
	 /*配置MAC地址*/
	set_w5500_mac();

	/***** W5500的IP信息初始化 *****/
	set_w5500_ip();
	
	/***** 初始化8个Socket的发送接收缓存大小 *****/
  socket_buf_init(txsize, rxsize);    
	
	printf("<Tip:Input W5500EVB's IP in browser's URL or input 'W5500EVB/' to get in >\r\n"); 
	while(1)														
	{
			/***** 打开HTTP Server *****/
			do_https();

			/***** NetBios测试程序 *****/
			do_netbios();                   
	} 

}

