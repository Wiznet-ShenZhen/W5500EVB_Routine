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
	/***** MCUʱ�ӳ�ʼ�� *****/
	systick_init(72);				            
				            
	/***** GPIO��ʼ�� *****/
	gpio_for_w5500_config();	         
	i2c_CfgGpio();				    	        
	
	/***** ���ڳ�ʼ�� *****/
	USART1_Config(); 
  printf(" W5500EVB Init Complete!\r\n");
	
	/***** Ӳ����W5500 *****/
	reset_w5500();

	/***** W5500��IP��Ϣ��ʼ�� *****/
	set_w5500_mac();                    
	set_w5500_ip();
	
	/***** ��ʼ��8��Socket�ķ��ͽ��ջ����С *****/
  socket_buf_init(txsize, rxsize);    
	
	printf("<Tip:Input W5500EVB's IP in browser's URL or input 'W5500EVB/' to get in >\r\n"); 
	while(1)														
	{
			do_https();                     /*��HTTP Server*/
			do_netbios();                   /*NetBios���Գ���*/
	}
} 



