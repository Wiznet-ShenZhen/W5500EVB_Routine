#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "udp.h"
#include <string.h>

// 定义一个2KB的数组，用来存放Socket的通信数据
uint8 buffer[2048];												

uint8 remote_ip[4]={0,0,0,0};			  // 保存远程IP地址
uint16 remote_port=0;								// 保存远程端口
uint16 local_port=5000;							// 初始化一个本地端口
uint16 len=0;	

	/*Socket状态机，MCU通过读Sn_SR(0)的值进行判断Socket应该处于何种状态
		Sn_SR状态描述：
		0x00		SOCK_CLOSED
		0x13		SOCK_INIT
		0x14		SOCK_LISTEN
		0x17		SOCK_ESTABLISHED
		0x1C		SOCK_CLOSE_WAIT
		0x22		SOCK_UDP
	*/
void do_udp()
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
						memset(buffer,0,len+1);
						recvfrom(0,buffer, len, remote_ip,&remote_port);			// W5500接收来自远程上位机的数据，并通过SPI发送给MCU
						printf("%s\r\n",buffer);															// 串口打印接收到的数据
						sendto(0,buffer,len, remote_ip, remote_port);		  		// 接收到数据后再回给远程上位机，完成数据回环
						//memset(buffer,0,sizeof(buffer));
					}
			break;
			case SOCK_CLOSED:																						// Socket处于关闭状态
					socket(0,Sn_MR_UDP,local_port,0);												// 打开Socket0，并配置为UDP模式，打开一个本地端口
			break;
		}
}
