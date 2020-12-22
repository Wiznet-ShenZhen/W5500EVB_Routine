/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.1
* @date    			2017-08-01
* @brief   			用3.5.0版本库建的工程模板
* @attention  								
*               内网测试，请保证W5500的IP与测试PC机在同一网段内，且不冲突
*               如果是用网线跟PC机直连，请设置PC机本地连接地址IP为静态IP
******************************************************************************
*/ 
#include "include.h"



uint16 i;
uint32_t addr, app_first_4_bytes;
int main(void)
{ 	
  /***** MCU时钟初始化 *****/	  
	systick_init(72);

	/***** 配置嵌套中断向量 *****/
	NVIC_Configuration();
	
	/***** GPIO初始化 *****/
	gpio_for_w5500_config();
	
	/***** 串口初始化 *****/
	USART1_Config();

	/***** 硬复位W5500 *****/
	reset_w5500();
	
	/***** 获取配置信息 *****/
	get_config();

	/***** W5500的IP信息初始化 *****/
  set_network();
	printf("APP  RUNING!\r\n");	
	
	while(1)
	 {
		if(update_flag==0)
		  {
		     w5500_version();
		  }
		if(update_flag==1)
			{
				 w5500_update();			  
			 }
    do_udp_config();
    do_fw_update();
  }
} 



