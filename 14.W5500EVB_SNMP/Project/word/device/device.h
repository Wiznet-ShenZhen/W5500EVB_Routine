#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "stm32f10x.h"
#include"stdio.h"
#define DEVICE_ID "W5500"

#define FW_VER_HIGH  				1
#define FW_VER_LOW    			0
#define LED0                PAout(0)// PB5
#define LED1                PAout(1)// PE5	
#define LED2                PAout(2)// PB5
#define LED3                PAout(3)// PB5

#define WIZ_SCS			    		GPIO_Pin_12	// out
#define WIZ_SCLK						GPIO_Pin_13	// out
#define WIZ_MISO						GPIO_Pin_14	// in
#define WIZ_MOSI						GPIO_Pin_15	// out

#define WIZ_RESET		    		GPIO_Pin_9	// out
#define WIZ_INT			    		GPIO_Pin_6	// in
typedef  void (*pFunction)(void);
void GPIO_Configuration(void);
void set_network(void);
void write_config_to_eeprom(void);
void set_default(void);
void Reset_W5500(void);
void reboot(void);
void get_config(void);
#endif

