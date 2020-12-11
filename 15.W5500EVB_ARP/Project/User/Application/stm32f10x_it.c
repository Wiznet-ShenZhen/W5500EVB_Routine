
#include "stm32f10x_it.h"
#include "types.h"

extern uint8 tx_arp_flag;
void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}


void DebugMon_Handler(void)
{
}


void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
}
/*
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)
	{
		ISR();//ÖÐ¶Ï´¦Àí

		EXTI_ClearITPendingBit(EXTI_Line6);       
	}                  
}
*/

void TIM2_IRQHandler(void)
{
	vu32 t_msec=0;
	vu8 t_sec=0,t_min=0,t_hour=0;
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		t_msec++;//mili second
  
  if(t_msec>=1000)//second
  {  
    t_msec=0;
    t_sec++;
    tx_arp_flag=1;//set arp flag
  }
  if(t_sec>=60)//minute
  {
    t_sec=0;
    t_min++;
    
  }
  if(t_min>=60)//hour
  {
    t_min=0;
    t_hour++;
  }
  if(t_hour>=24)//day
  {
    t_hour=0;
  }
	}
        //printf("1");
}

