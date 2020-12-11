#include "device.h"
#include "config.h"
#include "socket.h"
#include "ult.h"
#include "w5500.h"
#include <stdio.h> 
#include <string.h>

CONFIG_MSG  ConfigMsg, RecvMsg;
	
uint8 txsize[MAX_SOCK_NUM] = {4,2,2,2,2,2,2,0};  //����Socket���ͻ����С
uint8 rxsize[MAX_SOCK_NUM] = {4,2,2,2,2,2,2,0};  //����Socket���ջ����С
//public buffer for DHCP, DNS, HTTP
uint8 pub_buf[1460];
uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};
uint8 lip[4]={192,168,1,150};
uint8 sub[4]={255,255,255,0};
uint8 gw[4]={192,168,1,1};
uint8 dns[4]={8,8,8,8};

void GPIO_Configuration(void)
{
	// ����GPIO�ĳ�ʼ���ṹ��
  GPIO_InitTypeDef GPIO_InitStructure;
	
	// ����GPIOA��GPIOB��GPIOC��AFIOʱ��
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO , ENABLE); 

  // PA0/1/2/3����
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1| GPIO_Pin_2 |GPIO_Pin_3; // ѡ��PA0/1/2/3���к�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// ͨ���ٶ���Ϊ50MHz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// ѡ���������
  GPIO_Init(GPIOA, &GPIO_InitStructure);							// �������
  
  GPIO_ResetBits(GPIOA, GPIO_Pin_0);		// PA0(USART2_CTS/WKUP)��0
  GPIO_ResetBits(GPIOA, GPIO_Pin_1);		// PA1(USART2_RTS)��0
  GPIO_SetBits(GPIOA, GPIO_Pin_2); 			// PA2(USART2_Tx)��1
  GPIO_SetBits(GPIOA, GPIO_Pin_3); 			// PA3(USART2_Rx)��1 
  
	// PB9����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_Pin_9);			

	// PC6����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		// ��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void Reset_W5500(void)
{
  GPIO_ResetBits(GPIOB, WIZ_RESET);
  Delay_us(2);  
  GPIO_SetBits(GPIOB, WIZ_RESET);
  Delay_ms(1600);
}

void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  JumpAddress = *(vu32*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}

void set_network(void)															// ���ó�ʼ��IP��Ϣ����ӡ����ʼ��8��Socket
{
  setSHAR(ConfigMsg.mac);/*����Mac��ַ*/
  setSUBR(ConfigMsg.sub);/*������������*/
  setGAR(ConfigMsg.gw);/*����Ĭ������*/
  setSIPR(ConfigMsg.lip);/*����Ip��ַ*/

  sysinit(txsize, rxsize); /*��ʼ��8��socket*/
  
  setRTR(2000);/*�������ʱ��ֵ*/
  setRCR(3);/*����������·��ʹ���*/
  
  getSIPR (lip);
  printf("W5500EVB IP : %d.%d.%d.%d\r\n", lip[0],lip[1],lip[2],lip[3]);
  getSUBR(sub);
  printf("W5500EVB SN : %d.%d.%d.%d\r\n", sub[0],sub[1],sub[2],sub[3]);
  getGAR(gw);
  printf("W5500EVB GW : %d.%d.%d.%d\r\n", gw[0],gw[1],gw[2],gw[3]); 
}

void set_default(void)                            // ����Ĭ��MAC��IP��GW��SUB��DNS
{  
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

