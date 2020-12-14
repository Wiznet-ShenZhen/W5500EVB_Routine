#include "device.h"
#include "config.h"
#include "socket.h"
#include "ult.h"
#include "w5500.h"
#include <stdio.h> 
#include <string.h>
#include "arp.h"

CONFIG_MSG  ConfigMsg, RecvMsg;

uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};

uint8 pub_buf[1460];
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO , ENABLE); 
  // Port A output
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1| GPIO_Pin_2 |GPIO_Pin_3|GPIO_Pin_8|GPIO_Pin_15; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOA, GPIO_Pin_0);
  GPIO_ResetBits(GPIOA, GPIO_Pin_1);
  GPIO_SetBits(GPIOA, GPIO_Pin_2); // led off
  GPIO_SetBits(GPIOA, GPIO_Pin_3); // led off  
  // Port B output;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_Pin_0);

  // Port C input
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	  // Port C output
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void Reset_W5500(void)
{
  GPIO_ResetBits(GPIOB, WIZ_RESET);
  Delay_us(2);  
  GPIO_SetBits(GPIOB, WIZ_RESET);
  Delay_ms(800); 
}

void set_network(void)
{
  uint8 ip[4];
  setSHAR(ConfigMsg.mac);
  setSUBR(ConfigMsg.sub);
  setGAR(ConfigMsg.gw);
  setSIPR(ConfigMsg.lip);

  //Init. TX & RX Memory size of w5500
  sysinit(txsize, rxsize); 
  
  setRTR(2000);//200ms
  setRCR(3);
  
  getSIPR (ip);
  
  printf("W5500EVB IP : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);

  getSUBR(ip);

  printf("W5500EVB SN : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);

  getGAR(ip);

  printf("W5500EVB GW : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
  //ConfigMsg.mac[5]=1;
 
}

 void set_default(void)
{
  
  uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};//{0xff,0xff,0xff,0xff,0xff,0xff};//
  uint8 lip[4]={192,168,1,150};
  uint8 sub[4]={255,255,255,0};
  uint8 gw[4]={192,168,1,1};
  uint8 dns[4]={8,8,8,8};
  uint8 rip[4]={192,168,1,1};
  memcpy(ConfigMsg.lip, lip, 4);
  memcpy(ConfigMsg.sub, sub, 4);
  memcpy(ConfigMsg.gw,  gw, 4);
  memcpy(ConfigMsg.mac, mac,6);
  memcpy(ConfigMsg.dns,dns,4);
  memcpy(ConfigMsg.rip,rip,4);
  ConfigMsg.rport=5000;

  ConfigMsg.dhcp=0;
  ConfigMsg.debug=1;

  ConfigMsg.sw_ver[0]=FW_VER_HIGH;
  ConfigMsg.sw_ver[1]=FW_VER_LOW;
  
}

