#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "stm32f10x.h"
#include "Types.h"
#include"stdio.h"

#define LED0  					GPIO_Pin_0	// out
#define LED1						GPIO_Pin_1	// out
#define LED2		    		GPIO_Pin_2	// 
#define LED3						GPIO_Pin_3	// 

extern	uint8 newname[20];
extern	uint8 oldname[20];
	
typedef  void (*pFunction)(void);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void reboot(void);

void write_config_to_eeprom(void);
void get_config(void);
void LED(void);
#endif

