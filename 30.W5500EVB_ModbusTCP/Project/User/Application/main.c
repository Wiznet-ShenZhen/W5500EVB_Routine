#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "24c16.h"
#include "string.h"
#include "mbascii.h"

uint16 local_port=5000;										// 定义一个本地端口并初始化 	
extern uint8 txsize[];										// 引用外部变量，声明Socket发送缓存大小
extern uint8 rxsize[];										// 引用外部变量，声明Socket接收缓存大小

uint8 buffer[2048];												// 定义一个2KB的数组，用来存放Socket的通信数据

int main(void)
{  												
	/***** MCU时钟初始化 *****/							  
	Systick_Init(72);		
	
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
	
	printf("TCP Server Port: %d\r\n",local_port);
	printf("W5500 Init Complete!\r\n");
  printf("Start Modbus TCP Server Test!\r\n"); 

	while(1)																	
	{
		do_Modbus();
	}
}

