#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "udp.h"
#include <string.h>

// ����һ��2KB�����飬�������Socket��ͨ������
uint8 buffer[2048];												

uint8 remote_ip[4]={0,0,0,0};			  // ����Զ��IP��ַ
uint16 remote_port=0;								// ����Զ�̶˿�
uint16 local_port=5000;							// ��ʼ��һ�����ض˿�
uint16 len=0;	

	/*Socket״̬����MCUͨ����Sn_SR(0)��ֵ�����ж�SocketӦ�ô��ں���״̬
		Sn_SR״̬������
		0x00		SOCK_CLOSED
		0x13		SOCK_INIT
		0x14		SOCK_LISTEN
		0x17		SOCK_ESTABLISHED
		0x1C		SOCK_CLOSE_WAIT
		0x22		SOCK_UDP
	*/
void do_udp()
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
						memset(buffer,0,len+1);
						recvfrom(0,buffer, len, remote_ip,&remote_port);			// W5500��������Զ����λ�������ݣ���ͨ��SPI���͸�MCU
						printf("%s\r\n",buffer);															// ���ڴ�ӡ���յ�������
						sendto(0,buffer,len, remote_ip, remote_port);		  		// ���յ����ݺ��ٻظ�Զ����λ����������ݻػ�
						//memset(buffer,0,sizeof(buffer));
					}
			break;
			case SOCK_CLOSED:																						// Socket���ڹر�״̬
					socket(0,Sn_MR_UDP,local_port,0);												// ��Socket0��������ΪUDPģʽ����һ�����ض˿�
			break;
		}
}
