#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "stm32f10x.h"
#include "Types.h"
#include "device.h"

//#define USE_S2E// 模块 
#define USE_EVB// EVB

#define __GNUC__

#pragma pack(1)
typedef struct _DATE_TIME
{
  uint16 year;
  uint8 month;
  uint8 day;
  uint8 hour;
  uint8 minute;
  uint8 second;
}DATE_TIME;

typedef struct _NET_PARAM
{
  uint8 dhcp;
  uint8 ip[4];
  uint8 sub[4];
  uint8 gw[4];
  uint8 dns[4];
}NET_PARAM;

typedef struct _SEC_PARAM
{
    //串口控制
	uint8 baudrate;  //波特率
	uint8 databit;   //数据位index,0:7 bits, 1:8 bits
  uint8 parity;    //校验位
  uint8 stopbit;   //停止位
  uint8 flowcontrol;//硬件流控制
  //S2E
  uint8 mode;       //通信模式tcp client, tcp server, udp. 
  uint16 lport;    //本地端口号 
  uint8  rip[4];   //远程IPremote ip address
  uint16 rport;    //远程端口号remmote port number
  uint8 dns_flag;  //链接建立后 没有收发数据的时间
  uint8 domain[32]; //域名

  //s2e options
  uint16 inactivity;     //链接建立后 没有收发数据的时间
 	uint16 reconn;        //TCP客户端重连时间 unit: ms
	uint16 nagle_time;     //串口接受停止时间
	uint16 pack_len;		   //连续接收时分包
	uint16 keepalive_time; //心跳时间 0-255
	uint8  bind_port;				//TCP客户端绑定本地端口号
	
	uint8  link_clearbuf;		//建立连接后是否清空buff，0——不清空；1——清空
	uint8  link_password;		//连接建立以后是否校验密码，0——不校验； 1——校验
	uint8  link_message;    //0——建立连接后不发送信息； 1——建立连接后发送设备ID; 2——发送MAC地址；  3——建立连接后发送设备IP;
	uint8  link_condition;	//0——上电后立即连接； 1——串口收到数据后连接   TCPC模式有效			
}SEC_PARAM;


//=================================================
typedef struct _CONFIG_MSG
{
  uint8 op[4];//header: FIND;SETT;FACT;FIRM...
	uint8 mac[6]; 
	char device_id[16];//设备串号  	
	uint8 state;
  /*
  module state flag 
  0xff: only boot, first run! without mac address
  0x00: in boot and without app, waiting for app
  0x01: app inside, app running
  0x02: f/w received, boot should copy f/w from backup flash area to app area
	0x80: normal standby state, app running
  0x82: remote s2e host connected (TCP)
  */
  /****************************************************************************************/
  /********** ONLY BELOW PARAMS INCLUDED IN CRC16 CACULATION AND STORED IN FLASH **********/
  uint8 sw_ver[2];
  char device_type[16]; //设备类型 W5500S2E 不可更改

  char device_name[16];//设备名称
  //char username[16];   //web config username
  char pwd[16];        //device password web and at 
  uint16 http_port;    //http server port numberdefault: 80
	
  uint32 fw_len;       //received firmware length from http or firmware server
  uint32 fw_checksum;  //calculated checksum of received firmware
	
  uint8 debug_flag;    //debug message
	uint8	echo_flag;		 //回显标志位	0——关闭回显， 1——打开回显  
	uint8	netbios_flag;	 //==1 开启netbios服务
	
	NET_PARAM net;       //device network parameters
  SEC_PARAM sec1;
//  uint8 start_mode;    //开机工作模式 0 AT命令模式 1 数据模式
 //crc16
//  uint16 crc16;

}CONFIG_MSG;
typedef struct _WORK_STATE
{
  uint32_t netsend_num;			//以太网发送字节数
  uint32_t netrcv_num;			//以太网接收字节数
  uint32_t sec1_send_num;		//串口发送字节数
	uint32_t sec1_rcv_num;		//串口接收字节数
  uint32_t on_time;					//设备正常工作的运行时间
	uint8 run_mode;	          //设备当前工作模式
	uint8 temperature;        //设备温度
}WORK_STATE;
#pragma pack()



#define CONFIG_MSG_LEN        	sizeof(CONFIG_MSG) - 4 // the 4 bytes OP will not save to EEPROM
#define CONFIG_MSG_LEN_ALL		sizeof(CONFIG_MSG)

#define NO_NEED_CRC_MSG_LEN   	2
//module state definination
#define BOOT_WITHOUT_APP      	0x00
#define FW_RXD_HTTP           	0x02
#define FW_RXD_SERVER         	0x03
#define FW_APP_STATE        	0x80
#define FW_UPDATE_CONFIG     	  0x10
#define EEPROM_CRC_ERROR				0x20
#define WEBPAGE_UPDATE_CONFIG 	0x11
//#define 

//socket define

#define SOCK_DATA               0       //	
#define SOCK_CONFIG							1	// UDP
#define SOCK_DNS								2	// UDP
#define SOCK_DHCP								3	// UDP
#define SOCK_LOC                4     //tcp
#define SOCK_FW                 5     // F/W update socket
#define SOCK_HTTP               7

#define HTTP_PWD_TIMEOUT        5 //5 minutes
#define CONSOLE_TIMEOUT         HTTP_PWD_TIMEOUT
#define MAX_BUF_SIZE						1460
#define KEEP_ALIVE_TIME	        30	// 30sec
#define WWDG_TIMEOUT_COUNTER    1000//58*2 seconds

#define ON											1
#define OFF											0

#define HIGH										1
#define LOW											0

// SRAM address range is 0x2000 0000 ~ 0x2000 BFFF (48KB)
#define SOCK_BUF_ADDR 	0x2000C000
#define SIO_BUF_ADDR 	(SOCK_BUF_ADDR + MAX_BUF_SIZE)

#define TX_RX_MAX_BUF_SIZE	2048
//#define TX_BUF	0x20004000
//#define RX_BUF	(TX_BUF+TX_RX_MAX_BUF_SIZE)
//extern uint8 TX_BUF[TX_RX_MAX_BUF_SIZE];
//extern uint8 RX_BUF[TX_RX_MAX_BUF_SIZE];
extern CONFIG_MSG  ConfigMsg, RecvMsg;

#define ApplicationAddress 			0x08008000//boot size is 32K
#define AppBackupAddress        0x0801C000 //app size is 112K: (256-32)/2

#endif


