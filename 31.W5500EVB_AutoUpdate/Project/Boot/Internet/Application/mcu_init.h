#ifndef _MCU_INIT_
#define _MCU_INIT_
#include "config.h"
/* Private function prototypes ----------------*/

#define PA11			GPIO_Pin_11	// 
#define PA12			GPIO_Pin_12	//
#define PC14				GPIO_Pin_14	

//digital output 
#define DOUT0     PC14 //alive
#define DOUT1     PA11 // run
//digital in 
#define DIN0      PA12 // default


#ifdef  USE_S2E /*定义SPI1作为W5500的硬件接口*/

#define WIZ_SCS									GPIO_Pin_4								/*定义W5500的片选管脚*/
#define WIZ_SCLK								GPIO_Pin_5								/*定义W5500的时钟管脚*/
#define WIZ_MISO								GPIO_Pin_6								/*定义W5500的MISO管脚*/
#define WIZ_MOSI								GPIO_Pin_7								/*定义W5500的MOSI管脚*/

#define WIZ_RESET						  	GPIO_Pin_5								/*定义W5500的RESET管脚*/
#define WIZ_INT							  	GPIO_Pin_4								/*定义W5500的INT管脚*/
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

/* Private functions --------------------------*/
#endif

