#include "include.h"

CONFIG_MSG  ConfigMsg, RecvMsg;

uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};		// ѡ��8��Socketÿ��Socket���ͻ���Ĵ�С����w5500.c��void sysinit()�����ù���
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};		// ѡ��8��Socketÿ��Socket���ջ���Ĵ�С����w5500.c��void sysinit()�����ù���

extern uint8 MAC[6];
uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};		
uint8 lip[4]={192,168,1,150};
uint8 sub[4]={255,255,255,0};
uint8 gw[4]={192,168,1,1};
uint8 dns[4]={8,8,8,8};
//public buffer for DHCP, DNS, HTTP
uint8 pub_buf[1460];

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO , ENABLE); 
  // Port A output
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1| GPIO_Pin_2 |GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOA, GPIO_Pin_0);
  GPIO_ResetBits(GPIOA, GPIO_Pin_1);
  GPIO_SetBits(GPIOA, GPIO_Pin_2); // led off
  GPIO_SetBits(GPIOA, GPIO_Pin_3); // led off  
  // Port B output;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_Pin_9);

  // Port C input
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
}
void Reset_W5500(void)
{
  GPIO_ResetBits(GPIOB, WIZ_RESET);
  Delay_us(2);  
  GPIO_SetBits(GPIOB, WIZ_RESET);
  Delay_ms(1600);
}
//reboot 
void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  JumpAddress = *(vu32*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}

void set_network(void)														// ���ó�ʼ��IP��Ϣ����ӡ����ʼ��8��Socket
{
  uint8 ip[4];
  setSHAR(ConfigMsg.mac);
  setSUBR(ConfigMsg.sub);
  setGAR(ConfigMsg.gw);
  setSIPR(ConfigMsg.lip);

  sysinit(txsize, rxsize); 												// ��ʼ��8��socket
  
  setRTR(2000);																		// ���ó�ʱʱ��
  setRCR(3);																			// ����������·��ʹ���
  
	getSIPR (ip);
  printf("W5500EVB IP : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  getSUBR(ip);
  printf("W5500EVB SN : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  getGAR(ip);
  printf("W5500EVB GW : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]); 
}

void set_default(void)														// ����Ĭ��MAC��IP��GW��SUB��DNS
{  
//	uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};		
//  uint8 lip[4]={192,168,1,150};
//  uint8 sub[4]={255,255,255,0};
//  uint8 gw[4]={192,168,1,1};
//  uint8 dns[4]={8,8,8,8};
  memcpy(ConfigMsg.lip, lip, 4);
  memcpy(ConfigMsg.sub, sub, 4);
  memcpy(ConfigMsg.gw,  gw, 4);
  memcpy(ConfigMsg.mac, mac,6);
  memcpy(ConfigMsg.dns,dns,4);

  ConfigMsg.dhcp=0;
  ConfigMsg.debug=1;
  ConfigMsg.fw_len=0;
  
  ConfigMsg.state=NORMAL_STATE;
  ConfigMsg.sw_ver[0]=FW_VER_HIGH;
  ConfigMsg.sw_ver[1]=FW_VER_LOW;
  
}


// �ж����ó���
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;			// ��ռ���ȼ���Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						// �����ȼ���Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								// IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);																// ����ָ���Ĳ�����ʼ��NVIC�Ĵ���
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);  						// only app, no boot included
}
