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
int main(void)
{	 
	/***** MCUʱ�ӳ�ʼ�� *****/
	systick_init(72);

	/***** ���ڳ�ʼ�� *****/
	USART1_Config();

	/*��ʼ��eeprom*/
	i2c_CfgGpio();				    	       
	
  printf(" W5500EVB Init Complete!\r\n");
	
	/***** GPIO��ʼ�� *****/
	gpio_for_w5500_config();

	/***** Ӳ����W5500 *****/
	reset_w5500();					            
	
	 /*����MAC��ַ*/
	set_w5500_mac();

	/***** W5500��IP��Ϣ��ʼ�� *****/
	set_w5500_ip();
	
	/***** ��ʼ��8��Socket�ķ��ͽ��ջ����С *****/
  socket_buf_init(txsize, rxsize);    
	
	printf("<Tip:Input W5500EVB's IP in browser's URL or input 'W5500EVB/' to get in >\r\n"); 
	while(1)														
	{
			/***** ��HTTP Server *****/
			do_https();

			/***** NetBios���Գ��� *****/
			do_netbios();                   
	} 

}

