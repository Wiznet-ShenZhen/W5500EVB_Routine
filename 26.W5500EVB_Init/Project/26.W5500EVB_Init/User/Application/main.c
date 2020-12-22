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
uint8 reboot_flag = 0;
int main(void)
{	
	/***** MCU时钟初始化 *****/
	systick_init(72);				            
				            
	/***** GPIO初始化 *****/
	gpio_for_w5500_config();	         
	i2c_CfgGpio();				    	        
	
	/***** 串口初始化 *****/
	USART1_Config(); 
  printf(" W5500EVB Init Complete!\r\n");
	
	/***** 硬重启W5500 *****/
	reset_w5500();

	/***** W5500的IP信息初始化 *****/
	set_w5500_mac();                    
	set_w5500_ip();
	
	/***** 初始化8个Socket的发送接收缓存大小 *****/
  socket_buf_init(txsize, rxsize);    
	
	printf("<Tip:Input W5500EVB's IP in browser's URL or input 'W5500EVB/' to get in >\r\n"); 
	while(1)														
	{
			do_https();                     /*打开HTTP Server*/
			do_netbios();                   /*NetBios测试程序*/
	}
} 



