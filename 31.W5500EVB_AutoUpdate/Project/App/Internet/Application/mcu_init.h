#ifndef _MCU_INIT_
#define _MCU_INIT_
#include "config.h"
/* Private function prototypes ---------------------*/

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) 

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //

#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 

#define PA11			GPIO_Pin_11	// 
#define PA12			GPIO_Pin_12	//
#define PC14			GPIO_Pin_14	

//digital output 
//digital in 
#define DOUT0     PA11 // run
#define DOUT1     PC14 //alive
#define DIN0      PA12 // default
#define KEY0      PAin(12)

#define LED0      PAout(11)// 
#define LED1      PCout(14)//	

#ifdef  USE_S2E /*����SPI1��ΪW5500��Ӳ���ӿ�*/
#define WIZ_SCS									GPIO_Pin_4								/*����W5500��Ƭѡ�ܽ�*/
#define WIZ_SCLK								GPIO_Pin_5								/*����W5500��ʱ�ӹܽ�*/
#define WIZ_MISO								GPIO_Pin_6								/*����W5500��MISO�ܽ�*/
#define WIZ_MOSI								GPIO_Pin_7								/*����W5500��MOSI�ܽ�*/

#define WIZ_RESET							GPIO_Pin_5								/*����W5500��RESET�ܽ�*/
#define WIZ_INT								GPIO_Pin_4								/*����W5500��INT�ܽ�*/
#endif

#ifdef  USE_EVB 
#define WIZ_SCS			    		GPIO_Pin_12	// out
#define WIZ_SCLK						GPIO_Pin_13	// out
#define WIZ_MISO						GPIO_Pin_14	// in
#define WIZ_MOSI						GPIO_Pin_15	// out

#define WIZ_RESET		    		GPIO_Pin_9	// out
#define WIZ_INT			    		GPIO_Pin_6	// in
#endif

void NVIC_Configuration(void);
void GPIO_Configuration(void);
void IWDG_Configureation(void);

/* Private functions -------------------------------*/
#endif

