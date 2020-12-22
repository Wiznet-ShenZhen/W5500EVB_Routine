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
	
	/***** GPIO��ʼ�� *****/
	gpio_for_w5500_config();
	
	/***** ���ڳ�ʼ�� *****/
	USART1_Config();

	/***** Ӳ��λW5500 *****/
	reset_w5500();
	
	/***** ��ȡ������Ϣ *****/
	get_config();

	/***** W5500��IP��Ϣ��ʼ�� *****/
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



