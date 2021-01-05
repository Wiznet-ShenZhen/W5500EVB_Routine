#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "multcast.h"
#include <string.h>

uint8 buffer[2048];												// 定义一个2KB的数组，用来存放Socket的通信数据

uint8 remote_ip[4];			// 配置远程IP地址
uint16 remote_port;								// 配置远程端口
uint16 local_port=4000;									// 初始化一个本地端口
uint16 len=0;

uint8 DIP[4]={224,1,1,1};
uint8 DHAR[6]={0x01,0x00,0x5e,0x01,0x01,0x01};
uint16 DPORT=4000;

void do_multcast()
{
	switch(getSn_SR(0))																						// 获取socket0的状态
	{
		case SOCK_UDP:																							// Socket处于初始化完成(打开)状态
				Delay_ms(100);
				if(getSn_IR(0) & Sn_IR_RECV)
				{
					setSn_IR(0, Sn_IR_RECV);															// Sn_IR的RECV位置1
				}
				// 数据回环测试程序：数据从远程上位机发给W5500，W5500接收到数据后再回给远程上位机
				if((len=getSn_RX_RSR(0))>0)
				{ 
					recvfrom(0,buffer, len, remote_ip,&remote_port);			// W5500接收来自远程上位机的数据，并通过SPI发送给MCU
					printf("%d.%d.%d.%d:%d\r\n", remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3],remote_port);
					printf("%s", buffer);														// 串口打印接收到的数据
					sendto(0,buffer,len, DIP, DPORT);		  		      // 接收到数据后再回给远程上位机，完成数据回环
					memset(buffer,0,len+1);                         // 清空接收数组
				}
		break;
		case SOCK_CLOSED:																						// Socket处于关闭状态
				 setDIPR(0,DIP);
				 setDHAR(0,DHAR);
				 setDPORT(0,DPORT);
				 socket(0,0x82,4000,0);                      /*初始化socket 0的套接字*/									
		break;
	}
}
