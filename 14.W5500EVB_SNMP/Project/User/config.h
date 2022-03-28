#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "stm32f10x.h"
#include "Types.h"
#include "device.h"

#pragma pack(1)
typedef struct _CONFIG_MSG
{
  uint8 op[4];//header: FIND;SETT;FACT...
  uint8 mac[6];
  uint8 sw_ver[2];
  uint8 device_name[16];
  uint8 device_serial[16];
  uint8 lip[4];
  uint8 sub[4];
  uint8 gw[4];
  uint8 dns[4];	
  uint8 dhcp;
  uint8 debug;

  uint16 fw_len;
  uint8 state;
  
  uint8 baudrate;
  uint8 databit; //index,0:7 bits, 1:8 bits
  uint8 parity;
  uint8 stopbit;
  uint8 flow;
 
  //password for web/telnet config
  uint8 pwd[16];//md5 digest
  
  //communication
  uint8 mode; //tcp client, tcp server, udp. 
  uint16 lport;
  uint8 rip[4]; //remote ip address
  uint16 rport; //remmote port number
  uint8 dns_flag;
  uint8 domain[32];
}CONFIG_MSG;
typedef struct _CONFIG_TYPE_DEF
{
	uint16 port;
	uint8 destip[4];
}CHCONFIG_TYPE_DEF;
#define TX_RX_MAX_BUF_SIZE	2048
//#define TX_BUF	0x20004000
//#define RX_BUF	(TX_BUF+TX_RX_MAX_BUF_SIZE)
extern uint8 TX_BUF[TX_RX_MAX_BUF_SIZE];
extern uint8 RX_BUF[TX_RX_MAX_BUF_SIZE];
#pragma pack()

#define CONFIG_MSG_LEN        sizeof(CONFIG_MSG) - 4 // the 4 bytes OP will not save to EEPROM

#define MAX_BUF_SIZE		 1460
#define KEEP_ALIVE_TIME	     30	// 30sec
							 
#define ON	                 1
#define OFF	                 0

#define HIGH	           	 1
#define LOW		             0

#define __GNUC__

// SRAM address range is 0x2000 0000 ~ 0x2000 BFFF (48KB)
#define SOCK_BUF_ADDR 	      0x20000000

extern CONFIG_MSG  ConfigMsg, RecvMsg;

//#define SOCK_HUMTEM             0       //Humidity and temperature
//#define SOCK_TCP	        	1	// TCP
//#define SOCK_DNS		        2	// UDP
//#define SOCK_DHCP		        3	// UDP
//#define SOCK_LOC                4       //tcp
//#define SOCK_NTP                5       //time
//#define SOCK_TELNET             6       //tcp, telnet
//#define SOCK_HTTP               7

#define SOCK_DHCP             0
#define SOCK_HTTP             1
#define SOCK_SNMP	      3

#define AppBackAddress        0x08020000 //from 128K
#define ConfigAddr		      0x0800FC00


#define NORMAL_STATE          0
#define NEW_APP_IN_BACK       1 //there is new app in back address
#define CONFIGTOOL_FW_UP      2 //configtool update f/w in app

#endif


