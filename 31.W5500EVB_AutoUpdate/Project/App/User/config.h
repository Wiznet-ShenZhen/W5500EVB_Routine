#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "types.h"
#include "device.h"

#define ApplicationAddress 			0x08008000//boot size is 32K
#define AppBackupAddress        0x08024000 //app size is 112K: (256-32)/2

#define USE_EVB// EVB SPI2 UART1

//socket define
#define SOCK_HTTP               0
#define W5500_UPDATE            1
#pragma pack(1)

typedef struct _DATE_TIME
{
	uint16 year;
	uint8 month;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;
}DATE_TIME;
#pragma pack()
extern DATE_TIME Data_Time;
#pragma pack(1)
typedef struct _NET_PARAM
{
	uint8 dhcp;  //�����͹ر�DHCP
	uint8 ip[4]; //����IP
	uint8 sub[4];//��������
	uint8 gw[4]; //����
	  //uint8 mac[6];
	uint8 dns[4];//DNS������
}NET_PARAM;

typedef struct _SEC_PARAM
{
	//���ڿ���
	uint8 baudrate;  //������
	uint8 databit;   //����λindex,0:7 bits, 1:8 bits
	uint8 parity;    //У��λ
	uint8 stopbit;   //ֹͣλ
	uint8 flowcontrol;//Ӳ��������
	//S2E
	uint8  mode;      //ͨ��ģʽtcp client, tcp server, udp. 
	uint16 lport;    //���ض˿ں� 
	uint8  rip[4];   //Զ��IPremote ip address
	uint16 rport;    //Զ�̶˿ں�remmote port number
	uint8 dns_flag;  //
	uint8 domain[32];//����

	//s2e options
	uint16 inactivity;     //���ӽ�����û���շ����ݵ�ʱ�� 0-60000 unit: ms 
	uint16 reconn;         //TCP�ͻ�������ʱ�� 0-60000 unit: ms 
	uint16 nagle_time;     //���ڽ���ֹͣʱ�� 0-60000  unit: ms 
	uint16 pack_len;		   //��������ʱ�ְ�
	uint16 keepalive_time; //����ʱ��         0-255  unit: 5s 
	uint8  bind_port;			 //TCP�ͻ��˰󶨱��ض˿ں�

	uint8  link_clearbuf;		//�������Ӻ��Ƿ����buff��0��������գ�1�������
	uint8  link_password;		//���ӽ����Ժ��Ƿ�У�����룬0������У�飻 1����У��
	uint8  link_message;    //0�����������Ӻ󲻷�����Ϣ�� 1�����������Ӻ����豸ID; 2��������MAC��ַ��  3�����������Ӻ����豸IP;
	uint8  link_condition;	//0�����ϵ���������ӣ� 1���������յ����ݺ����� TCPCģʽ��Ч			

}SEC_PARAM;

/*
  module state flag
  0xff: only boot, first run! without mac address
  0x00: in boot and without app, waiting for app
  0x01: app inside, app running
  0x02: f/w received, boot should copy f/w from backup flash area to app area
	0x80: normal standby state, app running
  0x82: remote s2e host connected (TCP)
*/
//=================================================

typedef struct _CONFIG_MSG
{
	uint8 op[4];//header: FIND;SETT;FACT;FIRM...
	uint8 mac[6];//MAC��ַ
	char device_id[16];//�豸ID
	uint8 state;//���״̬
  /****************************************************************************************/
  /********** ONLY BELOW PARAMS INCLUDED IN CRC16 CACULATION AND STORED IN FLASH **********/
	uint8 sw_ver[2];

	char device_type[16];  //�豸���� W5500S2E ���ɸ���
	char device_name[16];  //�豸����
	//char username[16];   //web config username
	char pwd[16];        //device password web and at 

	uint16 http_port;    //http server port numberdefault: 80
	uint32 fw_len;      //received firmware length from http or firmware server
	uint32 fw_checksum; //calculated checksum of received firmware
	uint8 debug_flag;   //debug message
	uint8	echo_flag;		//���Ա�־λ	0�����رջ��ԣ� 1�����򿪻���  
	uint8	netbios_flag;	//==1 ����netbios����
	NET_PARAM net;      //device network parameters
	SEC_PARAM sec1;
	//crc16	
	uint8 start_mode;    //��������ģʽ 0 AT����ģʽ 1 ����ģʽ
	uint16 crc16;
	//IP

}CONFIG_MSG;

typedef struct _WORK_STATE
{
	uint32_t netsend_num;			//��̫�������ֽ���
	uint32_t netrcv_num;			//��̫�������ֽ���
	uint32_t sec1_send_num;		//���ڷ����ֽ���
	uint32_t sec1_rcv_num;		//���ڽ����ֽ���
	uint32 on_time;					  //�豸��������������ʱ��  unit:s
	uint8 run_mode;	          //�豸��ǰ����ģʽ
	uint8 temperature;        //�豸�¶�
}WORK_STATE;
#pragma pack()

extern CONFIG_MSG  ConfigMsg, RecvMsg;
extern WORK_STATE  WorkState;


#define CONFIG_MSG_LEN        sizeof(CONFIG_MSG) - 4 // the 4 bytes OP will not save to EEPROM
#define CONFIG_MSG_LEN_ALL		sizeof(CONFIG_MSG)
#define WORK_STATE_MSG_LEN				sizeof(WORK_STATE)
#define NO_NEED_CRC_MSG_LEN   2

#define RULE_START_ADDR       CONFIG_MSG_LEN
#define RULE_LEN              sizeof(USER_RULE)
#define TOTAL_RULE_NUM        16

#define BOOT_WITHOUT_APP      0x00
#define FW_UPDATE_CONFIG      0x10
#define WEBPAGE_UPDATE_CONFIG 0x11
//module state definination
#define BOOT_WITHOUT_APP      0x00
#define FW_RXD_HTTP           0x02
#define FW_RXD_SERVER         0x03
#define FW_APP_NORMAL         0x80
//#define 
#define FLASH_PAGE_SIZE       0x800


#define HTTP_PWD_TIMEOUT        5 //5 minutes
#define CONSOLE_TIMEOUT         HTTP_PWD_TIMEOUT
#define MAX_BUF_SIZE						1460
#define KEEP_ALIVE_TIME	        30	// 30sec
#define WWDG_TIMEOUT_COUNTER    1000//58*2 seconds

#define ON											1
#define OFF											0

#define HIGH										1
#define LOW											0

//SRAM address range is 0x2000 0000 ~ 0x2000 BFFF (48KB)
#define SOCK_BUF_ADDR 	0x2000C000
#define SIO_BUF_ADDR 	(SOCK_BUF_ADDR + MAX_BUF_SIZE)
#define TX_RX_MAX_BUF_SIZE	2048

#define ApplicationAddress			0x08008000  //boot size is 32K
#define ApplicationBackAddress  0x0801C000 //total app size is 112K, 80K app program and 32KB webpage space included, app program is from 0x08008000 to 0x0801BFFF

#endif


