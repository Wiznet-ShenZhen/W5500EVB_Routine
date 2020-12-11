#ifndef _W5500_CONF_H_
#define _W5500_CONF_H_

#include "stm32f10x.h"
#include "stdio.h"
#include "types.h"
#define __GNUC__

typedef  void (*pFunction)(void);

extern uint8  	remote_ip[4];					/*Զ��IP��ַ*/
extern uint16 	remote_port;					/*Զ�˶˿ں�*/
extern uint16		local_port;						/*���屾�ض˿�*/
extern uint8  	use_dhcp;							/*�Ƿ�ʹ��DHCP��ȡIP*/
extern uint8  	use_eeprom;						/*�Ƿ�ʹ��EEPROM�е�IP������Ϣ*/


//#define STM32_SPI1
#define  STM32_SPI2	

/*W5500��STM32�� SPI1Ӳ�����߶���*/
//	W5500_SCS    --->		STM32_GPIOA4								 /*W5500��Ƭѡ�ܽ�*/
//	W5500_SCLK	 --->	  STM32_GPIOA5								 /*W5500��ʱ�ӹܽ�*/
//  W5500_MISO	 --->		STM32_GPIOA6								 /*W5500��MISO�ܽ�*/
//	W5500_MOSI	 --->		STM32_GPIOA7								 /*W5500��MOSI�ܽ�*/
//	W5500_RESET	 --->   STM32_GPIOC0								 /*W5500��RESET�ܽ�*/
//	W5500_INT		 --->		STM32_GPIOC1								 /*W5500��INT�ܽ�*/

/*W5500��STM32�� SPI2Ӳ�����߶���*/
//	W5500_SCS 	--->	 STM32_GPIOB12								 /*W5500��Ƭѡ�ܽ�*/
//	W5500_SCLK	--->	 STM32_GPIOB13								 /*W5500��ʱ�ӹܽ�*/
//  W5500_MISO	--->	 STM32_GPIOB14								 /*W5500��MISO�ܽ�*/
//	W5500_MOSI	--->	 STM32_GPIOB15								 /*W5500��MOSI�ܽ�*/
//	W5500_RESET	--->   STM32_GPIOB9								   /*W5500��RESET�ܽ�*/
//	W5500_INT		--->   STM32_GPIOC6								   /*W5500��INT�ܽ�*/

#define NORMAL_STATE            0
#define NEW_APP_IN_BACK         1           //there is new app in back address
#define ApplicationAddress 	    0x08008000  // 0x08000000-0x08007FFF Boot size is 32K
#define AppBackupAddress        0x08018000  // 0x08008000-0x08017FFF App back start address and size is 64k
#define ConfigAddr		          0x08028000  // 0x08018000-0x08027FFF APP backup and configaddr start address is 64k 
#define VerAddr                 0x08029000  // �汾��Ϣ�洢   

#define FW_VER_HIGH  						1               
#define FW_VER_LOW    					0
#define ON	                 		1
#define OFF	                 		0
#define HIGH	           	 			1
#define LOW		             			0

#define MAX_BUF_SIZE		 				1460											  /*����ÿ�����ݰ��Ĵ�С*/
#define KEEP_ALIVE_TIME	     		30	                        // 30sec
#define TX_RX_MAX_BUF_SIZE      2048							 
#define EEPROM_MSG_LEN        	sizeof(EEPROM_MSG)

#define IP_FROM_DEFINE	        0                          /*ʹ�ó�ʼ�����IP��Ϣ*/
#define IP_FROM_DHCP	          1                          /*ʹ��DHCP��ȡIP��Ϣ*/
#define IP_FROM_EEPROM	        2							             /*ʹ��EEPROM�����IP��Ϣ*/
extern uint8	ip_from;											               /*ѡ��IP��Ϣ����Դ*/
#pragma pack(1)
/*�˽ṹ�嶨����W5500�ɹ����õ���Ҫ����*/
typedef struct _CONFIG_MSG											
{
  uint8 op[4];
	uint8 mac[6];																						 /*MAC��ַ*/
  uint8 sw_ver[2];																				 /*����汾��*/
	uint8 lip[4];																						 /*local IP����IP��ַ*/
  uint8 sub[4];																						 /*��������*/
  uint8 gw[4];																						 /*����*/	
  uint8 dns[4];																						 /*DNS��������ַ*/
  //uint8 rip[4];																						 /*remote IPԶ��IP��ַ*/

  uint8 dhcp;
  uint8 debug;
  uint16 fw_len;
  uint8 state;
}CONFIG_MSG;



#pragma pack()

#pragma pack(1)
/*�˽ṹ�嶨����eepromд��ļ����������ɰ����޸�*/
typedef struct _EEPROM_MSG	                    
{
	uint8 mac[6];																							/*MAC��ַ*/
  uint8 lip[4];																							/*local IP����IP��ַ*/
  uint8 sub[4];																							/*��������*/
  uint8 gw[4];																							/*����*/
}EEPROM_MSG_STR;
#pragma pack()

extern EEPROM_MSG_STR EEPROM_MSG;
extern CONFIG_MSG  	ConfigMsg,RecvMsg;
extern uint8 dhcp_ok;																				/*DHCP��ȡ�ɹ�*/
extern uint32	dhcp_time;																		/*DHCP���м���*/
extern vu8	ntptimer;																				/*NPT�����*/

/*MCU������غ���*/
void gpio_for_w5500_config(void);														/*SPI�ӿ�reset ���ж�����*/
void timer2_init(void);																			/*STM32��ʱ��2��ʼ��*/
void timer2_isr(void);																			/*��ʱ���ж�ִ�еĺ���*/
void reboot(void);																					/*STM32��λ*/
void write_config_to_eeprom(void);													/*д������Ϣ��EEPROM��*/
void read_config_from_eeprom(void);													/*��EEPROM�ж�����Ϣ*/

/*W5500SPI��غ���*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data);						/*д��һ��8λ���ݵ�W5500*/
uint8 IINCHIP_READ(uint32 addrbsb);													/*��W5500����һ��8λ����*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len);	/*��W5500д��len�ֽ�����*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len);	/*��W5500����len�ֽ�����*/

/*W5500����������غ���*/
void reset_w5500(void);																			/*Ӳ��λW5500*/
void set_w5500_mac(void);																		/*����W5500��MAC��ַ*/
void set_w5500_ip(void);																		/*����W5500��IP��ַ*/

/*��Ҫ�ö�ʱ�ĵ�Ӧ�ú���*/
void dhcp_timer_init(void);																	/*dhcp�õ��Ķ�ʱ����ʼ��*/
void ntp_timer_init(void);																	/*npt�õ��Ķ�ʱ����ʼ��*/


void set_default(void);
void set_network(void);
uint32 FLASH_PagesMask(vu32 Size);
void write_config_to_flash(void);
void reboot_app(void);
void copy_app(uint32 fw_len);
void get_config(void);
void get_vernn(void);
void write_ver_to_flash(void);

#endif
