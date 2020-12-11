#include <string.h>
#include "config.h"
#include "types.h"
#include "w5500.h"
#include "socket.h"
#include "mbascii.h"
#include "mbtcp.h"

#define MB_TCP_BUF_SIZE     ( 256 + 7 ) /* Must hold a complete Modbus TCP frame. */

#define MB_TCP_TID1         0
#define MB_TCP_TID2         1
#define MB_TCP_PID          2
#define MB_TCP_LEN          4
#define MB_TCP_UID          6
#define MB_TCP_FUNC         7

#define MB_UDP_CRC16_SIZE   2

#define MB_TCP_PROTOCOL_ID  0   /* 0 = Modbus Protocol */

uint8 b_listening_printed = 0;
uint8 mbTCPtid1, mbTCPtid2;
static uint8    aucTCPBuf[MB_TCP_BUF_SIZE];

extern volatile uint8* pucASCIIBufferCur;
extern volatile uint16 usASCIIBufferPos;
extern uint8 lip[4];
extern uint16 local_port;

static bool mbTCPGet( uint8 ** ppucMBTCPFrame, uint16 * usTCPLength )
{
	uint16 len;
	uint16   usTCPBufPos;
	
	len = getSn_RX_RSR(0);
	
	if(len > 0)
	{
		switch(getSn_SR(0))
		{		
			case SOCK_ESTABLISHED:
			case SOCK_CLOSE_WAIT:
				usTCPBufPos = recv(0, aucTCPBuf, len);
				break;
			default:
				break;
		}
		*ppucMBTCPFrame = &aucTCPBuf[0];
	  *usTCPLength = usTCPBufPos;
		return TRUE;
	}
	return FALSE;
}

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- MBAP Header --------------------------------------*/
/*
 *
 * <------------------------ MODBUS TCP/IP ADU(1) ------------------------->
 *              <----------- MODBUS PDU (1') ---------------->
 *  +-----------+---------------+------------------------------------------+
 *  | TID | PID | Length | UID  |Code | Data                               |
 *  +-----------+---------------+------------------------------------------+
 *  |     |     |        |      |                                           
 * (2)   (3)   (4)      (5)    (6)                                          
 *
 * (2)  ... MB_TCP_TID          = 0 (Transaction Identifier - 2 Byte) 
 * (3)  ... MB_TCP_PID          = 2 (Protocol Identifier - 2 Byte)
 * (4)  ... MB_TCP_LEN          = 4 (Number of bytes - 2 Byte)
 * (5)  ... MB_TCP_UID          = 6 (Unit Identifier - 1 Byte)
 * (6)  ... MB_TCP_FUNC         = 7 (Modbus Function Code)
 *
 * (1)  ... Modbus TCP/IP Application Data Unit
 * (1) ... Modbus Protocol Data Unit
 */
static bool mbTCPPackage( uint8* pucRcvAddress, uint8** ppucFrame, uint16 * pusLength )
{
	uint8  *pucMBTCPFrame;
	uint16  usLength;
	uint16  usPID;

	if( mbTCPGet( &pucMBTCPFrame, &usLength ) != FALSE )//判断是否有网络数据并获取数据帧和长度
	{
		//解析基于TCP传输的MODUBUS协议的7个字节报文头
		 usPID = pucMBTCPFrame[MB_TCP_PID] << 8U;    //2个字节的协议标识符  0 ---> modubus协议
		 usPID |= pucMBTCPFrame[MB_TCP_PID + 1];

		 if( usPID == MB_TCP_PROTOCOL_ID )
		 {
				/* Modbus TCP does not use any addresses. Fake the source address such
				 * that the processing part deals with this frame.
				 */
				*pucRcvAddress = pucMBTCPFrame[MB_TCP_UID]; //获取单元标识符以后帧的长度
				/*记录事物处理标识符*/
				mbTCPtid1 = pucMBTCPFrame[MB_TCP_TID1];
				mbTCPtid2 = pucMBTCPFrame[MB_TCP_TID2];
				
				*ppucFrame = &pucMBTCPFrame[MB_TCP_FUNC];//获取单元标识符
				*pusLength = usLength - MB_TCP_FUNC;
			  return TRUE;
		 }
	}
	return FALSE;
}
// 组包
bool MBtcp2evbFrame(void)
{
	uint8 pucRcvAddress;
	uint16 pusLength;
	uint8* ppucFrame;
	uint8 usLRC;
	
	if(mbTCPPackage( &pucRcvAddress, &ppucFrame, &pusLength ) != FALSE) //解析数据帧
	{
		pucASCIIBufferCur = ppucFrame - 1;//获定位功能码以及后面的数据
		pucASCIIBufferCur[MB_SER_PDU_ADDR_OFF] = pucRcvAddress;//获取功能码以及后面的数据
		usASCIIBufferPos = pusLength + MB_RTU_ADDR_SIZE;//获取数据大小
		usLRC = prvucMBLRC( ( uint8 * ) pucASCIIBufferCur, usASCIIBufferPos );//LRC校验
		pucASCIIBufferCur[usASCIIBufferPos++] = usLRC;
		
		return TRUE;
	}
	return FALSE;
}
void do_Modbus(void)
{
	uint8_t state=0;
	
	state=getSn_SR(0);
	switch(state)
  {
		case SOCK_SYNSENT:
			break;
		case SOCK_INIT:  
				listen(0);      
				if(!b_listening_printed)
				{
					b_listening_printed=1;
					 printf("Listening on %d.%d.%d.%d:%d\r\n",
					lip[0],lip[1],lip[2],lip[3],local_port);
				 }       
		break;
		case SOCK_LISTEN:
				break;
		case SOCK_ESTABLISHED:
			if(getSn_IR(0) & Sn_IR_CON)
			{
				uint8_t rip[4];      
				setSn_IR(0, Sn_IR_CON);
				printf("Connected\r\n");
				rip[0]=IINCHIP_READ( Sn_DIPR0(0));
				rip[1]=IINCHIP_READ( Sn_DIPR1(0));
				rip[2]=IINCHIP_READ( Sn_DIPR2(0));
				rip[3]=IINCHIP_READ( Sn_DIPR3(0));
				
				printf("RemoteIP:%d.%d.%d.%d Port:%d\r\n",	rip[0],rip[1],rip[2],rip[3],
				IINCHIP_READ( Sn_DPORT0(0))*256+IINCHIP_READ( Sn_DPORT1(0))
				);
				
				if(b_listening_printed) b_listening_printed=0;
			}		
			if(getSn_RX_RSR(0))
			 {
				mbTCPtoEVB();
			 }		
			break;
		case SOCK_CLOSE_WAIT:
			disconnect(0);
			break;
		case SOCK_CLOSED:
		case SOCK_FIN_WAIT:
			close(0);	  
			socket(0, Sn_MR_TCP,local_port, Sn_MR_ND);	//Sn_MR_ND     
			break;
		default:
			break;         
  }

}

