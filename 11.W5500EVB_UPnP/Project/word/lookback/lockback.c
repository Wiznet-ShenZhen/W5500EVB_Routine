#include "stm32f10x.h"
#include "config.h"
#include "w5500.h"
#include "socket.h"
#include "ult.h"
#include "lockback.h"
#include <stdio.h>
#include <string.h>
#include "Types.h"

#define tick_second 1
uint8 Buff1[6] ="LED_ON";
uint8 Buff2[7] ="LED_OFF";
uint8 OK_Buff[2] ="OK";
extern uint8 ch_status[MAX_SOCK_NUM];
extern CHCONFIG_TYPE_DEF Chconfig_Type_Def;
extern uint32_t presentTime;
uint16 any_port = 1000;
//u8 data_buf[TX_RX_MAX_BUF_SIZE];
extern uint8  I_STATUS[MAX_SOCK_NUM];
extern uint16 SSIZE[MAX_SOCK_NUM]; /**< Max Tx buffer size by each channel */
extern uint16 RSIZE[MAX_SOCK_NUM]; /**< Max Rx buffer size by each channel */
extern uint8* GET_SIP; 
extern void   SOCK_DISCON(SOCKET s);
void loopback_tcps(SOCKET s, uint16 port)
{	
	uint16 len=0;
	uint8 * data_buf = (uint8*) TX_BUF;
	switch(getSn_SR(s))/*获取socket SOCK_TCPS 的状态*/
	{
		case SOCK_CLOSED:/*socket关闭*/
			socket(s,Sn_MR_TCP,port,0x00);/*打开socket端口*/
		break;
		case SOCK_INIT:/*socket初始化完成*/
			listen(s);/*监听这个socket*/
	
		break;
		case SOCK_ESTABLISHED:/*socket连接建立*/
		
			if(getSn_IR(s) & Sn_IR_CON)
			{
				setSn_IR(s, Sn_IR_CON);/*清除接收中断标志*/
			}
			len=getSn_RX_RSR(s);/*len为已接收数据的大小*/
			if(len>0)
			{
				recv(s, data_buf, len);/*W5500接收来自TCP Client的数据*/
//				data_buf[len]=0x00; //添加字符串结束符
				if (memcmp(Buff1, data_buf,6) == 0)
				{
				send(s,Buff1,6);
				LED0=0;
				LED1=0; 
				printf("%s\r\n",OK_Buff); 
				}
				if(memcmp(Buff2, data_buf,7) == 0)	
				{
				send(s,Buff2,7);
				LED0=1;
				LED1=1; 
				printf("%s\r\n",OK_Buff);
				}
				send(s,data_buf,len);
				printf("%s\r\n",data_buf);
		}
		break;
		case SOCK_CLOSE_WAIT:/*socket等待关闭状态*/
			 close(s);
		break;
	}
}


void loopback_udp(SOCKET s, uint16 port)
{
	uint16 len;
	uint8 * data_buf = (u8*) TX_BUF;
	u8 destip[4];
	uint16 destport;
 //   uint8 addr[4] = {192, 168, 1, 110};
 //   uint8	tmp;
  
switch(getSn_SR(s))/*获取socket 0的状态*/
      {
         case SOCK_UDP:/*socket 的套接字初始化完成*/
           if(getSn_IR(s) & Sn_IR_RECV)
          {
            setSn_IR(s, Sn_IR_RECV);/*Sn_IR的第0位置1*/
          }
           if((len=getSn_RX_RSR(s))>0)
           {  
		      memset(data_buf,0,len+1);
              recvfrom(s,data_buf, len,(uint8*)&destip,&destport);/*W5200接收计算机发送来的数据*/
			  printf("%s\r\n",data_buf);
			//  printf("\r\n%d.%d.%d.%d:%d", destip[0],destip[1],destip[2],destip[3],destport);
              sendto(s,data_buf,len,(uint8*)&destip,destport);/*W5200把接收到的数据发送给计算机*/
			  
            }
           break;
						case SOCK_CLOSED:/*socket 关闭*/
           socket(s,Sn_MR_UDP,port,0);/*初始化socket 0的套接字*/
           break;
       }

}

