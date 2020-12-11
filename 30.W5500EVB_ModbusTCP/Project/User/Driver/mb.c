#include "mbtcp.h"
#include "mbascii.h"
#include "usart.h"
#include "w5500.h"
#include "socket.h"

volatile uint8* pucASCIIBufferCur;
volatile uint16 usASCIIBufferPos;
volatile uint8 *pucTCPBufferCur;
volatile uint16 usTCPBufferPos;

void PutByte(uint8 ch, uint8 data)
{
  USART_TypeDef* USARTx;  
	
  USARTx = USART1;
  USART1->SR;
	USARTx->DR=(data | 0x80) ;
  while((USARTx->SR & USART_FLAG_TC)==RESET);
}

void mbTCPtoEVB(void)
{
	
	if(MBtcp2evbFrame() != FALSE)  //帧接收完成
	{
		if((uint8)pucASCIIBufferCur[3]==0x00)
		{
			if((uint8)pucASCIIBufferCur[4]==0xff)
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_0);
				printf("LED 0 ON\r\n");
			}
			else if((uint8)pucASCIIBufferCur[4]==0x00)
			{
			  GPIO_SetBits(GPIOA, GPIO_Pin_0);
			}
		}
		else if(pucASCIIBufferCur[3]==0x01)
		{
			if((uint8)pucASCIIBufferCur[4]==0xff)
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_1);
				printf("LED 1 ON\r\n");
			}
			else if((uint8)pucASCIIBufferCur[4]==0x00)
			{
			  GPIO_SetBits(GPIOA, GPIO_Pin_1);
			}
		}
		else if(pucASCIIBufferCur[3]==0x02)
		{
			if((uint8)pucASCIIBufferCur[4]==0xff)
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_2);
				printf("LED 2 ON\r\n");
			}
			else if((uint8)pucASCIIBufferCur[4]==0x00)
			{
			  GPIO_SetBits(GPIOA, GPIO_Pin_2);
			}
		}
		else if(pucASCIIBufferCur[3]==0x03)
		{
			if((uint8)pucASCIIBufferCur[4]==0xff)
			{
				GPIO_ResetBits(GPIOA, GPIO_Pin_3);
				printf("LED 3 ON\r\n");
			}
			else if((uint8)pucASCIIBufferCur[4]==0x00)
			{
			  GPIO_SetBits(GPIOA, GPIO_Pin_3);
			}
		}
//		while(usASCIIBufferPos)
//  	{      
//			printf("%0x ",(uint8)*pucASCIIBufferCur);
//		  pucASCIIBufferCur++;
//		  usASCIIBufferPos--;
//	  }
	}
}

void mbEVBtoTCP(void)
{
	if(MBevb2tcpFrame() != FALSE)
	{
		switch(getSn_SR(0))
		{	 
		case SOCK_ESTABLISHED:
    case SOCK_CLOSE_WAIT:
			send(0, (uint8*)pucTCPBufferCur, usTCPBufferPos);
			break;
		default:
      break;
	  }
	}
}

