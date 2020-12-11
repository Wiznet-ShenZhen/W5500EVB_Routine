#include "stm32f10x.h"
#include "usart.h"
#include "flash.h"
#include "config.h"
#include "device.h"
#include "socket.h"
#include "w5500.h"
#include "spi2.h"
#include "ult.h"
#include "tftp.h"
#include <stdio.h>
#include <string.h>

TFTP_MSG  tftp;
uint8_t g_socket_rcv_buf[MAX_MTU_SIZE];
uint8 Buffer[1024];
uint32_t tftp_server;
uint8 filename_flag=0;

int main()
{
	uint8 len,ret;	
	u32 addr; 
  u32 app_first_4_bytes;
	Systick_Init(72);
	NVIC_Configuration(); //NVIC Configuration	
	USART1_Init(); //115200@8-n-1 
	GPIO_Configuration(); //GPIO configuration
	TIM3_Int_Init(1000,71);//10Khz的计数频率，计数到1000为1ms 
	printf("\r\n MCU initialized.\r\n");
	Reset_W5500();
	WIZ_SPI_Init();
	set_w5500_mac();
	set_default();
	set_network(); 
	printf("Network is ready!\r\n");
  addr = ApplicationAddress;
  app_first_4_bytes = *(vu32*)addr;
  if ((app_first_4_bytes & 0xffffffff) == 0xffffffff)//there is no app code in flash
  { 
		write_config_to_eeprom(); 
  }	
	get_config();//read config data from flash 
	TFTP_init(SOCK_TFTP,g_socket_rcv_buf);	
	while(1)
  {
		if((ConfigMsg.newfirm[0]!=ConfigMsg.sw_ver[0])||(ConfigMsg.newfirm[1]!=ConfigMsg.sw_ver[1]))
		{			
					if(USART_RX_STA&0x8000)  
					{	
						len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
						printf("\r\nsend new version filename: %s\r\n",USART_RX_BUF);
						memcpy(ConfigMsg.filename,USART_RX_BUF,len);		
						USART_RX_STA=0;
						memset(USART_RX_BUF,0,len+1);
						filename_flag=1;
					}		
					if(filename_flag==1)
					{
						filename_flag=0;			
						printf(" TFTP START\r\n");
						tftp_server = (remote_ip[0] << 24) | (remote_ip[1] << 16) | (remote_ip[2] << 8) | (remote_ip[3]);
						TFTP_read_request(tftp_server,ConfigMsg.filename);
						while(1)
						{	
							ret = TFTP_run();							
							//if(ret == TFTP_SUCCESS)
							if(ret != TFTP_PROGRESS)
							break;
						}
						if(ret == TFTP_SUCCESS)
						{
							printf("TFTP SUCCEED\r\n");	
							TIM_Cmd(TIM3, DISABLE);							
							Delay_ms(10);
							reboot_app();
						}			
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

