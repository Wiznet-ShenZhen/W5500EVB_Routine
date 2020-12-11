#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "string.h"
#include "tcp.h"

extern uint8 txsize[];										// �����ⲿ����������Socket���ͻ����С
extern uint8 rxsize[];										// �����ⲿ����������Socket���ջ����С

uint8  buffer[16384];

uint8 server_ip[4] = { 192,168,1,101 };			// ����Զ�̷�����IP��ַ
uint16 server_port = 3000;								// ����Զ�̷������˿�
uint16 local_port = 5000;									// ��ʼ��һ�����ض˿�
uint16 len = 0;

/*****
	Socket״̬����MCUͨ����Sn_SR(0)��ֵ�����ж�SocketӦ�ô��ں���״̬
	Sn_SR״̬������
	0x00		SOCK_CLOSED
	0x13		SOCK_INIT
	0x14		SOCK_LISTEN
	0x17		SOCK_ESTABLISHED
	0x1C		SOCK_CLOSE_WAIT
	0x22		SOCK_UDP
*****/
void tcpc()
{
	switch (getSn_SR(0))														// ��ȡsocket0��״̬
	{
	case SOCK_INIT:															// Socket���ڳ�ʼ�����(��)״̬
		connect(0, server_ip, server_port);			// ����Sn_CRΪCONNECT������TCP������������������		
		break;
	case SOCK_ESTABLISHED:											// Socket�������ӽ���״̬
		if (getSn_IR(0) & Sn_IR_CON)
		{
			setSn_IR(0, Sn_IR_CON);								// Sn_IR��CONλ��1��֪ͨW5500�����ѽ���
		}
		// ���ݻػ����Գ������ݴ���λ������������W5500��W5500���յ����ݺ��ٻظ�������
		len = getSn_RX_RSR(0);										// len=Socket0���ջ������ѽ��պͱ�������ݴ�С

		if (len >= sizeof(buffer))
		{
			recv(0, buffer, len);										// W5500�������Է����������ݣ���ͨ��SPI���͸�MCU
//						printf("%d\r\n",len);							    // ���ڴ�ӡ���յ�������
			send(0, buffer, len);										// ���յ����ݺ��ٻظ���������������ݻػ�
		}
		else if (len > 0)
		{
			len = recv(0, buffer, len);
			send(0, buffer, len);										// ���յ����ݺ��ٻظ���������������ݻػ�
		}
		// W5500�Ӵ��ڷ����ݸ��ͻ��˳����������Իس�����
//					if(USART_RX_STA & 0x8000)								// �жϴ��������Ƿ�������
//					{					   
//						len=USART_RX_STA & 0x3fff;						// ��ȡ���ڽ��յ����ݵĳ���							
//						send(0,USART_RX_BUF,len);							// W5500��ͻ��˷�������						
//						USART_RX_STA=0;												// ���ڽ���״̬��־λ��0
//						memset(USART_RX_BUF,0,len+1);					// ���ڽ��ջ�����0						
//					}
		break;
	case SOCK_CLOSE_WAIT:												  // Socket���ڵȴ��ر�״̬
//					len=getSn_RX_RSR(0);										// len=Socket0���ջ������ѽ��պͱ�������ݴ�С
//					if(len>0)
//					{
//						recv(0,buffer,len);										// W5500�������Է����������ݣ���ͨ��SPI���͸�MCU
////						printf("%s\r\n",buffer);							// ���ڴ�ӡ���յ�������
////						send(0,buffer,len);										// ���յ����ݺ��ٻظ���������������ݻػ�
//					}
		disconnect(0);
		break;
	case SOCK_CLOSED:														// Socket���ڹر�״̬
		socket(0, Sn_MR_TCP, local_port, Sn_MR_ND);		// ��Socket0��������ΪTCP����ʱģʽ����һ�����ض˿�
		break;
	}
}

