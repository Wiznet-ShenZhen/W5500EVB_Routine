#include "include.h"

extern uint8 txsize[];										// 引用外部变量，声明Socket发送缓存大小
extern uint8 rxsize[];										// 引用外部变量，声明Socket接收缓存大小


int main(void)
{
  uint8 server_ip[4]={192,168,1,102};			// 配置远程服务器IP地址
	uint16 server_port=5000;								// 配置远程服务器端口
	
  /***** MCU时钟初始化 *****/							  
	Systick_Init(72);	
	
	/***** 中断控制器配置 *****/
	NVIC_Configuration();
	
	/***** GPIO、SPI初始化 *****/
	GPIO_Configuration();			
	WIZ_SPI_Init();
	
	/***** 串口初始化 *****/
	USART1_Init(); 		
	
	
	/***** 硬重启W5500 *****/
	Reset_W5500();
	
	/***** W5500的IP信息初始化 *****/
	set_default(); 														// 设置默认MAC、IP、GW、SUB、DNS
	set_network();														// 配置初始化IP信息并打印，初始化8个Socket
	
	/***** 打开W5500的Keepalive功能 *****/
	setkeepalive(0);
	
	printf("TCP Server IP: %d.%d.%d.%d \r\n",server_ip[0],server_ip[1],server_ip[2],server_ip[3]);
  printf("TCP Server Port: %d \r\n",server_port);	
	printf("W5500 Init Complete!\r\n");
  printf("Start TCP Client Test!\r\n"); 

	while(1)																				
	{	
		// Socket状态机
		tcpc(server_ip);
	}
}


