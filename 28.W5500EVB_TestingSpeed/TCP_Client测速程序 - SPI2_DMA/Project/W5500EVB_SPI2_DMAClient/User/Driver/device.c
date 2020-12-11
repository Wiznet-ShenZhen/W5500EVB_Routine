#include "stm32f10x.h"
#include "device.h"
#include "config.h"
#include "socket.h"
#include "ult.h"
#include "w5500.h"
#include <stdio.h> 
#include <string.h>

CONFIG_MSG  ConfigMsg, RecvMsg;

uint8 txsize[MAX_SOCK_NUM] = {16,0,0,0,0,0,0,0};		// 选择8个Socket每个Socket发送缓存的大小，在w5500.c的void sysinit()有设置过程
uint8 rxsize[MAX_SOCK_NUM] = {16,0,0,0,0,0,0,0};		// 选择8个Socket每个Socket接收缓存的大小，在w5500.c的void sysinit()有设置过程

extern uint8 MAC[6];

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
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;					 /*选择要控制的GPIO引脚*/		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*设置引脚速率为50MHz */		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 /*设置引脚模式为通用推挽输出*/	
	GPIO_Init(GPIOC, &GPIO_InitStructure);							 /*调用库函数，初始化GPIO*/
	GPIO_SetBits(GPIOC, GPIO_Pin_5);		
		/*定义INT引脚*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;						 /*选择要控制的GPIO引脚*/		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*设置引脚速率为50MHz*/		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;				 /*设置引脚模式为通用推挽模拟上拉输入*/		
	GPIO_Init(GPIOC, &GPIO_InitStructure);							 /*调用库函数，初始化GPIO*/
	
}

void DMA_Configuration(void)
{
//	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	/* Deinitialize DMA Streams */
	DMA_DeInit(DMA1_Channel5);	//SPI1_TX_DMA_STREAM
	DMA_DeInit(DMA1_Channel4);	//SPI1_RX_DMA_STREAM
  DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_Cmd(DMA1_Channel5, DISABLE);
}

void Reset_W5500(void)
{
	#if 1  		
  GPIO_ResetBits(GPIOB, WIZ_RESET);
  Delay_us(2);  
  GPIO_SetBits(GPIOB, WIZ_RESET);
  Delay_ms(1600);
  #endif
	#if 0  		
  GPIO_ResetBits(GPIOC, WIZ_RESET);
  Delay_us(2);  
  GPIO_SetBits(GPIOC, WIZ_RESET);
  Delay_ms(1600);
  #endif
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


void set_network(void)															// 配置初始化IP信息并打印，初始化8个Socket
{
  uint8 ip[4];
  setSHAR(ConfigMsg.mac);
  setSUBR(ConfigMsg.sub);
  setGAR(ConfigMsg.gw);
  setSIPR(ConfigMsg.lip);

  sysinit(txsize, rxsize); 													// 初始化8个socket
  setRTR(2000);																			// 设置超时时间
  setRCR(3);																				// 设置最大重新发送次数
  
  getSIPR (ip);
  printf("W5500EVB IP : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  getSUBR(ip);
  printf("W5500EVB SN : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  getGAR(ip);
  printf("W5500EVB GW : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
}

void set_default(void)															// 设置默认MAC、IP、GW、SUB、DNS
{  
  uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};
  uint8 lip[4]={192,168,1,150};
  uint8 sub[4]={255,255,255,0};
  uint8 gw[4]={192,168,1,1};
  uint8 dns[4]={114,114,114,114};
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

//void write_config_to_eeprom(void)
//{
//  uint8 data;
//  uint16 i,j;
//  uint16 dAddr=0;
//  for (i = 0, j = 0; i < (uint8)(sizeof(ConfigMsg)-4);i++) 
//  {
//    data = *(uint8 *)(ConfigMsg.mac+j);
//    at24c16_write(dAddr, data);
//    dAddr += 1;
//    j +=1;
//  }
//}
//void get_config(void)
//{
//  uint8 tmp=0;
//  uint16 i;
//  for (i =0; i < CONFIG_MSG_LEN; i++) 
//  {
//    tmp=at24c16_read(i);
//    *(ConfigMsg.mac+i) = tmp;
//  }
//  if((ConfigMsg.mac[0]==0xff)&&(ConfigMsg.mac[1]==0xff)&&(ConfigMsg.mac[2]==0xff)&&(ConfigMsg.mac[3]==0xff)&&(ConfigMsg.mac[4]==0xff)&&(ConfigMsg.mac[5]==0xff))
//  {
//    set_default();
//  }
//}

// 中断配置程序
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;			// 抢占优先级设为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						// 子优先级设为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);																// 根据指定的参数初始化NVIC寄存器
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);  						// only app, no boot included
}

