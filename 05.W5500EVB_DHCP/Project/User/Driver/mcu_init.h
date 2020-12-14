#ifndef _MCU_INIT_
#define _MCU_INIT_

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void UART2_RS232_Configration(void);
void UART2_RS485_Configration(void);
void Timer_Configuration(void);
void Timer2_ISR(void);
void DMA_Configuration(unsigned long ADCConvertedValue);
void ADC_Configuration(void);
/* Private functions ---------------------------------------------------------*/
#endif

