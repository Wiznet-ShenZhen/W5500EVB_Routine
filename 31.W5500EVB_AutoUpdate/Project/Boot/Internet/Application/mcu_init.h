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


#ifdef  USE_S2E /*����SPI1��ΪW5500��Ӳ���ӿ�*/

#define WIZ_SCS									GPIO_Pin_4								/*����W5500��Ƭѡ�ܽ�*/
#define WIZ_SCLK								GPIO_Pin_5								/*����W5500��ʱ�ӹܽ�*/
#define WIZ_MISO								GPIO_Pin_6								/*����W5500��MISO�ܽ�*/
#define WIZ_MOSI								GPIO_Pin_7								/*����W5500��MOSI�ܽ�*/

#define WIZ_RESET						  	GPIO_Pin_5								/*����W5500��RESET�ܽ�*/
#define WIZ_INT							  	GPIO_Pin_4								/*����W5500��INT�ܽ�*/
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

