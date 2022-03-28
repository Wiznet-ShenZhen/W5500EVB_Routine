#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "stm32f10x.h"
#include "Types.h"
#include "device.h"
#define __GNUC__

#define UDP_LISTEN_PORT					5000
#define SOCK_SSDP								0	// UDP
#define SOCK_UPNP								1	// TCP
#define SOCK_UPNP_EVENTING			2	// TCP
#define SOCK_CONFIG							2	// UDP
#define SOCK_DNS								2	// UDP
#define SOCK_DHCP								3	// UDP

#define ON											1
#define OFF											0
#define HIGH										1
#define LOW											0
#define SUCCESS									1
#define FAIL										0

#define CONFIG_MSG_LEN        	sizeof(CONFIG_MSG) - 4 // the 4 bytes OP will not save to EEPROM
#define TX_RX_MAX_BUF_SIZE			2048
#define MAX_BUF_SIZE						1460
#define KEEP_ALIVE_TIME	        30	// 30sec
// SRAM address range is 0x2000 0000 ~ 0x2000 BFFF (48KB)
#define SOCK_BUF_ADDR 					0x20000000
#define AppBackAddress          0x08020000 //from 128K
#define ConfigAddr							0x0800FC00
#define NORMAL_STATE            0
#define NEW_APP_IN_BACK         1 //there is new app in back address
#define CONFIGTOOL_FW_UP        2 //configtool update f/w in app

extern uint8 TX_BUF[TX_RX_MAX_BUF_SIZE];
extern uint8 RX_BUF[TX_RX_MAX_BUF_SIZE];

//=================================================
typedef struct _CONFIG_MSG
{
  uint8 op[4];//header: FIND;SETT;FACT...
  uint8 mac[6];
  uint8 sw_ver[2];
  uint8 lip[4];
  uint8 sub[4];
  uint8 gw[4];
  uint8 dns[4];	
  uint8 dhcp;
  uint8 debug;

  uint16 fw_len;
  uint8 state;
 
}CONFIG_MSG;
extern CONFIG_MSG  ConfigMsg, RecvMsg;
#pragma pack()
typedef struct _CONFIG_TYPE_DEF
{
	uint16 port;
	uint8 destip[4];
}CHCONFIG_TYPE_DEF;

#endif


