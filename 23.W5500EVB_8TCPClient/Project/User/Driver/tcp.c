#include "tcp.h"

uint8 buffer[2048];												// ����һ��2KB�����飬�������Socket��ͨ������
uint8 server_ip1[4]={192,168,1,101};			// ����Զ�̷�����IP��ַ
uint8 server_ip2[4]={192,168,1,102};			// ����Զ�̷�����IP��ַ
uint16 server_port1=5000;								// ����Զ�̷������˿�
uint16 server_port2=6000;								// ����Զ�̷������˿�
uint16 server_port3=7000;								// ����Զ�̷������˿�
uint16 server_port4=8000;								// ����Զ�̷������˿�

uint16 local_port = 5000;												// ����һ�����ض˿ڲ���ʼ��
uint16 len = 0;

void tcpc(uint8 i)
{
	switch (getSn_SR(i))														// ��ȡsocket0��״̬
	{
	case SOCK_INIT:															// Socket���ڳ�ʼ�����(��)״̬
		//printf("Socket%d-1 ", i);
		Delay_ms(500);
		if (i == 0)
		{	connect(i, server_ip1, server_port1);	}		// ����Sn_CRΪCONNECT������TCP������������������
		else if(i == 1)
		{	connect(i, server_ip1, server_port2);	}		// ����Sn_CRΪCONNECT������TCP������������������
		else if(i == 2)
		{	connect(i, server_ip1, server_port3);	}		// ����Sn_CRΪCONNECT������TCP������������������
		else if(i == 3)
		{	connect(i, server_ip1, server_port4);	}		// ����Sn_CRΪCONNECT������TCP������������������
		else if(i == 4)
		{	connect(i, server_ip2, server_port1);	}		// ����Sn_CRΪCONNECT������TCP������������������
		else if(i == 5)
		{	connect(i, server_ip2, server_port2);	}		// ����Sn_CRΪCONNECT������TCP������������������
		else if(i == 6)
		{	connect(i, server_ip2, server_port3);	}		// ����Sn_CRΪCONNECT������TCP������������������
		else if(i == 7)
		{	connect(i, server_ip2, server_port4);	}		// ����Sn_CRΪCONNECT������TCP�������������
		
		break;
		
	case SOCK_ESTABLISHED:											// Socket�������ӽ���״̬
		//printf("Socket%d-2 ", i);
		Delay_ms(500);
		if (getSn_IR(i) & Sn_IR_CON)
		{
			setSn_IR(i, Sn_IR_CON);								// Sn_IR��CONλ��1��֪ͨW5500�����ѽ���
			printf("%d:connected.\r\n", i);
		}
		// ���ݻػ����Գ������ݴ���λ������������W5500��W5500���յ����ݺ��ٻظ�������
		len = getSn_RX_RSR(i);										// len=Socket0���ջ������ѽ��պͱ�������ݴ�С
		if (len > 0)
		{
			recv(i, buffer, len);										// W5500�������Է����������ݣ���ͨ��SPI���͸�MCU
			printf("%d:%s\r\n", i, buffer);							// ���ڴ�ӡ���յ�������
      send(i,buffer,len);										// ���յ����ݺ��ٻظ���������������ݻػ�
			memset(buffer,0,len+1);
		}
		// W5500�Ӵ��ڷ����ݸ��ͻ��˳����������Իس�����
		if (USART_RX_STA & 0x8000)								// �жϴ��������Ƿ�������
		{
			len = USART_RX_STA & 0x3fff;						// ��ȡ���ڽ��յ����ݵĳ���
			send(i, USART_RX_BUF, len);							// W5500��ͻ��˷�������
			USART_RX_STA = 0;												// ���ڽ���״̬��־λ��0
			memset(USART_RX_BUF, 0, len + 1);					// ���ڽ��ջ�����0
		}
		break;
		
	case SOCK_CLOSE_WAIT:												// Socket���ڵȴ��ر�״̬
		//printf("Socket%d-3 ", i);
		Delay_ms(500);
		len = getSn_RX_RSR(i);										// len=Socket0���ջ������ѽ��պͱ�������ݴ�С
		if (len > 0)
		{
			recv(i, buffer, len);										// W5500�������Է����������ݣ���ͨ��SPI���͸�MCU
			printf("%d:%s\r\n", i, buffer);							// ���ڴ�ӡ���յ�������
			send(i, buffer, len);										// ���յ����ݺ��ٻظ���������������ݻػ�
		}
		disconnect(i);																// �Ͽ�Socket0������
		break;
		
	case SOCK_CLOSED:														// Socket���ڹر�״̬
		//printf("Socket%d-4 ", i);
		Delay_ms(500);
		socket(i, Sn_MR_TCP, local_port++, Sn_MR_ND);		// ��Socket0��������ΪTCP����ʱģʽ����һ�����ض˿�
		break;
	}
}

void do_8tcpc(void)
{
	int i = 0;
	for (i = 0; i <= 7; i++)
	{
		tcpc(i);
	}
}
