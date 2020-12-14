#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "stm32f10x.h"
#include "Types.h"
#include "device.h"

//=================================================
// Port define 

// Port A
#define PA0				GPIO_Pin_0	// out
#define PA1				GPIO_Pin_1	// out
#define PA2				GPIO_Pin_2	// 
#define PA3				GPIO_Pin_3	// 
#define PA4				GPIO_Pin_4	// 
#define PA5				GPIO_Pin_5	// 
#define PA6				GPIO_Pin_6	// 
#define PA7				GPIO_Pin_7	// 
#define PA8				GPIO_Pin_8	// 
#define PA9				GPIO_Pin_9	// 
#define PA10			GPIO_Pin_10	// 
#define PA11			GPIO_Pin_11	// 
#define PA12			GPIO_Pin_12	// 
#define PA13			GPIO_Pin_13	// 
#define PA14			GPIO_Pin_14	// 
#define PA15			GPIO_Pin_15	// 



#define USART1_TX		                    PA9	// out
#define USART1_RX		                    PA10	// in 
//#define USART2_TX		                  PA2	// out
//#define USART2_RX		                  PA3	// in 
#define LED0                            PA0
#define LED1                            PA1
#define LED2                            PA2
#define LED3                            PA3
//Port B

#define PB0                             GPIO_Pin_0
#define PB1				                      GPIO_Pin_1	// 
#define PB2				                      GPIO_Pin_2	// 
#define PB3                             GPIO_Pin_3	// 
#define PB4				                      GPIO_Pin_4	// 
#define PB5				                      GPIO_Pin_5	// 
#define PB6				                      GPIO_Pin_6	// 
#define PB7                             GPIO_Pin_7
#define PB8                             GPIO_Pin_8
#define PB9                             GPIO_Pin_9
#define PB10                            GPIO_Pin_10
#define PB11                            GPIO_Pin_11
#define PB12                            GPIO_Pin_12
#define PB13                            GPIO_Pin_13
#define PB14                            GPIO_Pin_14
#define PB15                            GPIO_Pin_15

//SPI2
#define WIZ_SCS			  PB12	// out
#define WIZ_SCLK			PB13	// out
#define WIZ_MISO			PB14	// in
#define WIZ_MOSI			PB15	// out


#define WIZ_RESET		  PB9	// out


//eeprom  PB

#define I2C_SCK	      PB6  //out
#define I2C_SDA	      PB7  //bidirection



//

// Port C
#define PC4         GPIO_Pin_4
#define PC5         GPIO_Pin_5
#define PC6         GPIO_Pin_6
#define PC13				GPIO_Pin_13	//
#define PC14				GPIO_Pin_14	//
#define PC15				GPIO_Pin_15	//

#define WIZ_INT			PC6	// in
//Port D

//=================================================
#pragma pack(1)
typedef struct _CONFIG_MSG
{
  uint8 op[4];//header: FIND;SETT;FACT...
  uint8 mac[6];
  uint8 sw_ver[2];
  uint8 lip[4];
  uint8 sub[4];
  uint8 gw[4];
  uint8 dns[4];	
  uint8 dhcp;
  uint8 debug;

  uint16 fw_len;
  uint8 state;
  
}CONFIG_MSG;
#pragma pack()

#define CONFIG_MSG_LEN        sizeof(CONFIG_MSG) - 4 // the 4 bytes OP will not save to EEPROM
//



#define MAX_BUF_SIZE		1460
#define KEEP_ALIVE_TIME	        30	// 30sec


#define ON	1
#define OFF	0

#define HIGH		1
#define LOW		  0

//#define __GNUC__

// SRAM address range is 0x2000 0000 ~ 0x2000 BFFF (48KB)
#define SOCK_BUF_ADDR 	0x20000000


extern CONFIG_MSG  ConfigMsg, RecvMsg;

#define SOCK_DHCP             0
#define SOCK_HTTP             1

#define AppBackAddress          0x08020000 //from 128K
#define ConfigAddr		          0x0800FC00


#define NORMAL_STATE            0
#define NEW_APP_IN_BACK         1 //there is new app in back address
#define CONFIGTOOL_FW_UP        2 //configtool update f/w in app

#endif

