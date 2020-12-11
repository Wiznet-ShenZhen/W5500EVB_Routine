#ifndef _DEVICE_H_
#define _DEVICE_H_
#include "stm32f10x.h"
#include "Types.h"
#include"stdio.h"
#define DEVICE_ID "W5500"
extern uint8 BUFPUB[1460];

extern uint8  	remote_ip[4];															/*Զ��IP��ַ*/
extern uint16 	remote_port;															/*Զ�˶˿ں�*/
extern uint16		local_port;
extern uint8 	dhcp_ok;

#define CONFIG_MSG_SIZE		sizeof(CONFIG_MSG)-4	
	
#define FW_VER_HIGH  				1
#define FW_VER_LOW    			0
#define LED0  							GPIO_Pin_0	// out
#define LED1								GPIO_Pin_1	// out
#define LED2		    				GPIO_Pin_2	// 
#define LED3								GPIO_Pin_3	// 

#define WIZ_SCS			    		GPIO_Pin_12	// out
#define WIZ_SCLK						GPIO_Pin_13	// out
#define WIZ_MISO						GPIO_Pin_14	// in
#define WIZ_MOSI						GPIO_Pin_15	// out

#define WIZ_RESET		    		GPIO_Pin_9	// out
#define WIZ_INT			    		GPIO_Pin_6	// in
typedef  void (*pFunction)(void);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void set_w5500_mac(void);
void set_default(void);
void set_network(void);
void Reset_W5500(void);
void reboot_app(void);
void reboot(void);
void NVIC_Configuration(void);
void TIM3_Int_Init(u16 arr,u16 psc);

void write_config_to_eeprom(void);
void get_config(void);
#endif

