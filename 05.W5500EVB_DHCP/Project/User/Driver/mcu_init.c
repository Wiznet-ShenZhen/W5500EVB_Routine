#include "stm32f10x.h"
#include "config.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

vuint32 ms=0;
vuint8 sec=0;
vuint8 min=0;
vuint8 hour=0;

extern uint32  dhcp_time;
/*
*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the GPIO.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************
*/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  // Port B output
  GPIO_InitStructure.GPIO_Pin = WIZ_SCS; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_SetBits(GPIOA, WIZ_SCS);//SPI1
	
  // Port A output
  GPIO_InitStructure.GPIO_Pin =LED0 | LED1 | LED2 | LED3; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOA, LED0);
  GPIO_ResetBits(GPIOA, LED1);
  GPIO_SetBits(GPIOA, LED2); // led off
  GPIO_SetBits(GPIOA, LED3); // led off
 
  // Configure the GPIO ports( USART1 Transmit and Receive Lines)
  // Configure the USART1_Tx as Alternate function Push-Pull 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin =  USART1_TX;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  // Configure the USART1_Rx as input floating
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = USART1_RX;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure SPIy pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = WIZ_SCLK | WIZ_MISO | WIZ_MOSI;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  //SPI2

  // Port B output;
  GPIO_InitStructure.GPIO_Pin = WIZ_RESET ; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, WIZ_RESET);//SPI2
  
  // Port C input
  GPIO_InitStructure.GPIO_Pin = WIZ_INT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
}
/*
*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************
*/
void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;
  
 /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);
    
      //adc
  
#if defined (STM32F10X_LD_VL) || (STM32F10X_LD_VL) || (STM32F10X_LD_VL)
  RCC_ADCCLKConfig(RCC_PCLK2_Div2);
#else
  RCC_ADCCLKConfig(RCC_PCLK2_Div4);
#endif

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_APB1Periph_TIM2 | RCC_APB1Periph_USART2 | RCC_APB1Periph_SPI2, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
  											|RCC_APB2Periph_AFIO  | RCC_APB2Periph_USART1, ENABLE);
}
/*
*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************
*/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);

#else  /* VECT_TAB_FLASH  */
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  /* Set WWDG interrupt vector Preemption Priority to 1 */
  NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

  
  /* Set the Vector Table base location at 0x08002000 -> USE AIP*/ 
  //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);  
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);  //only app, no boot included
#endif
}
/*
*******************************************************************************
* Function Name  : Timer_Configuration
* Description    : Timer configuration.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************
*/
void Timer_Configuration(void)
{

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 , ENABLE);
  // Time base configuration 
  TIM_TimeBaseStructure.TIM_Period = 1000;          
  TIM_TimeBaseStructure.TIM_Prescaler = 0;      
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  // Prescaler configuration 
  TIM_PrescalerConfig(TIM2, 71, TIM_PSCReloadMode_Immediate);

	// TIM IT enable 
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  
	
  // TIM enable counter 
  TIM_Cmd(TIM2, ENABLE); 

}
/*
*******************************************************************************
* Function Name  : Timer2_ISR
* Description    : Timer Interrupt service function.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************
*/
void Timer2_ISR(void)
{
  ms++;//mili 
  if(ms>=1000)//second
  {  
    ms=0;
    sec++;
    dhcp_time++; 
  }
  if(sec>=60)//minute
  {
    min++; 
    sec=0;
    
  }
  if(min>=60)//hour
  {
    min=0;
    hour++;
  }
  if(hour>=24)//day
  {
    hour=0;    
  }
}

