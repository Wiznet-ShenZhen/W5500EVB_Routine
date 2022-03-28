#include "usart.h"
#include "device.h"
#include "spi2.h"
#include "ult.h"
#include "socket.h"
#include "w5500.h"
#include "24c16.h"
#include "md5.h"
#include "string.h"

extern uint8 txsize[];										// �����ⲿ����������Socket���ͻ����С
extern uint8 rxsize[];										// �����ⲿ����������Socket���ջ����С

uint8 buffer[2048];												// ����һ��2KB�����飬�������Socket��ͨ������

int main(void)
{
  uint16 local_port=6000;												// ����һ�����ض˿ڲ���ʼ��
  uint16 len=0; 													
	uint8 phyconf=0;
	/***** MCUʱ�ӳ�ʼ�� *****/							  
	Systick_Init(72);	
	
	/***** �жϿ��������� *****/
//	NVIC_Configuration();
	
	/***** GPIO��SPI��ʼ�� *****/
	GPIO_Configuration();			
	WIZ_SPI_Init();
	
	/***** ���ڳ�ʼ�� *****/
	USART1_Init(); 		
	
	/***** ��ʼ��eeprom *****/
  erase_eeprom(0,255);
	at24c16_init();
	
	/***** ��λW5500 *****/
	Reset_W5500();
	
	/***** W5500��IP��Ϣ��ʼ�� *****/
	set_default(); 														// ����Ĭ��MAC��IP��GW��SUB��DNS
	set_network();														// ���ó�ʼ��IP��Ϣ����ӡ����ʼ��8��Socket
	
	/***** ��W5500��Keepalive���� *****/
	setkeepalive(0);
	
	printf("TCP Server Port:%d\r\n",local_port);
	printf("W5500 Init Complete!\r\n");
  printf("Start TCP Server Test!\r\n"); 
	
	//10Mbps
	
	phyconf=IINCHIP_READ(PHYCFGR);						//��PHY���üĴ�����ֵ
	printf("phy1=0x%x\r\n",phyconf);
	
	printf("phy s/w reset\r\n");
	IINCHIP_WRITE(PHYCFGR,0x7F);							//��λW5500����λ��������üĴ�����
//	printf("phy=10Mbps(0xcf)\r\n");
	IINCHIP_WRITE(PHYCFGR,0xDF);							//����PHY�Ĵ���Ϊ10Mȫ˫��ģʽ��SPD���� 
	IINCHIP_WRITE(PHYCFGR,0x7F);							//��λW5500����λ��������üĴ�����
	IINCHIP_WRITE(PHYCFGR,0xCF);							//����PHY�Ĵ���Ϊ100Mȫ˫��ģʽ��SPD���� 
//	IINCHIP_WRITE(PHYCFGR,0xBF);							//����PHY�Ĵ���Ϊ10/100Mȫ˫��ģʽ��SPD��״̬δ֪ 
//	Delay_ms(5000);
//	Delay_ms(5000);
//	Delay_ms(5000);
//	Delay_ms(5000);
//	Delay_ms(5000);
//	Delay_ms(5000);
//	//
//	//10Mbps full duplex=0xCF;
//	//PHY power off =0xF7
//	printf("phy power off\r\n");
//	IINCHIP_WRITE(PHYCFGR,0x7f);							//��λ
//	IINCHIP_WRITE(PHYCFGR,0xF7);							//��λ����������PHY�Ĵ���Ϊ����ģʽ
//	Delay_ms(5000);
//	Delay_ms(5000);
//	Delay_ms(5000);
//	phyconf=IINCHIP_READ(PHYCFGR);
//	printf("phy2=0x%x, phy h/w reset\r\n",phyconf);
//	Reset_W5500();														//����ģʽ����ͨ��Ӳ����λ����
//	set_network();														//Ӳ����λ����Ҫ��������W5500
//	//IINCHIP_WRITE(PHYCFGR,0x7F);
	Delay_ms(5000);
	Delay_ms(5000);
	Delay_ms(5000);
	Delay_ms(5000);
	phyconf=IINCHIP_READ(PHYCFGR);
	printf("phy3=0x%x\r\n",phyconf);
	
	/*Socket״̬����MCUͨ����Sn_SR(0)��ֵ�����ж�SocketӦ�ô��ں���״̬
		Sn_SR״̬������
		0x00		SOCK_CLOSED
		0x13		SOCK_INIT
		0x14		SOCK_LISTEN
		0x17		SOCK_ESTABLISHED
		0x1C		SOCK_CLOSE_WAIT
		0x22		SOCK_UDP
	*/
	while(1)																	// Socket״̬��
	{
			switch(getSn_SR(0))										// ��ȡsocket0��״̬
			{
				case SOCK_INIT:											// Socket���ڳ�ʼ�����(��)״̬
						listen(0);											// �����ոմ򿪵ı��ض˿ڣ��ȴ��ͻ�������
				break;
				case SOCK_ESTABLISHED:							// Socket�������ӽ���״̬
						if(getSn_IR(0) & Sn_IR_CON)			
						{
							setSn_IR(0, Sn_IR_CON);				// Sn_IR��CONλ��1��֪ͨW5500�����ѽ���
						}
						// ���ݻػ����Գ������ݴ���λ���ͻ��˷���W5500��W5500���յ����ݺ��ٻظ��ͻ���
						len=getSn_RX_RSR(0);						// ��ȡW5500���н��ջ���Ĵ�����ֵ������len��Sn_RX_RSR��ʾ���ջ������ѽ��պͱ�������ݴ�С
						if(len>0)
						{
							recv(0,buffer,len);						// W5500�������Կͻ��˵����ݣ���ͨ��SPI���͸�MCU
							printf("%s\r\n",buffer);			// ���ڴ�ӡ���յ�������
						}
						// W5500�Ӵ��ڷ����ݸ��ͻ��˳����������Իس�����
						if(USART_RX_STA & 0x8000)				// �жϴ��������Ƿ�������
						{					   
							len = USART_RX_STA & 0x3fff;		// ��ȡ���ڽ��յ����ݵĳ���
							send(0,USART_RX_BUF,len);			// W5500��ͻ��˷�������
							USART_RX_STA = 0;								// ���ڽ���״̬��־λ��0
							memset(USART_RX_BUF,0,len+1);	// ���ڽ��ջ�����0
						}
				break;
				case SOCK_CLOSE_WAIT:								// Socket���ڵȴ��ر�״̬
						close(0);												// �ر�Socket0
				break;
				case SOCK_CLOSED:										// Socket���ڹر�״̬
						socket(0,Sn_MR_TCP,local_port,Sn_MR_ND);		// ��Socket0��������ΪTCP����ʱģʽ����һ�����ض˿�
				break;
			}
	}
}

