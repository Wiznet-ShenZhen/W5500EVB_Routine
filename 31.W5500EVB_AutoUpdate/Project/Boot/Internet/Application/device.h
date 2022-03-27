#ifndef _DEVICE_H_
#define _DEVICE_H_
#include "stm32f10x.h"
#include "Types.h"
#include"stdio.h"


#define FW_VER_HIGH   1
#define FW_VER_LOW    1


#define BR_115200_INDEX 			9

#define RS232          		 	0
#define RS485           		1

#define DATABIT_7						7
#define DATABIT_8						8

#define DATABIT_7_INDEX 		0
#define DATABIT_8_INDEX 		1

#define PARITY_NONE					0
#define PARITY_ODD					1
#define PARITY_EVEN					2

#define STOPBIT_0_5						0
#define STOPBIT_1_INDEX 			1
#define STOPBIT_1_5						2
#define STOPBIT_2							3

#define FLOW_NONE       		0
#define FLOW_RTSCTS					1

#define TCP_SERVER_MODE           0
#define TCP_CLIENT_MODE           1
#define TCP_MIX_MODE              2
#define UDP_MODE                  3
#define TCPDISABLE                4

/* UDP port numbers for Remote Configuration */
#define REMOTE_SERVER_PORT	5005	/* from server to client */
#define REMOTE_CLIENT_PORT	1460	/* from client to server */
#define REMOTE_CLIENT_PORT1	1461	/* from client to server */
#define REMOTE_UPDATE_PORT	1470	/* from client to server */

#define REMOTE_FIND 	0
#define REMOTE_SETT 	1
#define REMOTE_SMAC 	2
#define REMOTE_FW 	3

typedef  void (*pFunction)(void);

#define STATIC_MODE 					0
#define DHCP_MODE 						1

#define CONFIG_MSG_SIZE		sizeof(CONFIG_MSG)		

#define EE_OP  0  // Operation Mode

extern uint8 DEFAULT_MAC[6];
extern uint8 DEFAULT_LIP[4];
extern uint8 DEFAULT_MARK[4];
extern uint8 DEFAULT_GW[4];
extern uint8 DEFAULT_DNS[4];
extern uint8 DEFAULT_RIP[4];
extern uint8 DEFAULT_USERNAME[16];
extern uint8 DEFAULT_PWD[16];
extern uint8 DEFAULT_DOMAIN[32];
extern uint16 DEFAULT_LPORT;
extern uint16 DEFAULT_RPORT;
extern uint16 DEFAULT_WEB_PORT;
extern uint8 reboot_flag;
extern uint8 BUFPUB[1024];
#define gBUFPUBLIC BUFPUB;


void set_default(void);
int8 atonum(int8 ch);
void set_network(void);
uint8 get_hex(uint8 b0, uint8 b1);
void write_config_to_eeprom(void);

void reset_w5500(void);

void USART2_Init(uint8 baudrateIndex, uint8 databitIndex,uint8 stopbitIndex, uint8 parityIndex, uint8 flowIndex);
void uart_115200(void);

void reboot_back(void);
void reboot_app(void);
void reboot(void);

void get_config(void);
void do_udp_config(void);
void do_dhcp(void);
void do_dns(void);

void write_change_flag(uint8 flg);
uint8 read_change_flag(void);

void do_fw_update(void);
void set_device_state(uint8 state);
#endif

