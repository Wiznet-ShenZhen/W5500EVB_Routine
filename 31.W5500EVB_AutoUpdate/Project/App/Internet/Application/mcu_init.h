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

#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 

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

#ifdef  USE_S2E /*定义SPI1作为W5500的硬件接口*/
#define WIZ_SCS									GPIO_Pin_4								/*定义W5500的片选管脚*/
#define WIZ_SCLK								GPIO_Pin_5								/*定义W5500的时钟管脚*/
#define WIZ_MISO								GPIO_Pin_6								/*定义W5500的MISO管脚*/
#define WIZ_MOSI								GPIO_Pin_7								/*定义W5500的MOSI管脚*/

#define WIZ_RESET							GPIO_Pin_5								/*定义W5500的RESET管脚*/
#define WIZ_INT								GPIO_Pin_4								/*定义W5500的INT管脚*/
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

