#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "wol.h"
#include "config.h"
#include <string.h>

extern uint8 txsize[];										// 引用外部变量，声明Socket发送缓存大小
extern uint8 rxsize[];										// 引用外部变量，声明Socket接收缓存大小
extern CONFIG_MSG  ConfigMsg;

uint8 buffer[2048]={0};												// 定义一个2KB的数组，用来存放Socket的通信数据

uint8 DIP[4]={0};
uint8 DHAR[6]={0};
uint16 DPORT=3000;

void do_wol()
{
   uint16 len=0;
   switch(getSn_SR(0))                          // 获取socket0的状态
   {
      case SOCK_UDP:                           	// Socket处于初始化完成(打开)状态
        if(getSn_IR(0) & Sn_IR_RECV)
        {
         setSn_IR(0, Sn_IR_RECV);
        }
				if(getIR() & 0x10)
        {
          printf("get wake on line\r\n");
					setIR(0x10);
					setMR(0x00);
        }
			 	// 数据回环测试程序：数据从远程上位机发给W5500，W5500接收到数据后再回给远程上位机
        if((len=getSn_RX_RSR(0))>0)
        { 
           recvfrom(0, buffer, len, DIP,&DPORT);  	// W5500接收来自远程上位机的数据，并通过SPI发送给MCU
					 printf("%d.%d.%d.%d:%d:\r\n", DIP[0],DIP[1],DIP[2],DIP[3],DPORT);
           printf("%s\r\n", buffer);                // 串口打印接收到的数据
           sendto(0, buffer,len, DIP, DPORT);       // 接收到数据后再回给远程上位机，完成数据回环
					 memset(buffer,0,len+1);                  // 清空接收到的数据
         }
        break; 
      case SOCK_CLOSED:                          // Socket处于关闭状态
         socket(0,Sn_MR_UDP,5000,0);         
			   setIMR(0x10);
			   setMR(MR_WOL);
        break;
    }
}
