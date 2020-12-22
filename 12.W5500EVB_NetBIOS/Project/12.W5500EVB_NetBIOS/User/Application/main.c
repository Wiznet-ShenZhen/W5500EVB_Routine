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
	/***** MCUʱ�ӳ�ʼ�� *****/		
	systick_init(72);

	/***** ���ڳ�ʼ�� *****/
	USART1_Config();

	/***** ��ʼ��eeprom *****/
	i2c_CfgGpio();				    	        
	
  printf(" NetBIOS Name Server Demo V1.0 \r\n");
	/***** ��ʼ��MCU������� *****/
	gpio_for_w5500_config();

	/***** ��λW5500 *****/
	reset_w5500();	
	
	/***** ����MAC��ַ *****/
	set_w5500_mac(); 

	/***** ��ʼ��8��Socket�ķ��ͽ��ջ����С *****/
  socket_buf_init(txsize, rxsize);    
	
	while(1)														
	{
		 /***** ��DHCP��������ȡIP��ַ *****/
		do_dhcp();                       
		if(dhcp_ok==1)										/*��ȡIP��ַ֮��*/
		{
			/***** ��HTTP Server *****/
			do_https();

			/***** NetBios���Գ��� *****/
			do_netbios();                   
		} 
	}
} 



