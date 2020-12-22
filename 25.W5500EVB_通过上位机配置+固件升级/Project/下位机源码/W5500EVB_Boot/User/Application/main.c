/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.1
* @date    			2017-08-01
* @brief   			��3.5.0�汾�⽨�Ĺ���ģ��
* @attention  								
*               �������ԣ��뱣֤W5500��IP�����PC����ͬһ�����ڣ��Ҳ���ͻ
*               ����������߸�PC��ֱ����������PC���������ӵ�ַIPΪ��̬IP
******************************************************************************
*/ 
#include "include.h"

uint16 i;
uint32_t addr, app_first_4_bytes;
int main(void)
{ 	
  /***** MCUʱ�ӳ�ʼ�� *****/  
	systick_init(72);	
	
	/***** ����Ƕ���ж����� *****/
	NVIC_Configuration();
	
	/***** ���ڳ�ʼ�� *****/
	USART1_Config();

	/***** GPIO��ʼ�� *****/
	gpio_for_w5500_config();

	/***** Ӳ����W5500 *****/
	reset_w5500();	

	/***** ��ȡ������Ϣ *****/
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



