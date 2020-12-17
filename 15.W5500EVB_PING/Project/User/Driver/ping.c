/**
******************************************************************************
* @file   		ping.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-12-12
* @brief   		ping 演示函数 
******************************************************************************
**/
#include "ping.h"

#define PING_DEBUG
PINGMSGR PingRequest;	 
PINGMSGR PingReply;	   

uint8  remote_ip[4]={192,168,1,101};											/*远端IP地址*/
static uint16 RandomID = 0x1234; 
static uint16 RandomSeqNum = 0x4321;
uint8 ping_reply_received = 0; 
uint8 req=0;
uint8 rep=0;

/**
*@brief		设定次数ping外网IP函数
*@param		s-   socket number
*@param		addr-  外网IP地址
*@param		pCount- ping的次数
*@return	无
*/
void ping_count(uint8 s, uint16 pCount, uint8 *addr)
{
	uint16 rlen, cnt,i;
	cnt = 0;
	
	for(i=0; i<pCount+1;i++)																	/*循环ping pCount次*/
	{		
		if(i!=0)
		{
			/* Output count number */
			printf( "No.%d  ",i);
		}
		switch(getSn_SR(s))																			/*获取socket状态*/
		{
			case SOCK_CLOSED:																			/*socket关闭状态*/
				close(s);			
				/* Create Socket */  
				IINCHIP_WRITE(Sn_PROTO(s), IPPROTO_ICMP);						/*设置ICMP 协议*/
				if(socket(s,Sn_MR_IPRAW,3000,0)!=0)									/*判断ip raw模式socket是否开启*/
				{ }
				/* Check socket register */
				while(getSn_SR(s)!=SOCK_IPRAW);		
			break;
			case SOCK_IPRAW:																			/*ip raw模式*/		
				ping_request(s, addr);															/*发送Ping请求*/
				req++;			
			while(1)
			{
				if ( (rlen = getSn_RX_RSR(s) ) > 0)
				{
					ping_reply(s, addr, rlen);											/*获取回复信息*/
					rep++;
					if (ping_reply_received)  break;				
				}	
				if ( (cnt > 1000) ) 
				{
					printf("Request Time out\r\n\r\n") ;
					cnt = 0;
					break;
				}
				else
				{ 
					cnt++; 	
					delay_ms(1000);
				}
			}
			break;	
			default:					
			break;
		}
		if(req>=pCount)
		{	
			printf("Ping Request = %d, Ping Reply = %d, Lost = %d\r\n",req,rep,req-rep);						  
		}
	}
}

/**
*@brief		ping请求函数
*@param		s-  socket number
*@param		addr-  P地址
*@return	无
*/
uint8 ping_request(uint8 s, uint8 *addr)
{
	uint16 i;
	ping_reply_received = 0;	        												/*ping 回复初始化标志位*/
	PingRequest.Type = PING_REQUEST;                  				/*Ping-Request*/
	PingRequest.Code = CODE_ZERO;	                   					/*总是 '0'*/
	PingRequest.ID = htons(RandomID++);	       								/*设置ping响应ID为随机的整型变量*/
	PingRequest.SeqNum =htons(RandomSeqNum++);								/*设置ping响应的序列号为随机整形变量*/
	for(i = 0 ; i < BUF_LEN; i++)
	{	                                
		PingRequest.Data[i] = (i) % 8;		 				 							/*ping相应的数在'0'~'8‘*/
	}
	PingRequest.CheckSum = 0;
	/* 计算响应次数*/
	PingRequest.CheckSum = htons(checksum((uint8*)&PingRequest,sizeof(PingRequest))); 
	/*发送ping响应到目的方 */
	if(sendto(s,(uint8 *)&PingRequest,sizeof(PingRequest),addr,3000)==0)	
	{  
		printf("Fail to send ping-reply packet\r\n") ;					
	}
	else
	{
		printf("Ping:%d.%d.%d.%d\r\n",(addr[0]),  (addr[1]),  (addr[2]),  (addr[3]));
	}
	return 0;
} 

/**
*@brief		解析Ping回复
*@param		s-  socket number
*@param		addr- Ping地址
*@return	无
*/
uint8 ping_reply(uint8 s, uint8 *addr,  uint16 rlen)
{
	uint16 tmp_checksum;
	uint16 len;
	uint16 i;
	uint8 data_buf[136];
	
	uint16 port = 3000;
	PINGMSGR PingReply;
	len = recvfrom(s, data_buf,rlen,addr,&port);			/*从目的端接收数据*/
	
	if(data_buf[0] == PING_REPLY) 		
	{	
		PingReply.Type 		 = data_buf[0];
		PingReply.Code 		 = data_buf[1];
		PingReply.CheckSum   = (data_buf[3]<<8) + data_buf[2];
		PingReply.ID 		 = (data_buf[5]<<8) + data_buf[4];
		PingReply.SeqNum 	 = (data_buf[7]<<8) + data_buf[6];
			
		for(i=0; i<len-8 ; i++)
		{
			PingReply.Data[i] = data_buf[8+i];
		}
		tmp_checksum = ~checksum(data_buf,len);									/*检查ping回复的次数*/
		if(tmp_checksum != 0xffff)
			printf("tmp_checksum = %x\r\n",tmp_checksum);
		
		else
		{
			printf("Reply from %3d.%3d.%3d.%3d ID=%x Byte=%d\r\n\r\n",(addr[0]),  (addr[1]),  (addr[2]),  (addr[3]),htons(PingReply.ID),(rlen+6));			
			ping_reply_received =1;																/*当退出ping回复循环时，设置ping回复标志为1*/
		}
	}
	else if(data_buf[0] == PING_REQUEST)
	{
		PingReply.Code 	 = data_buf[1];
		PingReply.Type 	 = data_buf[2];
		PingReply.CheckSum  = (data_buf[3]<<8) + data_buf[2];
		PingReply.ID 		 = (data_buf[5]<<8) + data_buf[4];
		PingReply.SeqNum 	 = (data_buf[7]<<8) + data_buf[6];		
		for(i=0; i<len-8 ; i++)
		{
			PingReply.Data[i] = data_buf[8+i];
		}
		tmp_checksum = PingReply.CheckSum;											/*检查ping回复次数*/
		PingReply.CheckSum = 0;
		if(tmp_checksum != PingReply.CheckSum)
		{
			printf( " \n CheckSum is in correct %x shold be %x \n",   (tmp_checksum),  htons(PingReply.CheckSum)) ;
		}
		else
		{		}
		printf("	Request from %d.%d.%d.%d  ID:%x SeqNum:%x  :data size %d bytes\r\n",
		(addr[0]),  (addr[1]),  (addr[2]),  (addr[3]),  (PingReply.ID),  (PingReply.SeqNum),  (rlen+6) );	   
		ping_reply_received =1;																	/* 当退出ping回复循环时，设置ping回复标志为1	*/
	}
	else
	{      
		printf(" Unkonwn msg. \n");
	}
	return 0;
}

/**
*@brief		执行ping函数
*@param		无
*@return	无
*/
void do_ping(void)
{	 
	ping_count(SOCK_PING,4,remote_ip);
}




