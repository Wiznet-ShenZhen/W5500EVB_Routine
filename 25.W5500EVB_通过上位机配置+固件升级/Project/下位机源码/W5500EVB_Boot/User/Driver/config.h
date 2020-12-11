#ifndef __CONFIG_H
#define	__CONFIG_H

#include "string.h"
#include "types.h"
#define ApplicationAddress 	 0x08008000 // 0x08000000-0x08007FFF Boot size is 32K
#define AppBackupAddress     0x08018000 // 0x08008000-0x08017FFF App back start address and size is 64k
#define ConfigAddr		       0x08028000 // 0x08018000-0x08027FFF APP backup and configaddr start address is 64k 

#define FW_VER_HIGH  						1               
#define FW_VER_LOW    					0

typedef struct _CONFIG_MSG											
{
  uint8 op[4];
	uint8 mac[6];																						 /*MAC地址*/
 	uint8 sw_ver[2];																				 /*软件版本号*/
	uint8 lip[4];																						 /*local IP本地IP地址*/
  uint8 sub[4];																						 /*子网掩码*/
  uint8 gw[4];																						 /*网关*/	
  uint8 dns[4];																						 /*DNS服务器地址*/
  uint8 dhcp;
  uint8 debug;
  uint16 fw_len;
  uint8 state;  
}CONFIG_MSG;

#endif
