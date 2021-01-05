#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "multcast.h"
#include <string.h>

uint8 buffer[2048];												// ����һ��2KB�����飬�������Socket��ͨ������

uint8 remote_ip[4];			// ����Զ��IP��ַ
uint16 remote_port;								// ����Զ�̶˿�
uint16 local_port=4000;									// ��ʼ��һ�����ض˿�
uint16 len=0;

uint8 DIP[4]={224,1,1,1};
uint8 DHAR[6]={0x01,0x00,0x5e,0x01,0x01,0x01};
uint16 DPORT=4000;

void do_multcast()
{
	switch(getSn_SR(0))																						// ��ȡsocket0��״̬
	{
		case SOCK_UDP:																							// Socket���ڳ�ʼ�����(��)״̬
				Delay_ms(100);
				if(getSn_IR(0) & Sn_IR_RECV)
				{
					setSn_IR(0, Sn_IR_RECV);															// Sn_IR��RECVλ��1
				}
				// ���ݻػ����Գ������ݴ�Զ����λ������W5500��W5500���յ����ݺ��ٻظ�Զ����λ��
				if((len=getSn_RX_RSR(0))>0)
				{ 
					recvfrom(0,buffer, len, remote_ip,&remote_port);			// W5500��������Զ����λ�������ݣ���ͨ��SPI���͸�MCU
					printf("%d.%d.%d.%d:%d\r\n", remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3],remote_port);
					printf("%s", buffer);														// ���ڴ�ӡ���յ�������
					sendto(0,buffer,len, DIP, DPORT);		  		      // ���յ����ݺ��ٻظ�Զ����λ����������ݻػ�
					memset(buffer,0,len+1);                         // ��ս�������
				}
		break;
		case SOCK_CLOSED:																						// Socket���ڹر�״̬
				 setDIPR(0,DIP);
				 setDHAR(0,DHAR);
				 setDPORT(0,DPORT);
				 socket(0,0x82,4000,0);                      /*��ʼ��socket 0���׽���*/									
		break;
	}
}
