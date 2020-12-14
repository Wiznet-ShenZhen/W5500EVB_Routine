
#include "config.h"
#include "socket.h"
#include "util.h"
#include "w5500.h"
#include "device.h"

#include <stdio.h> 
#include <string.h>

CONFIG_MSG  ConfigMsg, RecvMsg;

uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};

//public buffer for DHCP, DNS, HTTP
uint8 pub_buf[1460];

void Reset_W5500(void)
{
  GPIO_ResetBits(GPIOB, WIZ_RESET);
  Delay_us(50);  
  GPIO_SetBits(GPIOB, WIZ_RESET);
  Delay_us(50);
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
void USART1_Init(void)
{
  USART_InitTypeDef USART_InitStructure;

  /* USARTx configuration ------------------------------------------------------*/
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //Enable rx enable, 
  
  /* Configure the USARTx */ 
  USART_Init(USART1, &USART_InitStructure);
  /* Enable the USARTx */
  USART_Cmd(USART1, ENABLE);
}

void set_network(void)
{
  uint8 ip[4];
  setSHAR(ConfigMsg.mac);/*配置Mac地址*/
  setSUBR(ConfigMsg.sub);/*配置子网掩码*/
  setGAR(ConfigMsg.gw);/*配置默认网关*/
  setSIPR(ConfigMsg.lip);/*配置Ip地址*/

  //Init. TX & RX Memory size of w5500
  sysinit(txsize, rxsize); /*初始化8个socket*/
  
  setRTR(2000);/*设置溢出时间值*/
  setRCR(3);/*设置最大重新发送次数*/
  

  getSIPR (ip);

  printf("W5500EVB IP : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);

  getSUBR(ip);

  printf("W5500EVB SN : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);

  getGAR(ip);

  printf("W5500EVB GW : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
 
}

void set_default(void)
{  
  uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};
  uint8 lip[4]={192,168,1,150};
  uint8 sub[4]={255,255,255,0};
  uint8 gw[4]={192,168,1,1};
  uint8 dns[4]={8,8,8,8};
  memcpy(ConfigMsg.lip, lip, 4);
  memcpy(ConfigMsg.sub, sub, 4);
  memcpy(ConfigMsg.gw,  gw, 4);
  memcpy(ConfigMsg.mac, mac,6);
  memcpy(ConfigMsg.dns,dns,4);
  /*
  uint8 dhcp;
  uint8 debug;

  uint16 fw_len;
  uint8 state;
  */
  ConfigMsg.dhcp=0;
  ConfigMsg.debug=1;
  ConfigMsg.fw_len=0;
  
  ConfigMsg.state=NORMAL_STATE;
  ConfigMsg.sw_ver[0]=FW_VER_HIGH;
  ConfigMsg.sw_ver[1]=FW_VER_LOW;
  
}

