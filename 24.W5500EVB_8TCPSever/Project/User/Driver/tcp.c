#include "tcp.h"

uint8 buffer[2048];												// ����һ��2KB�����飬�������Socket��ͨ������
uint16 local_port = 5000;												// ����һ�����ض˿ڲ���ʼ��
uint16 len = 0;

void tcps(uint8 i)
{
	switch (getSn_SR(i))										// ��ȡsocketi��״̬
	{
	case SOCK_INIT:											// Socket���ڳ�ʼ�����(��)״̬
  //printf("%d-1 ",i);
		Delay_ms(500);
		listen(i);											// �����ոմ򿪵ı��ض˿ڣ��ȴ��ͻ�������
		break;
	case SOCK_ESTABLISHED:							// Socket�������ӽ���״̬			
  //printf("%d-2 ",i);
		Delay_ms(500);
	
		Delay_ms(500);
		if (getSn_IR(i) & Sn_IR_CON)
		{
			setSn_IR(i, Sn_IR_CON);				// Sn_IR��CONλ��1��֪ͨW5500�����ѽ���
      printf("%d Connected\r\n", i);
		}
		// ���ݻػ����Գ������ݴ���λ���ͻ��˷���W5500��W5500���յ����ݺ��ٻظ��ͻ���
		len = getSn_RX_RSR(i);						// ��ȡW5500���н��ջ���Ĵ�����ֵ������len��Sn_RX_RSR��ʾ���ջ������ѽ��պͱ�������ݴ�С
		if (len > 0)
		{
			recv(i, buffer, len);						// W5500�������Կͻ��˵����ݣ���ͨ��SPI���͸�MCU
			printf("%d:%s\r\n", i, buffer);	// ���ڴ�ӡ���յ�������
			send(i, buffer, len);						// ���յ����ݺ��ٻظ��ͻ��ˣ�������ݻػ�
			memset(buffer,0,len+1);         // ���ջ�����0
		}
		// W5500�Ӵ��ڷ����ݸ��ͻ��˳����������Իس�����
		if (USART_RX_STA & 0x8000)				// �жϴ��������Ƿ�������
		{
			len = USART_RX_STA & 0x3fff;		// ��ȡ���ڽ��յ����ݵĳ���
			send(i, USART_RX_BUF, len);			// W5500��ͻ��˷�������
			USART_RX_STA = 0;								// ���ڽ���״̬��־λ��0
			memset(USART_RX_BUF, 0, len + 1);	// ���ڽ��ջ�����0
		}
		break;
	case SOCK_CLOSE_WAIT:								// Socket���ڵȴ��ر�״̬
  //printf("%d-3 ",i);
		Delay_ms(500);
		close(i);												// �ر�Socketi
		break;
	case SOCK_CLOSED:										// Socket���ڹر�״̬
  //printf("%d-4 ",i);
		Delay_ms(500);
		socket(i, Sn_MR_TCP, local_port, Sn_MR_ND);		// ��Socketi��������ΪTCP����ʱģʽ����һ�����ض˿�
		break;
	}
}

void do_8tcps(void)
{
	int i = 0;
	for (i = 0; i <= 7; i++)
	{
		tcps(i);
	}
}
