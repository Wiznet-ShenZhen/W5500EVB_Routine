#include "include.h"

uint8_t g_socket_rcv_buf[MAX_MTU_SIZE];

int main()
{
  /***** MCU时钟初始化 *****/		
	Systick_Init(72);
	
	/***** 配置嵌套中断向量 *****/	
	NVIC_Configuration(); 
	
	/***** GPIO初始化 *****/
	GPIO_Configuration(); 
	
	//10Khz的计数频率，计数到1000为1ms 
	TIM3_Int_Init(1000,71);
	
  /***** SPI初始化 *****/
	WIZ_SPI_Init();
	
	/***** 串口初始化 *****/
	USART1_Init(); 
	
	/***** 硬重启W5500 *****/
	Reset_W5500();

	/***** W5500的IP信息初始化 *****/		
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

