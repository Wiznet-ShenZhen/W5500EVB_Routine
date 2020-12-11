#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "stm32f10x.h"
#include "Types.h"
#include "device.h"
#define __GNUC__

#define FW_VER_HIGH  				2
#define FW_VER_LOW    			0

#define ConfigAddr		      		0x0800FC00
#define CONFIG_MSG_SIZE		sizeof(CONFIG_MSG)-4
	
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
#endif


