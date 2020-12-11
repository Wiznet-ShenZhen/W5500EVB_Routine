/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.0
* @date    			2015-12-12
* @brief   			��3.5.0�汾�⽨�Ĺ���ģ��
* @attention  	
*               �������ԣ��뱣֤W5500��IP�����PC����ͬһ�����ڣ��Ҳ���ͻ
******************************************************************************
*/ 
#include "include.h"
uint8 reboot_flag = 0;
int main(void)
{	  
	systick_init(72);				            /*��ʼ��Systick����ʱ��*/
	USART1_Config(); 				            /*��ʼ������ͨ��:115200@8-n-1*/
	i2c_CfgGpio();				    	        /*��ʼ��eeprom*/
	
  printf(" W5500EVB Init Complete!\r\n");
	
	gpio_for_w5500_config();	         	/*��ʼ��MCU�������*/
	reset_w5500();					            /*Ӳ��λW5500*/
	set_w5500_mac();                    /*����MAC��ַ*/
	set_w5500_ip();
  socket_buf_init(txsize, rxsize);    /*��ʼ��8��Socket�ķ��ͽ��ջ����С*/
	
	printf("<Tip:Input W5500EVB's IP in browser's URL or input 'W5500EVB/' to get in >\r\n"); 
	while(1)														/*ѭ��ִ�еĺ���*/ 
	{
			do_https();                     /*��HTTP Server*/
	}
} 


