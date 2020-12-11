#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "stm32f10x.h"
#include "Types.h"
#include "device.h"

#define __GNUC__

#pragma pack(1)
typedef struct _CONFIG_MSG
{
  uint8 op[4];//header: FIND;SETT;FACT...
  uint8 mac[6];
  uint8 sw_ver[2];
  uint8 newfirm[2];	
  uint8 lip[4];
  uint8 sub[4];
  uint8 gw[4];
  uint8 dns[4];
	uint8 filename[20];
  uint8 dhcp;
  uint8 debug;

  uint16 fw_len;
  uint8 state;
  
}CONFIG_MSG;
#pragma pack()
extern CONFIG_MSG  ConfigMsg, RecvMsg;

typedef struct _TFTP_MSG
{
  uint8 lip[4];
  uint16 filename[10]; 
}TFTP_MSG;
extern TFTP_MSG  tftp;

#define MAX_BUF_SIZE			1460
#define KEEP_ALIVE_TIME	        30	// 30sec

#define ON						1
#define OFF						0
#define HIGH					1
#define LOW						0
// SRAM address range is 0x2000 0000 ~ 0x2000 4FFF (20KB)
//#define SOCK_BUF_ADDR 			    0x08006000
#define ApplicationAddress  		0x08008000 //app size is 112K, 32KB webpage space included, app program is from 0x08008000 to 0x0801C000
#define ConfigAddr		      		0x0800FC00
#define SOCK_OK               	1        ///< Result is OK about socket process.
#define SOCK_BUSY             	0        ///< Socket is busy on processing the operation. Valid only Non-block IO Mode.
#define SOCK_FATAL            	-1000    ///< Result is fatal error about socket process.

#endif


