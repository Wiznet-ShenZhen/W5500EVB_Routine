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
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio.h"
#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"
#include "nvic.h"
#include "udp_config.h"
#include "fw_updata.h"

uint16 i;
uint32_t addr, app_first_4_bytes;
int main(void)
{ 	
    
	systick_init(72);				            /*初始化Systick工作时钟*/
	NVIC_Configuration();
	USART1_Config(); 				            /*初始化串口通信:115200@8-n-1*/
	gpio_for_w5500_config();	         	/*初始化MCU相关引脚*/
	reset_w5500();					            /*硬复位W5500*/
	get_config();

		 	
	addr = ApplicationAddress;
	app_first_4_bytes = *(uint32_t*)addr;
	
	 if ((app_first_4_bytes & 0xffffffff) == 0xffffffff)
  {
		printf("\r\nW5500 Demo Code for Remote IAP.\r\n");
    printf("Please upload APP by using ConfigTool\r\n");
    set_default();
    set_network();
	
  }else{	    
	switch (ConfigMsg.state)
    {
    case NORMAL_STATE:
      printf(" NORMAL_STATE.\r\n");
      for(i=0;i<1000;i++);
      reboot_app();
      break;
    case  NEW_APP_IN_BACK:
			printf("NEW_APP_IN_BACK.\r\n");
      printf("Copy App: %d\r\n", ConfigMsg.fw_len);
      copy_app(ConfigMsg.fw_len);
		  ConfigMsg.fw_len = 0; 
      ConfigMsg.state=NORMAL_STATE;
      write_config_to_flash();
      reboot_app();
      break;
    default:
      printf("unknown state=%d\r\n",ConfigMsg.state);
      break;
    }    
	}
   
	while(1)
	 {
    do_udp_config();
    do_fw_update();
  }
} 



