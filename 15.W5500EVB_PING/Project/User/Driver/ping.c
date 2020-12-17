/**
******************************************************************************
* @file   		ping.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-12-12
* @brief   		ping ��ʾ���� 
******************************************************************************
**/
#include "ping.h"

#define PING_DEBUG
PINGMSGR PingRequest;	 
PINGMSGR PingReply;	   

uint8  remote_ip[4]={192,168,1,101};											/*Զ��IP��ַ*/
static uint16 RandomID = 0x1234; 
static uint16 RandomSeqNum = 0x4321;
uint8 ping_reply_received = 0; 
uint8 req=0;
uint8 rep=0;

/**
*@brief		�趨����ping����IP����
*@param		s-   socket number
*@param		addr-  ����IP��ַ
*@param		pCount- ping�Ĵ���
*@return	��
*/
void ping_count(uint8 s, uint16 pCount, uint8 *addr)
{
	uint16 rlen, cnt,i;
	cnt = 0;
	
	for(i=0; i<pCount+1;i++)																	/*ѭ��ping pCount��*/
	{		
		if(i!=0)
		{
			/* Output count number */
			printf( "No.%d  ",i);
		}
		switch(getSn_SR(s))																			/*��ȡsocket״̬*/
		{
			case SOCK_CLOSED:																			/*socket�ر�״̬*/
				close(s);			
				/* Create Socket */  
				IINCHIP_WRITE(Sn_PROTO(s), IPPROTO_ICMP);						/*����ICMP Э��*/
				if(socket(s,Sn_MR_IPRAW,3000,0)!=0)									/*�ж�ip rawģʽsocket�Ƿ���*/
				{ }
				/* Check socket register */
				while(getSn_SR(s)!=SOCK_IPRAW);		
			break;
			case SOCK_IPRAW:																			/*ip rawģʽ*/		
				ping_request(s, addr);															/*����Ping����*/
				req++;			
			while(1)
			{
				if ( (rlen = getSn_RX_RSR(s) ) > 0)
				{
					ping_reply(s, addr, rlen);											/*��ȡ�ظ���Ϣ*/
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
*@brief		ping������
*@param		s-  socket number
*@param		addr-  P��ַ
*@return	��
*/
uint8 ping_request(uint8 s, uint8 *addr)
{
	uint16 i;
	ping_reply_received = 0;	        												/*ping �ظ���ʼ����־λ*/
	PingRequest.Type = PING_REQUEST;                  				/*Ping-Request*/
	PingRequest.Code = CODE_ZERO;	                   					/*���� '0'*/
	PingRequest.ID = htons(RandomID++);	       								/*����ping��ӦIDΪ��������ͱ���*/
	PingRequest.SeqNum =htons(RandomSeqNum++);								/*����ping��Ӧ�����к�Ϊ������α���*/
	for(i = 0 ; i < BUF_LEN; i++)
	{	                                
		PingRequest.Data[i] = (i) % 8;		 				 							/*ping��Ӧ������'0'~'8��*/
	}
	PingRequest.CheckSum = 0;
	/* ������Ӧ����*/
	PingRequest.CheckSum = htons(checksum((uint8*)&PingRequest,sizeof(PingRequest))); 
	/*����ping��Ӧ��Ŀ�ķ� */
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
*@brief		����Ping�ظ�
*@param		s-  socket number
*@param		addr- Ping��ַ
*@return	��
*/
uint8 ping_reply(uint8 s, uint8 *addr,  uint16 rlen)
{
	uint16 tmp_checksum;
	uint16 len;
	uint16 i;
	uint8 data_buf[136];
	
	uint16 port = 3000;
	PINGMSGR PingReply;
	len = recvfrom(s, data_buf,rlen,addr,&port);			/*��Ŀ�Ķ˽�������*/
	
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
		tmp_checksum = ~checksum(data_buf,len);									/*���ping�ظ��Ĵ���*/
		if(tmp_checksum != 0xffff)
			printf("tmp_checksum = %x\r\n",tmp_checksum);
		
		else
		{
			printf("Reply from %3d.%3d.%3d.%3d ID=%x Byte=%d\r\n\r\n",(addr[0]),  (addr[1]),  (addr[2]),  (addr[3]),htons(PingReply.ID),(rlen+6));			
			ping_reply_received =1;																/*���˳�ping�ظ�ѭ��ʱ������ping�ظ���־Ϊ1*/
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
		tmp_checksum = PingReply.CheckSum;											/*���ping�ظ�����*/
		PingReply.CheckSum = 0;
		if(tmp_checksum != PingReply.CheckSum)
		{
			printf( " \n CheckSum is in correct %x shold be %x \n",   (tmp_checksum),  htons(PingReply.CheckSum)) ;
		}
		else
		{		}
		printf("	Request from %d.%d.%d.%d  ID:%x SeqNum:%x  :data size %d bytes\r\n",
		(addr[0]),  (addr[1]),  (addr[2]),  (addr[3]),  (PingReply.ID),  (PingReply.SeqNum),  (rlen+6) );	   
		ping_reply_received =1;																	/* ���˳�ping�ظ�ѭ��ʱ������ping�ظ���־Ϊ1	*/
	}
	else
	{      
		printf(" Unkonwn msg. \n");
	}
	return 0;
}

/**
*@brief		ִ��ping������
*@param		��
*@return	��
*/
void do_ping(void)
{	 
	ping_count(SOCK_PING,4,remote_ip);
}




