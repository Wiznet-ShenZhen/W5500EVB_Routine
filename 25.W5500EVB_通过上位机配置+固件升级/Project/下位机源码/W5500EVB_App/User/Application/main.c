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
#include "stm32f10x_iwdg.h"
#include "auto_update.h"



uint16 i;
uint32_t addr, app_first_4_bytes;
int main(void)
{ 	
    
	systick_init(72);				            /*��ʼ��Systick����ʱ��*/
	NVIC_Configuration();
	USART1_Config(); 				            /*��ʼ������ͨ��:115200@8-n-1*/
	gpio_for_w5500_config();	         	/*��ʼ��MCU�������*/
	reset_w5500();					            /*Ӳ��λW5500*/
	get_config();
 // get_ver();
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



