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
	
	/***** 串口初始化 *****/
	USART1_Config();

	/***** GPIO初始化 *****/
	gpio_for_w5500_config();

	/***** 硬重启W5500 *****/
	reset_w5500();	

	/***** 获取配置信息 *****/
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



