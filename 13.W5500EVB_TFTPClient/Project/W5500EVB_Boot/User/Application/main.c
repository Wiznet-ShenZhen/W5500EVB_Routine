#include "include.h"

uint8_t g_socket_rcv_buf[MAX_MTU_SIZE];

int main()
{
  /***** MCUʱ�ӳ�ʼ�� *****/		
	Systick_Init(72);
	
	/***** ����Ƕ���ж����� *****/	
	NVIC_Configuration(); 
	
	/***** GPIO��ʼ�� *****/
	GPIO_Configuration(); 
	
	//10Khz�ļ���Ƶ�ʣ�������1000Ϊ1ms 
	TIM3_Int_Init(1000,71);
	
  /***** SPI��ʼ�� *****/
	WIZ_SPI_Init();
	
	/***** ���ڳ�ʼ�� *****/
	USART1_Init(); 
	
	/***** Ӳ����W5500 *****/
	Reset_W5500();

	/***** W5500��IP��Ϣ��ʼ�� *****/		
	set_default();
	set_network(); 
	
	printf("\r\n W5500EVB initialized.\r\n");
	printf("Network is ready!\r\n");	
	
	check_flash_app();	
	get_config();
	TFTP_init(SOCK_TFTP,g_socket_rcv_buf);	
	
	while(1)
  {
		//check firmware version
		if((ConfigMsg.newfirm[0]!=ConfigMsg.sw_ver[0])||(ConfigMsg.newfirm[1]!=ConfigMsg.sw_ver[1]))
		{			
			if(get_usart_buf() == 1)
			{	
				tftp_start();		
			}
		}
		else
		{
			printf(" newersion do not find, start oldversion\r\n");
			TIM_Cmd(TIM3, DISABLE);	
			reboot_app();
		}
	}										
}

